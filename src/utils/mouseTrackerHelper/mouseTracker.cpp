#include "mouseTracker.hpp"
#include "../common/common.hpp"
#include <fstream>
#include <iostream>

void MouseTracker::handleEvent(const XEvent& event) {
    if (event.type == GenericEvent && event.xcookie.evtype == XI_RawMotion) {
        Window root_return, child_return;
        int root_x_return, root_y_return;
        int win_x_return, win_y_return;
        unsigned int mask_return;

        if (XQueryPointer(event.xcookie.display, DefaultRootWindow(event.xcookie.display), 
                          &root_return, &child_return, 
                          &root_x_return, &root_y_return, 
                          &win_x_return, &win_y_return, &mask_return)) {
            std::string timestamp = getCurrentTimestamp();
            trajectory_.push_back({timestamp, {root_x_return, root_y_return}});
        }
    }
}

void MouseTracker::saveTrajectory(const std::string& filename) {
    if (trajectory_.empty()) {
        std::cout << "Debug: Trajectory is empty, nothing to save." << std::endl;
        return;
    }

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing mouse trajectory." << std::endl;
        return;
    }

    for (const auto& point : trajectory_) {
        outFile << "[" << point.first << "] - X: " << point.second.first 
                << ", Y: " << point.second.second << "\n";
    }
    
    outFile.close();
    std::cout << "Mouse trajectory saved to " << filename << std::endl;
}
