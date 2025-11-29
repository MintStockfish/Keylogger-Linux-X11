#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <X11/Xlib.h>
#include "../utils/keyboardEventHelper/keyboardEventHelper.hpp" 
#include "../utils/mouseTrackerHelper/mouseTracker.hpp"
#include <string>
#include <functional>

class Application {
public:
    using WindowChangeCallback = std::function<void(const std::string&, const std::string&)>; 
    using KeyPressCallback = std::function<void(const std::string&)>; 

    Application();
    Application(WindowChangeCallback onWindowChange, KeyPressCallback onKeyPress);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();
    void stop();

private:
    void initializeX11();
    void processEvents();
    void handleAction(const ProcessedEvent& result);
    void updateActiveWindow();
    
    Display* display_ = nullptr;
    Window rootWindow_;
    bool isRunning_ = true;

    KeyboardEventHandler keyboardHandler_;
    MouseTracker mouseTracker_;
    std::string logBuffer_;
    std::string currentWindowName_;
    
    WindowChangeCallback onWindowChange_;
    KeyPressCallback onKeyPress_;
};

#endif 