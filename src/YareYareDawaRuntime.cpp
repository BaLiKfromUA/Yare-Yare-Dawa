//
// Created by balik on 4/8/2022.
//

#include "YareYareDawaRuntime.h"
#include "Errors.h"
#include "scanning/Scanner.h"
#include "parsing/Parser.h"
#include "util.h"

void YareYareDawaRuntime::runFile(std::string_view filePath) {
    std::string content;

    try {
        content = util::getFileContentAsString(filePath);
    } catch (std::invalid_argument const &ex) {
        Errors::isError = true;
        std::cerr << ex.what() << "\n";
        return;
    }

    run(content);
}

void YareYareDawaRuntime::runPrompt() {
    while (true) {
        std::cout << "> ";
        std::string line;
        // todo: improve termination
        if (!std::getline(std::cin, line) || line == "exit") break;
        run(line);
        Errors::isError = false;
    }

    std::cout << "Yare Yare Dawa...\n";
}

void YareYareDawaRuntime::run(std::string_view source) {
    auto scanner = scanning::Scanner(source);
    auto tokens = scanner.scanTokens();

    auto parser = parsing::Parser(tokens);
    auto expression = parser.parse();

    // Stop if there was a syntax error.
    if (Errors::isError) return;
    // Stop if there was a runtime error.
    if (Errors::hadRuntimeError) return;

    _interpreter.interpret(expression);
}
