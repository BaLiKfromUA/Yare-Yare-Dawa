//
// Created by balik on 4/11/2022.
//

#ifndef YARE_YARE_DAWA_EXPRESSION_H
#define YARE_YARE_DAWA_EXPRESSION_H

#include <any>
#include <memory>
#include <utility>  // std::move
#include <vector>
#include "scanning/Token.h"

using scanning::Token;

namespace ast {

    class Binary;

    class Grouping;

    class Literal;

    class Unary;


    class ExprVisitor {
    public:
        virtual std::any visitBinaryExpr(std::shared_ptr<Binary> expr) = 0;

        virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) = 0;

        virtual std::any visitLiteralExpr(std::shared_ptr<Literal> expr) = 0;

        virtual std::any visitUnaryExpr(std::shared_ptr<Unary> expr) = 0;
    };

    class Expr {
    public:
        virtual std::any accept(ExprVisitor &visitor) = 0;
    };

    class Binary final : public Expr, public std::enable_shared_from_this<Binary> {
    public:
        Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
                : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visitBinaryExpr(shared_from_this());
        }

        const std::shared_ptr<Expr> left;
        const Token op;
        const std::shared_ptr<Expr> right;
    };

    class Grouping final : public Expr, public std::enable_shared_from_this<Grouping> {
    public:
        explicit Grouping(std::shared_ptr<Expr> expression)
                : expression{std::move(expression)} {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visitGroupingExpr(shared_from_this());
        }

        const std::shared_ptr<Expr> expression;
    };

    class Unary final : public Expr, public std::enable_shared_from_this<Unary> {
    public:
        Unary(Token op, std::shared_ptr<Expr> right)
                : op{std::move(op)}, right{std::move(right)} {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visitUnaryExpr(shared_from_this());
        }

        const Token op;
        const std::shared_ptr<Expr> right;
    };

    class Literal final : public Expr, public std::enable_shared_from_this<Literal> {
    public:
        explicit Literal(std::any value)
                : value{std::move(value)} {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visitLiteralExpr(shared_from_this());
        }

        const std::any value;
    };
}

#endif //YARE_YARE_DAWA_EXPRESSION_H
