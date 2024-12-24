#include <utils.h>

std::string extractContentLength(const std::string& response) {
    auto to_lower = [](char c) { return std::tolower(static_cast<unsigned char>(c)); };
    std::string lower_response = response;
    std::transform(lower_response.begin(), lower_response.end(), lower_response.begin(), to_lower);

    size_t pos = lower_response.find("content-length:");
    if (pos != std::string::npos) {
        size_t end_of_line = response.find("\n", pos);
        if (end_of_line != std::string::npos) {
            std::string content_length_line = response.substr(pos, end_of_line - pos);
            size_t colon_pos = content_length_line.find(":");
            if (colon_pos != std::string::npos) {
                std::string value = content_length_line.substr(colon_pos + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                return value;
            }
        }
    }
    return ""; 
}