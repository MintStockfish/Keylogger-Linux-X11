#include <iostream>
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>



int main() {
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Не удалось подключиться к X-серверу." << std::endl;
        return 1;
    }

    int xi_opcode, event, error;
    if (!XQueryExtension(display, "XInputExtension", &xi_opcode, &event, &error)) {
        std::cerr << "Расширение XInput не доступно." << std::endl;
        XCloseDisplay(display);
        return 1;
    }
    

    Window root = DefaultRootWindow(display);
    
    XIEventMask evmask;
    unsigned char mask_bytes[XI_LASTEVENT] = {0}; 

    XISetMask(mask_bytes, XI_RawKeyPress);
    XISetMask(mask_bytes, XI_RawKeyRelease);

    evmask.deviceid = XIAllMasterDevices; 
    evmask.mask_len = sizeof(mask_bytes);
    evmask.mask = mask_bytes;

    XISelectEvents(display, root, &evmask, 1);
    XFlush(display);

    std::cout << "Пассивный перехват запущен. Нажимайте клавиши..." << std::endl;
    std::cout << "Нажмите Escape для выхода." << std::endl;

  
    bool isShiftPressed = false;
  
    XEvent xevent;
    while (true) {
        XNextEvent(display, &xevent);
        
        if (xevent.xcookie.type == GenericEvent && xevent.xcookie.extension == xi_opcode) {
            XGetEventData(display, &xevent.xcookie);

            if (xevent.xcookie.evtype == XI_RawKeyPress) {
                XIRawEvent* rawev = (XIRawEvent*)xevent.xcookie.data;

                KeySym keysym = XkbKeycodeToKeysym(display, rawev->detail, 0, (int)isShiftPressed);
                
                if (keysym != NoSymbol) {
                    const char* key_string = XKeysymToString(keysym);
                    printf("Нажата клавиша: symbol=%s\n",
                           key_string ? key_string : "Unknown");

                    if (keysym == XK_Shift_L) {
                        isShiftPressed = true;
                    }

                    if (keysym == XK_Escape) {
                        XFreeEventData(display, &xevent.xcookie);
                        break; 
                    }
                }
            } 

            if (xevent.xcookie.evtype == XI_RawKeyRelease) {
                XIRawEvent* rawev = (XIRawEvent*)xevent.xcookie.data;
                KeySym keysym = XkbKeycodeToKeysym(display, rawev->detail, 0, (int)isShiftPressed);
                const char* key_string = XKeysymToString(keysym);
                printf("release:%s\n", key_string);

                if (keysym == XK_ISO_Next_Group) {
                    isShiftPressed = false;
                }
            }
            
            XFreeEventData(display, &xevent.xcookie);
        }
    }

    std::cout << "Завершение работы..." << std::endl;
    XCloseDisplay(display);
    return 0;
}