#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <X11/Xlib.h>
#include "../utils/keyboardEventHelper/keyboardEventHelper.hpp" 
#include <string>

class Application {
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

private:
    void initializeX11();
    void processEvents();
    void handleAction(const ProcessedEvent& result);
    void updateActiveWindow();

    Display* display_ = nullptr;
    Window rootWindow_;
    bool isRunning_ = true;

    KeyboardEventHandler keyboardHandler_;
    std::string logBuffer_;
    std::string currentWindowName_;
};

#endif 