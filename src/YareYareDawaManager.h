//
// Created by balik on 4/8/2022.
//

#ifndef YARE_YARE_DAWA_YAREYAREDAWAMANAGER_H
#define YARE_YARE_DAWA_YAREYAREDAWAMANAGER_H


#include <string_view>
#include <iostream>

#include "scanning/Token.h"
#include "scanning/TokenType.h"
#include "visitor/Interpreter.h"
#include "visitor/CodeGenerator.h"

enum RunMode {
    INTERPRETER,
    COMPILER
};

class YareYareDawaManager {
public:
    explicit YareYareDawaManager(RunMode mode) {
        if (mode == INTERPRETER) {
            _visitor = new visitor::Interpreter();
        } else {
#ifdef __linux__
            _visitor = new visitor::CodeGenerator();
#else
            throw std::runtime_error("Compiler Mode supported only for Linux!");
#endif
        }
    }

    ~YareYareDawaManager() {
        delete _visitor;
    }

    /*=== entry points ===*/
    void runFile(std::string_view filePath);

    void runPrompt();

private:
    visitor::AstVisitor *_visitor = nullptr;

    void run(std::string_view source);
};

#endif //YARE_YARE_DAWA_YAREYAREDAWAMANAGER_H
