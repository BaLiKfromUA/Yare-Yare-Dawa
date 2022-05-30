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
        environment->define(declaration->params[i].lexeme,
                            arguments[i]);
    }

    try {
        interpreter.executeBlock(declaration->body, environment);
    } catch (YareYareDawaReturn &returnValue) {
        return returnValue.value;
    }

    return nullptr;
}


