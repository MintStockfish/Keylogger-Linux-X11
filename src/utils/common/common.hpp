#pragma once
#include <string>

std::string exec(const char* cmd);
std::string getCurrentTimestamp();
void writeLog(const std::string& log_buffer, const std::string& filepath);
void trimLeadingNewlines(std::string& s); 