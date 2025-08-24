#include "./common.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring> 
#include <ctime>
#include <memory>
#include <array>
#include <functional>

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

void writeLog(const std::string& log_buffer, const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "Не удалось открыть файл для записи!" << std::endl;
        return;
    }
    file << log_buffer;
    file.close();
}

void trimLeadingNewlines(std::string& s) {
    size_t first_char = s.find_first_not_of('\n');

    if (std::string::npos != first_char) {
        s.erase(0, first_char);
    } else {
        s.clear();
    }
}