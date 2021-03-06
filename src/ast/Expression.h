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

    class Call;

    class Input;

    class ExprVisitor {
    public:
        virtual std::any visitAssignExpr(const std::shared_ptr<ast::Assign> &expr) = 0;

        virtual std::any visitBinaryExpr(const std::shared_ptr<ast::Binary> &expr) = 0;

        virtual std::any visitGroupingExpr(const std::shared_ptr<ast::Grouping> &expr) = 0;

        virtual std::any visitLiteralExpr(const std::shared_ptr<ast::Literal> &expr) = 0;

        virtual std::any visitUnaryExpr(const std::shared_ptr<ast::Unary> &expr) = 0;

        virtual std::any visitVariableExpr(const std::shared_ptr<ast::Variable> &expr) = 0;

        virtual std::any visitLogicalExpr(const std::shared_ptr<ast::Logical> &expr) = 0;

        virtual std::any visitCallExpr(const std::shared_ptr<ast::Call> &expr) = 0;

        virtual std::any visitInputExpr(const std::shared_ptr<ast::Input> &expr) = 0;

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

    class Call final : public Expr, public std::enable_shared_from_this<Call> {
    public:
        Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
                : callee{std::move(callee)}, paren{std::move(paren)}, arguments{std::move(arguments)} {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visitCallExpr(shared_from_this());
        }

        const std::shared_ptr<Expr> callee;
        const Token paren;
        const std::vector<std::shared_ptr<Expr>> arguments;
    };

    class Input final : public Expr, public std::enable_shared_from_this<Input> {
    public:

        explicit Input(const scanning::TokenType inputType) : inputType(inputType) {}

        std::any accept(ExprVisitor &visitor) override {
            return visitor.visitInputExpr(shared_from_this());
        }

        const scanning::TokenType inputType;
    };
}

#endif //YARE_YARE_DAWA_EXPRESSION_H
