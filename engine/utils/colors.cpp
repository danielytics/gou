
#include "colors.hpp"

glm::vec3 colors::parse_rgb (const std::string& hex_string)
{
    if (hex_string.length() != 7 && hex_string[0] != '#') {
        return {};
    }
    std::string red = hex_string.substr(1, 2);
    std::string green = hex_string.substr(3, 2);
    std::string blue = hex_string.substr(5, 2);
    return glm::vec3{
        std::stoi(red, nullptr, 16) / 255.0f,
        std::stoi(green, nullptr, 16) / 255.0f,
        std::stoi(blue, nullptr, 16) / 255.0f,
    };
}
