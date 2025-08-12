#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <memory> 
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "processTracking.hpp"


std::string get_window_title(Display* display, Window window) {
    if (window == None) {
        return "";
    }

    Atom net_wm_name_atom = XInternAtom(display, "_NET_WM_NAME", False);
    Atom utf8_string_atom = XInternAtom(display, "UTF8_STRING", False);

    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char* prop_data = nullptr;
    
    std::string title = ""; 

    int status = XGetWindowProperty(display, window, net_wm_name_atom, 0, 1024, False,
                                    AnyPropertyType, &actual_type, &actual_format,
                                    &nitems, &bytes_after, &prop_data);


    if (status == Success && prop_data != nullptr && actual_type == utf8_string_atom) {
        title.assign(reinterpret_cast<char*>(prop_data), nitems);
    }
    

    if (prop_data != nullptr) {
        XFree(prop_data);
    }

    return title;
}

Window get_active_window(Display* display) {
    Window root = DefaultRootWindow(display);
    Atom net_active_window_atom = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);

    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char* prop_data = nullptr;

    int status = XGetWindowProperty(display, root, net_active_window_atom, 0, 1, False,
                                    XA_WINDOW, &actual_type, &actual_format,
                                    &nitems, &bytes_after, &prop_data);

    Window active_window = None;

    if (status == Success && prop_data != nullptr && nitems > 0) {
        active_window = *reinterpret_cast<Window*>(prop_data);
    }

    if (prop_data != nullptr) {
        XFree(prop_data);
    }

    return active_window;
}

std::string windowTracking(Display* display) {

    while (true) {
        Window activeWindow = get_active_window(display);
        std::string currentTitle = get_window_title(display, activeWindow);
        
        return currentTitle;
    }

    return 0;
}