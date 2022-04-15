//
// Created by balik on 4/15/2022.
//

#ifndef YARE_YARE_DAWA_ERRORS_H
#define YARE_YARE_DAWA_ERRORS_H

#include <iostream>
#include "scanning/Token.h"

class RuntimeError : public std::runtime_error {
public:
    const scanning::Token &token;

    RuntimeError(const scanning::Token &token, std::string_view message)
            : std::runtime_error{message.data()}, token{token} {}
};


class Errors {
public:
    /*=== error handling ===*/
    static bool isError;
    static bool hadRuntimeError;

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

    static void errorRuntime(const RuntimeError &error) {
        std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
        hadRuntimeError = true;
    }
};

// todo: fix inline
inline bool Errors::isError = false;
inline bool Errors::hadRuntimeError = false;

#endif //YARE_YARE_DAWA_ERRORS_H
