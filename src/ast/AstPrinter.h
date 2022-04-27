//
// Created by balik on 4/15/2022.
//

#ifndef YARE_YARE_DAWA_ASTPRINTER_H
#define YARE_YARE_DAWA_ASTPRINTER_H

#include <any>
#include <cassert>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include "Expression.h"

/***
 * USE IT ONLY FOR DEBUG OF YOUR AST
 */
namespace ast {
    class AstPrinter final : public ExprVisitor {
    public:
        std::string print(const std::shared_ptr<Expr> &expr) {
            return std::any_cast<std::string>(expr->accept(*this));
        }

        std::any visitBinaryExpr(const std::shared_ptr<Binary> &expr) override {
            return parenthesize(expr->op.lexeme,
                                expr->left, expr->right);
        }

        std::any visitGroupingExpr(const std::shared_ptr<Grouping> &expr) override {
            return parenthesize("group", expr->expression);
        }

        std::any visitLiteralExpr(const std::shared_ptr<Literal> &expr) override {
            using namespace std::string_literals;
            auto &value_type = expr->value.type();

            if (value_type == typeid(nullptr)) {
                return "nil"s;
            } else if (value_type == typeid(std::string)) {
                return std::any_cast<std::string>(expr->value);
            } else if (value_type == typeid(double)) {
                return std::to_string(std::any_cast<double>(expr->value));
            } else if (value_type == typeid(bool)) {
                return std::any_cast<bool>(expr->value) ? "true"s : "false"s;
            }

            return "Error in visitLiteralExpr: literal type not recognized."s;
        }

        std::any visitUnaryExpr(const std::shared_ptr<Unary> &expr) override {
            return parenthesize(expr->op.lexeme, expr->right);
        }

    private:
        template<class... E>
        std::string parenthesize(std::string_view name, E... expr) {
            assert((... && std::is_same_v<E, std::shared_ptr<Expr>>));

            std::ostringstream builder;

            builder << "(" << name;
            ((builder << " " << print(expr)), ...);
            builder << ")";

            return builder.str();
        }
    };
}

#endif //YARE_YARE_DAWA_ASTPRINTER_H
