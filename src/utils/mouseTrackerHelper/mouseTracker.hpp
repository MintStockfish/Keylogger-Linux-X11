#ifndef MOUSE_TRACKER_HPP
#define MOUSE_TRACKER_HPP

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <vector>
#include <map>
#include <string>
#include <utility>

#include <functional>

class MouseTracker {
public:
    MouseTracker() = default;
    ~MouseTracker() = default;

    void handleEvent(const XEvent& event);
    void saveClicks(const std::string& filename);
    void setClickCallback(std::function<void(int, int)> callback);

private:
    std::map<std::pair<int, int>, int> clicks_;
    std::function<void(int, int)> onClickCallback_;
};

#endif 
