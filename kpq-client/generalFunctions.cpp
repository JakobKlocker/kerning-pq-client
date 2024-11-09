#include "pch.h"
#include "generalFunctions.h"

int hexCharToInt(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    else if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    else {
        return -1; // Invalid character
    }
}

// Function to convert a string of hexadecimal bytes into actual bytes
std::vector<unsigned char> hexStringToBytes(const std::string& hexString) {
    std::string sanitizedString;

    // Remove spaces from the input string
    for (char c : hexString) {
        if (c != ' ') {
            sanitizedString.push_back(c);
        }
    }

    std::vector<unsigned char> bytes;

    for (size_t i = 0; i < sanitizedString.size(); i += 2) {
        // Convert two hexadecimal characters into a byte
        int highNibble = hexCharToInt(sanitizedString[i]);
        int lowNibble = hexCharToInt(sanitizedString[i + 1]);

        if (highNibble == -1 || lowNibble == -1) {
            // Invalid character found
            std::cerr << "Invalid hexadecimal character encountered: " << sanitizedString[i] << sanitizedString[i + 1] << std::endl;
            return {};
        }

        unsigned char byte = (highNibble << 4) | lowNibble;
        bytes.push_back(byte);
    }

    return bytes;
}

void removeSubstring(std::string& str, const std::string& substr) {
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos) {
        str.erase(pos, substr.length());
    }
}

void replaceSubstring(std::string& str, const std::string& toReplace, const std::string& replacement) {
    size_t pos = 0;
    while ((pos = str.find(toReplace, pos)) != std::string::npos) {
        str.replace(pos, toReplace.length(), replacement);
        pos += replacement.length();
    }
}