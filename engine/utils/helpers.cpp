#include "utils/helpers.hpp"

#define PHYFSPP_IMPL
#include <physfs.hpp>

#include <exception>

std::string helpers::readToString(const std::string& filename)
{
    if (physfs::exists(filename)) {
        physfs::ifstream stream(filename);
        return std::string(std::istreambuf_iterator<char>(stream),
                        std::istreambuf_iterator<char>());
    } else {
        auto message = std::string{"File could not be read: "} + filename;
        throw std::invalid_argument(message);
    }
}