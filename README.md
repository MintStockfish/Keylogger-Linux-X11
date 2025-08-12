# C++ Keylogger for X11

This is a simple passive keylogger for X11-based operating systems (Linux), written in C++. The program captures keystrokes using the XInput2 extension, correctly handling both Cyrillic and Latin layouts, as well as the state of modifier keys (Shift, Caps Lock). Tracking active window where user is typing.

All captured input is written to `log.txt`. The program terminates when the `Escape` key is pressed.

## Dependencies

To build and run this project, you will need:

- **CMake** (version 3.10 or higher)
- A modern C++ compiler (e.g., **GCC/g++** or **Clang**)
- Development libraries for **X11** and **XInput2**

### Installing Dependencies

#### On Debian / Ubuntu:

```bash
sudo apt update
sudo apt install build-essential cmake libx11-dev libxi-dev
```

#### On Fedora / CentOS:

```bash
sudo dnf install cmake gcc-c++ libX11-devel libXi-devel
```

#### On Arch Linux:

```bash
sudo pacman -S base-devel cmake libx11 libxi
```

## Building and Running

1.  **Clone the repository:**

    ```bash
    git clone https://github.com/MintStockfish/Keylogger-Linux-X11
    cd Keylogger-Linux-X11
    ```

2.  **Create a build directory:**

    ```bash
    mkdir build && cd build
    ```

3.  **Configure the project with CMake:**

    ```bash
    cmake ..
    ```

4.  **Compile the project:**

    ```bash
    make
    ```

5.  **Run the keylogger:**
    The executable `keylogger` will be created in the `build` directory.
    ```bash
    ./keylogger
    ```
    The program will start capturing input in the background. Press `Escape` to terminate the program and save the log to `log.txt`.

## Project Structure

```
.
├── CMakeLists.txt      # CMake build script
├── README.md           # This file
├── src/                # Source code directory
│   ├── keylogger.cpp   # Main file with the main() function
│   ├── utils/          # Helper modules
│   └── ├── processTrackingHelper/
│       ├── cyrillicHelper/
│       ├── keyPressHelper/
│       └── specialKeysHelper/
└── .gitignore          # Git ignore file
```
