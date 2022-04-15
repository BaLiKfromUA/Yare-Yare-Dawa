//
// Created by balik on 4/8/2022.
//

#include "YareYareDawaRuntime.h"
#include "scanning/Scanner.h"
#include "scanning/Token.h"
#include "parsing/Parser.h"
#include "util.h"
#include "ast/AstPrinter.h"

void YareYareDawaRuntime::runFile(std::string_view filePath) {
    std::string content;

    try {
        content = util::getFileContentAsString(filePath);
    } catch (std::invalid_argument const &ex) {
        isError = true;
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
        isError = false;
    }

    std::cout << "Yare Yare Dawa...\n";
}

void YareYareDawaRuntime::run(std::string_view source) {
    auto scanner = scanning::Scanner(source);
    auto tokens = scanner.scanTokens();

    for (const auto &token: tokens) {
        std::cout << token.toString() << '\n';
    }

    auto parser = parsing::Parser(tokens);
    auto expression = parser.parse();

    // Stop if there was a syntax error.
    if (isError) return;

    std::cout << ast::AstPrinter().print(expression);
}