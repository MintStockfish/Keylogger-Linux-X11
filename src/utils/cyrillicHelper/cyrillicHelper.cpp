#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include "cyrillicHelper.hpp"

const char* keysym_to_utf8_cyrillic(KeySym keysym) {
    switch (keysym) {
        case XK_Cyrillic_a:        return "а";
        case XK_Cyrillic_be:       return "б";
        case XK_Cyrillic_ve:       return "в";
        case XK_Cyrillic_ghe:      return "г";
        case XK_Cyrillic_de:       return "д";
        case XK_Cyrillic_ie:       return "е";
        case XK_Cyrillic_io:       return "ё";
        case XK_Cyrillic_zhe:      return "ж";
        case XK_Cyrillic_ze:       return "з";
        case XK_Cyrillic_i:        return "и";
        case XK_Cyrillic_shorti:   return "й";
        case XK_Cyrillic_ka:       return "к";
        case XK_Cyrillic_el:       return "л";
        case XK_Cyrillic_em:       return "м";
        case XK_Cyrillic_en:       return "н";
        case XK_Cyrillic_o:        return "о";
        case XK_Cyrillic_pe:       return "п";
        case XK_Cyrillic_er:       return "р";
        case XK_Cyrillic_es:       return "с";
        case XK_Cyrillic_te:       return "т";
        case XK_Cyrillic_u:        return "у";
        case XK_Cyrillic_ef:       return "ф";
        case XK_Cyrillic_ha:       return "х";
        case XK_Cyrillic_tse:      return "ц";
        case XK_Cyrillic_che:      return "ч";
        case XK_Cyrillic_sha:      return "ш";
        case XK_Cyrillic_shcha:    return "щ";
        case XK_Cyrillic_hardsign: return "ъ";
        case XK_Cyrillic_yeru:     return "ы";
        case XK_Cyrillic_softsign: return "ь";
        case XK_Cyrillic_e:        return "э";
        case XK_Cyrillic_yu:       return "ю";
        case XK_Cyrillic_ya:       return "я";

        case XK_Cyrillic_A:        return "А";
        case XK_Cyrillic_BE:       return "Б";
        case XK_Cyrillic_VE:       return "В";
        case XK_Cyrillic_GHE:      return "Г";
        case XK_Cyrillic_DE:       return "Д";
        case XK_Cyrillic_IE:       return "Е";
        case XK_Cyrillic_IO:       return "Ё";
        case XK_Cyrillic_ZHE:      return "Ж";
        case XK_Cyrillic_ZE:       return "З";
        case XK_Cyrillic_I:        return "И";
        case XK_Cyrillic_SHORTI:   return "Й";
        case XK_Cyrillic_KA:       return "К";
        case XK_Cyrillic_EL:       return "Л";
        case XK_Cyrillic_EM:       return "М";
        case XK_Cyrillic_EN:       return "Н";
        case XK_Cyrillic_O:        return "О";
        case XK_Cyrillic_PE:       return "П";
        case XK_Cyrillic_ER:       return "Р";
        case XK_Cyrillic_ES:       return "С";
        case XK_Cyrillic_TE:       return "Т";
        case XK_Cyrillic_U:        return "У";
        case XK_Cyrillic_EF:       return "Ф";
        case XK_Cyrillic_HA:       return "Х";
        case XK_Cyrillic_TSE:      return "Ц";
        case XK_Cyrillic_CHE:      return "Ч";
        case XK_Cyrillic_SHA:      return "Ш";
        case XK_Cyrillic_SHCHA:    return "Щ";
        case XK_Cyrillic_HARDSIGN: return "Ъ";
        case XK_Cyrillic_YERU:     return "Ы";
        case XK_Cyrillic_SOFTSIGN: return "Ь";
        case XK_Cyrillic_E:        return "Э";
        case XK_Cyrillic_YU:       return "Ю";
        case XK_Cyrillic_YA:       return "Я";
        
        default:
            return NULL; 
    }
}