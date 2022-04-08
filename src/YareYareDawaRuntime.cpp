//
// Created by balik on 4/8/2022.
//

#include "YareYareDawaRuntime.h"
#include "util.h"

void YareYareDawaRuntime::runFile(std::string_view filePath) {
    std::string content;

    try {
        content = util::getFileContentAsString(filePath);
    } catch (std::invalid_argument const &ex) {
        errorState = true;
        std::cerr << ex.what() << "\n";
        return;
    }
    errorState = run(content);
}

void YareYareDawaRuntime::runPrompt() {
    bool scanLine = true;
    while (scanLine) {
        std::cout << "> ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        scanLine = run(line);
    }
    if (scanLine) {
        std::cout << "Yare Yare Dawa...\n";
    }
}

bool YareYareDawaRuntime::run(std::string_view source) {
    // dummy todo:
    std::cout << source << '\n';
    errorState = true;
    return true;
}
