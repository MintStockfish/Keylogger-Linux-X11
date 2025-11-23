#ifndef MOUSE_TRACKER_HPP
#define MOUSE_TRACKER_HPP

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <vector>
#include <string>
#include <utility>

class MouseTracker {
public:
    MouseTracker() = default;
    ~MouseTracker() = default;

    void handleEvent(const XEvent& event);
    void saveTrajectory(const std::string& filename);

private:
    std::vector<std::pair<std::string, std::pair<int, int>>> trajectory_;
};

#endif 
