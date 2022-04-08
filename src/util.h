//
// Created by balik on 4/8/2022.
//

#ifndef YARE_YARE_DAWA_UTIL_H
#define YARE_YARE_DAWA_UTIL_H

#include <string>
#include <iostream>
#include <cstring>
#include <fstream>

namespace util {
    inline std::string getFileContentAsString(std::string_view path) {
        std::ifstream file{path.data(), std::ios::in | std::ios::binary | std::ios::ate};
        if (!file) {
            throw std::invalid_argument("Failed to open file "
                                        + std::string(path.begin(), path.end())
                                        + ": " + std::strerror(errno));
        }

        std::string content;
        content.resize(file.tellg());

        file.seekg(0, std::ios::beg);
        file.read(content.data(), content.size());

        return content;
    }
}

#endif //YARE_YARE_DAWA_UTIL_H
