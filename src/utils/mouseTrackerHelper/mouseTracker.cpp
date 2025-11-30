#include "mouseTracker.hpp"
#include "../common/common.hpp"
#include <fstream>
#include <iostream>

void MouseTracker::handleEvent(const XEvent& event) {
    if (event.type == GenericEvent && event.xcookie.evtype == XI_RawButtonPress) {
        Window root_return, child_return;
        int root_x_return, root_y_return;
        int win_x_return, win_y_return;
        unsigned int mask_return;

        if (XQueryPointer(event.xcookie.display, DefaultRootWindow(event.xcookie.display), 
                          &root_return, &child_return, 
                          &root_x_return, &root_y_return, 
                          &win_x_return, &win_y_return, &mask_return)) {
            
            clicks_[{root_x_return, root_y_return}]++;
            
            if (onClickCallback_) {
                onClickCallback_(root_x_return, root_y_return);
            }

            std::cout << "Click detected at: " << root_x_return << ", " << root_y_return 
                      << " | Count: " << clicks_[{root_x_return, root_y_return}] << std::endl;
        }
    }
}

void MouseTracker::setClickCallback(std::function<void(int, int)> callback) {
    onClickCallback_ = callback;
}

void MouseTracker::saveClicks(const std::string& filename) {
    if (clicks_.empty()) {
        std::cout << "Debug: No clicks recorded, nothing to save." << std::endl;
        return;
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing mouse clicks." << std::endl;
        return;
    }

    for (const auto& entry : clicks_) {
        outFile << "X: " << entry.first.first 
                << ", Y: " << entry.first.second 
                << " - Count: " << entry.second << "\n";
    }
    
    outFile.close();
    std::cout << "Mouse clicks saved to " << filename << std::endl;
}
