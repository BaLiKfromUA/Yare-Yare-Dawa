//
// Created by balik on 4/8/2022.
//

#ifndef YARE_YARE_DAWA_YAREYAREDAWARUNTIME_H
#define YARE_YARE_DAWA_YAREYAREDAWARUNTIME_H


#include <string_view>
#include <iostream>

#include "scanning/Token.h"
#include "scanning/TokenType.h"

class YareYareDawaRuntime {
public:
    /*=== error handling ===*/
    static bool isError;

    static void report(int line, std::string_view where,
                       std::string_view message) {
        std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
        isError = true;
    }

    static void error(int line, std::string_view message) {
        report(line, "", message);
    }

    static void error(const scanning::Token &token, std::string_view message) {
        if (token.type == scanning::END_OF_FILE) {
            report(token.line, " at end", message);
        } else {
            report(token.line, " at '" + token.lexeme + "'", message);
        }
    }

    /*=== entry points ===*/
    void runFile(std::string_view filePath);

    void runPrompt();

private:
    void run(std::string_view source);
};

// todo: fix inline
inline bool YareYareDawaRuntime::isError = false;

#endif //YARE_YARE_DAWA_YAREYAREDAWARUNTIME_H
