//
// Created by balik on 4/8/2022.
//

#ifndef YARE_YARE_DAWA_YAREYAREDAWAMANAGER_H
#define YARE_YARE_DAWA_YAREYAREDAWAMANAGER_H


#include <string_view>
#include <iostream>

#include "scanning/Token.h"
#include "scanning/TokenType.h"
#include "interpreter/Interpreter.h"


class YareYareDawaManager {
public:
    /*=== entry points ===*/
    void runFile(std::string_view filePath);

    void runPrompt();

private:
    interpreter::Interpreter _interpreter{};

    void run(std::string_view source);
};

#endif //YARE_YARE_DAWA_YAREYAREDAWAMANAGER_H
