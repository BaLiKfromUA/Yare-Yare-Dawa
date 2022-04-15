//
// Created by balik on 4/15/2022.
//

#include "Interpreter.h"

namespace interpreter {

    std::any Interpreter::visitUnaryExpr(std::shared_ptr<ast::Unary> expr) {
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

    std::any Interpreter::visitBinaryExpr(std::shared_ptr<ast::Binary> expr) {
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
                checkNumberOperands(expr->op, left, right);
                return std::any_cast<double>(left) * std::any_cast<double>(right);
            default:
                // Unreachable.
                return {};
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
            if (text[text.length() - 2] == '.' &&
                text[text.length() - 1] == '0') {
                text = text.substr(0, text.length() - 2);
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
}