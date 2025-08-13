#pragma once

#include <string>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/XKBlib.h>

struct KeyPressResult {
    std::string char_to_log;      
    const char* printable_name = nullptr; 
    bool should_exit = false;   
    bool should_grabBuffer = false;  
};

KeyPressResult process_key_press(Display* display, XIRawEvent* rawev, const XkbStateRec& state, bool ctrlPressed);