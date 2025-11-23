# Keylogger-Linux-X11

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey.svg)

A passive keylogger for Linux operating systems using the X11 display server. This tool is written in modern C++ and designed as an educational example of how to interact with the X server at a low level.

The program captures keystrokes using the `XInput2` extension, correctly handling various keyboard layouts (e.g., Cyrillic, Latin) and modifier states (Shift, Caps Lock). It also tracks the active window title, captures clipboard content on `Ctrl+C`, takes screenshots when the `PrintScreen` key is pressed, and records the mouse cursor trajectory.

> **Disclaimer:** This software is intended for educational purposes and security research only. Using it to monitor individuals without their explicit consent is illegal and unethical. The author is not responsible for any malicious use of this program.

## Features

- **Passive Keystroke Logging:** Captures all keyboard events without grabbing the keyboard, meaning it doesn't interfere with other applications.
- **Multi-Layout Support:** Correctly interprets characters from different keyboard layouts (tested with Latin and Cyrillic).
- **Active Window Tracking:** Logs the title of the window where the user is currently typing.
- **Clipboard Capture:** Logs the content of the clipboard when `Ctrl+C` is pressed.
- **Screenshot on PrintScreen:** Automatically saves a screenshot to the `screenshots/` directory when the `PrintScreen` key is hit.
- **Mouse Trajectory Tracking:** Records the path of the mouse cursor and saves it to `mouse_log.txt` upon exit.
- **Clean Output:** All captured data is saved to a `log.txt` file in the project's root directory.
- **Safe Exit:** The program terminates gracefully when the `Escape` key is pressed.

## Prerequisites

To build and run this project, you will need:

- **CMake** (version 3.10 or higher)
- A modern C++ compiler (e.g., **g++** or **clang**)
- Development libraries for **X11** and **XInput2**
- Helper tools: **`xclip`** (for clipboard access) and **`scrot`** (for screenshots)

### Installing Dependencies

#### On Debian / Ubuntu:

```bash
sudo apt update
sudo apt install build-essential cmake libx11-dev libxi-dev xclip scrot
```

#### On Fedora / CentOS / RHEL:

```bash
sudo dnf install cmake gcc-c++ libX11-devel libXi-devel xclip scrot
```

#### On Arch Linux:

```bash
sudo pacman -S base-devel cmake libx11 libxi xclip scrot
```

## Building and Running

1.  **Clone the repository:**

    ```bash
    git clone https://github.com/MintStockfish/Keylogger-Linux-X11
    cd Keylogger-Linux-X11
    ```

2.  **Create a build directory:**
    This keeps the project root clean from build artifacts.

    ```bash
    mkdir build && cd build
    ```

3.  **Configure and build the project:**

    ```bash
    cmake ..
    make
    ```

4.  **Run the keylogger:**
    The executable `keylogger` will be created in the `build` directory.

    ```bash
    ./keylogger
    ```

    The program will now run in your terminal, capturing input passively. The log file (`log.txt`) and `screenshots/` directory will be created in the build folder.

5.  **Stop the program:**
    Press the `Escape` key in any window to terminate the program and save the final log.

## How It Works

The keylogger attaches to the root window of the X server and listens for raw keyboard events (`XI_RawKeyPress`, `XI_RawKeyRelease`) using the **XInput2** extension. This method is "passive" because the program only listens to events as they are delivered by the server, rather than intercepting or "grabbing" the keyboard, which would prevent other applications from receiving input. This makes the logger undetectable from a user experience perspective.

Mouse tracking is implemented using `XI_RawMotion` events to detect movement and `XQueryPointer` to retrieve global coordinates, ensuring the cursor is tracked even when hovering over other windows.

## Project Structure

The project follows a clean, modular structure to separate concerns.

```
.
├── CMakeLists.txt              # CMake build script
├── README.md                   # This file
├── src/                        # Source code directory
│   ├── main.cpp                # Main entry point - creates and runs the Application
│   ├── app/                    # Core application logic
│   │   ├── application.hpp
│   │   └── application.cpp
│   └── utils/                  # Helper modules and utilities
│       ├── common/             # Common functions (exec, timestamp, etc.)
│       ├── keyboardEventHelper/  # Logic for processing keyboard events
│       ├── cyrillicHelper/       # Mapping for Cyrillic keysyms
│       ├── specialKeysHelper/  # Mapping for special keys (Enter, Space, etc.)
│       ├── windowTrackingHelper/ # Logic for getting the active window title
│       └── mouseTrackerHelper/   # Logic for tracking mouse trajectory
└── .gitignore                  # Git ignore file
```
