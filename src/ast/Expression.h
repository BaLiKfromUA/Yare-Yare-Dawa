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

namespace ast {
    using scanning::Token;

    class Assign;

    class Binary;

    class Grouping;

    class Literal;

    class Unary;

    class Variable;

    class Logical;

    class ExprVisitor {
    public:
        virtual std::any visitAssignExpr(const std::shared_ptr<Assign> &expr) = 0;

        virtual std::any visitBinaryExpr(const std::shared_ptr<Binary> &expr) = 0;

        virtual std::any visitGroupingExpr(const std::shared_ptr<Grouping> &expr) = 0;

        virtual std::any visitLiteralExpr(const std::shared_ptr<Literal> &expr) = 0;

        virtual std::any visitUnaryExpr(const std::shared_ptr<Unary> &expr) = 0;

        virtual std::any visitVariableExpr(const std::shared_ptr<Variable> &expr) = 0;

        virtual std::any visitLogicalExpr(const std::shared_ptr<Logical> &expr) = 0;

        virtual ~ExprVisitor() = default;
    };

    class Expr {
    public:
        virtual std::any accept(ExprVisitor &visitor) = 0;
    };

    class Assign : public Expr, public std::enable_shared_from_this<Assign> {
    public:
        Assign(Token name, std::shared_ptr<Expr> value)
                : name{std::move(name)}, value{std::move(value)} {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visitAssignExpr(shared_from_this());
        }

        const Token name;
        const std::shared_ptr<Expr> value;
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

    class Logical final : public Expr, public std::enable_shared_from_this<Logical> {
    public:
        Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
                : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visitLogicalExpr(shared_from_this());
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

    class Variable final : public Expr, public std::enable_shared_from_this<Variable> {
    public:
        explicit Variable(Token name)
                : name{std::move(name)} {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visitVariableExpr(shared_from_this());
        }

        const Token name;
    };
}

#endif //YARE_YARE_DAWA_EXPRESSION_H
