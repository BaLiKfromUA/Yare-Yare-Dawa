#include <iostream>
#include "YareYareDawaManager.h"

int main(int argc, char *argv[]) {
    if (argc > 3) {
        std::cout << "Usage: yyd [[-mode] script_name.yyd]\n";
        return 64;
    }

    RunMode mode = INTERPRETER;

    if (argc == 3) {
        std::string modeStr = argv[1];
        if (modeStr == "-c") {
            mode = COMPILER;
        } else if (modeStr != "-i") {
            std::cerr << "Unknown mode :" << modeStr << "\n";
            return 64;
        }
    }

    YareYareDawaManager runtime(mode);

    if (argc == 3) {
        runtime.runFile(argv[2]);
    } else if (argc == 2) {
        runtime.runFile(argv[1]);
    } else {
        runtime.runPrompt();
    }

    int returnCode = Errors::isError;
    return returnCode;
}
