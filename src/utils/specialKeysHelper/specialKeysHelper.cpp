#include <string>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include "specialKeysHelper.hpp"

std::string handleSpecialKeys(KeySym keysym) {
    switch (keysym) {
        case XK_space:      return " ";
        case XK_Return:     return "\n";
        case XK_Tab:        return "\t";
        case XK_comma:      return ",";
        case XK_period:     return ".";
        case XK_exclam:     return "!";
        case XK_question:   return "?";
        case XK_colon:      return ":";
        case XK_semicolon:  return ";";
        case XK_minus:      return "-";
        case XK_underscore: return "_";
        case XK_plus:       return "+";
        case XK_equal:      return "=";
        case XK_slash:      return "/";
        case XK_backslash:  return "\\";
        case XK_quoteleft:  return "`";
        case XK_quoteright: return "'";
        case XK_quotedbl:   return "\"";
        case XK_bracketleft:return "[";
        case XK_bracketright:return "]";
        case XK_braceleft:  return "{";
        case XK_braceright: return "}";
        case XK_bar:        return "|";
        case XK_less:       return "<";
        case XK_greater:    return ">";
        case XK_parenleft:  return "(";
        case XK_parenright: return ")";
        case XK_at:         return "@";
        case XK_BackSpace:  return " BackSpace ";
        default:            return ""; 
    }
}