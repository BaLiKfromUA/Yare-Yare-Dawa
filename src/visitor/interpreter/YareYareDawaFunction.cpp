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
        // todo: refactor
        bool isValidType;
        switch (type.type) {
            case scanning::STR:
                isValidType = arguments[i].type() == typeid(std::string);
                break;
            case scanning::NUM:
                isValidType = arguments[i].type() == typeid(double);
                break;
            case scanning::BOOL:
                isValidType = arguments[i].type() == typeid(bool);
                break;
            default:
                isValidType = false;
        }


        if (isValidType) {
            environment->define(value.lexeme,
                                arguments[i]);
        } else {
            // todo: refactor
            throw std::runtime_error("invalid input type");
        }
    }

    try {
        interpreter.executeBlock(declaration->body, environment);
    } catch (YareYareDawaReturn &returnValue) {

        auto result = returnValue.value;

        // todo: refactor
        bool isValidType;
        switch (declaration->returnType.type) {
            case scanning::STR:
                isValidType = result.type() == typeid(std::string);
                break;
            case scanning::NUM:
                isValidType = result.type() == typeid(double);
                break;
            case scanning::BOOL:
                isValidType = result.type() == typeid(bool);
                break;
            case scanning::VOID:
                isValidType = result.type() == typeid(nullptr);
                break;
            default:
                isValidType = false;
        }

        if (isValidType) {
            return result;
        } else {
            // todo: refactor
            throw std::runtime_error("invalid return type");
        }
    }

    return nullptr;
}


