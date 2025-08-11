#include "keyPressHelper.hpp"
#include "../cyrillicHelper/cyrillicHelper.hpp"
#include "../specialKeysHelper/specialKeysHelper.hpp" 
#include <cstring> 

KeyPressResult process_key_press(Display* display, XIRawEvent* rawev, const XkbStateRec& state) {
    KeyPressResult result;

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

    if (keysym == XK_Escape) {
        result.should_exit = true;
        result.printable_name = "Escape";
        return result;
    }

    std::string special_char = handleSpecialKeys(keysym);
    if (!special_char.empty()) {
        result.char_to_log = special_char;
        result.printable_name = result.char_to_log.c_str(); 
    } else {
        const char* cyrillic_char = keysym_to_utf8_cyrillic(keysym);
        if (cyrillic_char) {
            result.char_to_log = cyrillic_char;
            result.printable_name = cyrillic_char;
        } else {
            const char* key_name = XKeysymToString(keysym);
            result.printable_name = key_name;

            if (key_name && strlen(key_name) == 1) {
                result.char_to_log = key_name;
            }
        }
    }
    
    return result;
}