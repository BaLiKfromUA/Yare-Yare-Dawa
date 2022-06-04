#include "YareYareDawaFunction.h"
#include "visitor/Environment.h"
#include "visitor/Interpreter.h"
#include "YareYareDawaReturn.h"

std::string visitor::YareYareDawaFunction::toString() {
    return "<fn " + declaration->name.lexeme + ">";
}

int visitor::YareYareDawaFunction::arity() {
    return static_cast<int>(declaration->params.size());
}

std::any visitor::YareYareDawaFunction::call(visitor::Interpreter &interpreter, std::vector<std::any> arguments) {
    auto environment = std::make_shared<Environment<std::any>>(closure);
    for (int i = 0; i < declaration->params.size(); ++i) {
        auto [type, value] = declaration->params[i];
        bool isValidType = std::any_cast<bool>(interpreter.validateType(type.type, arguments[i], false));

        if (isValidType) {
            environment->define(value.lexeme,
                                arguments[i]);
        } else {
            throw RuntimeError(type, "expected input type doesn't match with given");
        }
    }

    try {
        interpreter.executeBlock(declaration->body, environment);
    } catch (YareYareDawaReturn &returnValue) {
        auto result = returnValue.value;
        bool isValidType = std::any_cast<bool>(interpreter.validateType(declaration->returnType.type, result, true));

        if (isValidType) {
            return result;
        } else {
            throw RuntimeError(declaration->returnType, "expected return type doesn't match with given");
        }
    }

    return nullptr;
}


