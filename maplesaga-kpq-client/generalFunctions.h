#pragma once
#include <iostream>
#include <sstream>
#include <vector>

std::vector<unsigned char> hexStringToBytes(const std::string& hexString);
void removeSubstring(std::string& str, const std::string& substr);
void replaceSubstring(std::string& str, const std::string& toReplace, const std::string& replacement);