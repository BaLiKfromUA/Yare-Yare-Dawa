//
// Created by balik on 4/15/2022.
//

#include <iomanip>
#include "Interpreter.h"
#include "interpreter/YareYareDawaReturn.h"

namespace visitor {

    std::any Interpreter::visitUnaryExpr(const std::shared_ptr<ast::Unary> &expr) {
        std::any right = evaluate(expr->right);

        switch (expr->op.type) {
            case scanning::BANG:
                return !isTruthy(right);
            case scanning::MINUS:
                checkNumberOperand(expr->op, right);
                return -std::any_cast<double>(right);
            default:
                // Unreachable.
                return {};
        }
    }

    std::any Interpreter::visitBinaryExpr(const std::shared_ptr<ast::Binary> &expr) {
        std::any left = evaluate(expr->left);
        std::any right = evaluate(expr->right);

        switch (expr->op.type) {
            case scanning::BANG_EQUAL:
                return !isEqual(left, right);
            case scanning::EQUAL_EQUAL:
                return isEqual(left, right);
            case scanning::GREATER:
                checkNumberOperands(expr->op, left, right);
                return std::any_cast<double>(left) > std::any_cast<double>(right);
            case scanning::GREATER_EQUAL:
                checkNumberOperands(expr->op, left, right);
                return std::any_cast<double>(left) >= std::any_cast<double>(right);
            case scanning::LESS:
                checkNumberOperands(expr->op, left, right);
                return std::any_cast<double>(left) < std::any_cast<double>(right);
            case scanning::LESS_EQUAL:
                checkNumberOperands(expr->op, left, right);
                return std::any_cast<double>(left) <= std::any_cast<double>(right);
            case scanning::MINUS:
                checkNumberOperands(expr->op, left, right);
                return std::any_cast<double>(left) - std::any_cast<double>(right);
            case scanning::PLUS:
                if (left.type() == typeid(double) && right.type() == typeid(double)) {
                    return std::any_cast<double>(left) + std::any_cast<double>(right);
                }

                if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
                    return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
                }

                throw RuntimeError{expr->op, "Operands must be two numbers or two strings."};
            case scanning::SLASH:
                checkNumberOperands(expr->op, left, right);
                return std::any_cast<double>(left) / std::any_cast<double>(right);
            case scanning::STAR:
                if (left.type() == typeid(double) && right.type() == typeid(double)) {
                    return std::any_cast<double>(left) * std::any_cast<double>(right);
                }

                if ((left.type() == typeid(double) && right.type() == typeid(std::string)) ||
                    (left.type() == typeid(std::string) && right.type() == typeid(double))) {
                    auto str = left.type() == typeid(std::string) ? std::any_cast<std::string>(left)
                                                                  : std::any_cast<std::string>(right);
                    auto num =
                            left.type() == typeid(double) ? std::any_cast<double>(left) : std::any_cast<double>(right);

                    std::string result;
                    for (int i = 0; i < num; ++i) {
                        result += str;
                    }

                    return result;
                }

                throw RuntimeError{expr->op, "Operands must be two numbers or one number and string."};
            default:
                // Unreachable.
                throw RuntimeError{expr->op, "Unknown binary operation."};
        }
    }

    bool Interpreter::isEqual(const std::any &a, const std::any &b) {
        if (a.type() == typeid(nullptr) && b.type() == typeid(nullptr)) {
            return true;
        }

        if (a.type() == typeid(nullptr) || b.type() == typeid(nullptr)) {
            return false;
        }

        if (a.type() == typeid(std::string) &&
            b.type() == typeid(std::string)) {
            return std::any_cast<std::string>(a) ==
                   std::any_cast<std::string>(b);
        }
        if (a.type() == typeid(double) && b.type() == typeid(double)) {
            return std::any_cast<double>(a) == std::any_cast<double>(b);
        }
        if (a.type() == typeid(bool) && b.type() == typeid(bool)) {
            return std::any_cast<bool>(a) == std::any_cast<bool>(b);
        }

        return false;
    }

    std::string Interpreter::stringify(const std::any &object) {
        using namespace std::string_literals;
        if (object.type() == typeid(nullptr)) return "nil"s;

        if (object.type() == typeid(double)) {
            std::string text = std::to_string(
                    std::any_cast<double>(object));

            int ind = text.length() - 1;

            while (text[ind] == '0') {
                --ind;
            }

            if (text[ind] == '.') {
                text = text.substr(0, ind);
            } else {
                text = text.substr(0, ind + 1);
            }

            return text;
        }

        if (object.type() == typeid(std::string)) {
            return std::any_cast<std::string>(object);
        }
        if (object.type() == typeid(bool)) {
            return std::any_cast<bool>(object) ? "true"s : "false"s;
        }

        return "Error in stringify: object type not recognized."s;
    }

    std::any Interpreter::visitBlockStmt(const std::shared_ptr<ast::Block> &stmt) {
        executeBlock(stmt->statements, std::make_shared<Environment<std::any>>(environment));
        return {};
    }

    void Interpreter::executeBlock(const std::vector<std::shared_ptr<ast::Stmt>> &statements,
                                   const std::shared_ptr<Environment<std::any>> &env) {

        std::shared_ptr<Environment<std::any>> previous = this->environment;
        this->environment = env;
        try {
            for (const std::shared_ptr<ast::Stmt> &statement: statements) {
                execute(statement);
            }
        } catch (...) { // no finally blocks in C++ >:(
            this->environment = previous;
            throw;
        }

        this->environment = previous;
    }

    std::any Interpreter::visitExpressionStmt(const std::shared_ptr<ast::Expression> &stmt) {
        evaluate(stmt->expression);
        return {};
    }

    std::any Interpreter::visitPrintStmt(const std::shared_ptr<ast::Print> &stmt) {
        std::any value = evaluate(stmt->expression);
        std::cout << stringify(value) << "\n";
        return {};
    }

    std::any Interpreter::visitVarStmt(const std::shared_ptr<ast::Var> &stmt) {
        std::any value = nullptr;
        if (stmt->initializer != nullptr) {
            value = evaluate(stmt->initializer);
        }

        environment->define(stmt->name.lexeme, std::move(value));
        return {};
    }

    std::any Interpreter::visitAssignExpr(const std::shared_ptr<ast::Assign> &expr) {
        std::any value = evaluate(expr->value);
        environment->assign(expr->name, value);
        return value;
    }

    std::any Interpreter::visitVariableExpr(const std::shared_ptr<ast::Variable> &expr) {
        return environment->get(expr->name);
    }

    std::any Interpreter::visitLogicalExpr(const std::shared_ptr<ast::Logical> &expr) {
        std::any left = evaluate(expr->left);

        if (expr->op.type == scanning::OR) {
            if (isTruthy(left)) return true;
        } else {
            if (!isTruthy(left)) return false;
        }

        return isTruthy(evaluate(expr->right));
    }

    std::any Interpreter::visitIfStmt(const std::shared_ptr<ast::If> &stmt) {
        if (isTruthy(evaluate(stmt->condition))) {
            execute(stmt->thenBranch);
        } else if (stmt->elseBranch != nullptr) {
            execute(stmt->elseBranch);
        }
        return {};
    }

    std::any Interpreter::visitWhileStmt(const std::shared_ptr<ast::While> &stmt) {
        while (isTruthy(evaluate(stmt->condition))) {
            execute(stmt->body);
        }
        return {};
    }

    std::any Interpreter::visitCallExpr(const std::shared_ptr<ast::Call> &expr) {
        std::any callee = evaluate(expr->callee);

        std::vector<std::any> arguments;
        for (const std::shared_ptr<ast::Expr> &argument: expr->arguments) {
            arguments.push_back(evaluate(argument));
        }

        // Pointers in a std::any wrapper must be unwrapped before they
        // can be cast.
        std::shared_ptr<YareYareDawaCallable> function;

        if (callee.type() == typeid(std::shared_ptr<YareYareDawaFunction>)) {
            function = std::any_cast<std::shared_ptr<YareYareDawaFunction>>(callee);
        } else {
            function = findFunctionInStandardLibrary(callee, expr->paren);
        }

        if (arguments.size() != function->arity()) {
            throw RuntimeError{expr->paren, "Expected " +
                                            std::to_string(function->arity()) + " arguments but got " +
                                            std::to_string(arguments.size()) + "."};
        }

        return function->call(*this, std::move(arguments));
    }

    std::any Interpreter::visitFunctionStmt(std::shared_ptr<ast::Function> stmt) {
        auto function = std::make_shared<YareYareDawaFunction>(stmt, environment);
        environment->define(stmt->name.lexeme, function);
        return {};
    }

    std::any Interpreter::visitReturnStmt(std::shared_ptr<ast::Return> stmt) {
        std::any value = nullptr;
        if (stmt->value != nullptr) value = evaluate(stmt->value);

        throw YareYareDawaReturn{value};
    }

    std::any
    Interpreter::validateType(scanning::TokenType requiredToken, const std::any &candidateValue, bool checkVoid) {
        bool isValid;

        switch (requiredToken) {
            case scanning::STR_TYPE:
                isValid = candidateValue.type() == typeid(std::string);
                break;
            case scanning::NUM_TYPE:
                isValid = candidateValue.type() == typeid(double);
                break;
            case scanning::BOOL_TYPE:
                isValid = candidateValue.type() == typeid(bool);
                break;
            case scanning::VOID_TYPE:
                if (checkVoid) {
                    isValid = candidateValue.type() == typeid(nullptr);
                    break;
                }

            default:
                isValid = false;
        }

        return isValid;
    }

    // todo: refactor somehow to reduce code duplication
    std::any Interpreter::visitInputExpr(const std::shared_ptr<ast::Input> &expr) {
        switch (expr->inputType) {
            case scanning::STR_TYPE: {
                std::string n;
                std::cin >> n;
                if (!std::cin) {
                    std::cerr << "Problem reading stdin\n";
                    exit(1);
                }
                return n;
            }
            case scanning::NUM_TYPE: {
                int n;
                std::cin >> n;
                if (!std::cin) {
                    std::cerr << "Problem reading stdin\n";
                    exit(1);
                }
                return n;
            }
            case scanning::BOOL_TYPE: {
                bool n;
                std::cin >> n;
                if (!std::cin) {
                    std::cerr << "Problem reading stdin\n";
                    exit(1);
                }
                return n;
            }
            default:
                throw std::runtime_error("unknown input function!");
        }
    }
}