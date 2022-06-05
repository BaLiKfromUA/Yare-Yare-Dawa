//
// Created by balik on 05.06.22.
//

#include "StandardLibrary.h"
#include "visitor/Interpreter.h"

namespace visitor {
    std::any ToStringHelper::call(Interpreter &interpreter, std::vector<std::any> arguments) {
        return visitor::Interpreter::stringify(arguments[0]);
    }
}