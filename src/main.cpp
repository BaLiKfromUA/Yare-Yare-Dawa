#include <iostream>
#include "YareYareDawaManager.h"

int main(int argc, char *argv[]) {
    YareYareDawaManager runtime;

    if (argc > 2) {
        std::cout << "Usage: yyd [script_name.yyd]\n";
        return 64;
    } else if (argc == 2) {
        runtime.runFile(argv[1]);
    } else {
        runtime.runPrompt();
    }

    int returnCode = Errors::isError;
    return returnCode;
}
