//
// Created by balik on 4/27/2022.
//

#ifndef YARE_YARE_DAWA_STATEMENT_H
#define YARE_YARE_DAWA_STATEMENT_H

#include <any>
#include <memory>
#include <utility>  // std::move
#include <vector>

#include "../scanning/Token.h" // todo: fix
#include "Expression.h"

namespace ast {
    using scanning::Token;

    class Block;

    class Expression;

    class Print;

    class Var;

    class StmtVisitor {
    public:
        virtual std::any visitBlockStmt(const std::shared_ptr<Block> &stmt) = 0;

        virtual std::any visitExpressionStmt(const std::shared_ptr<Expression> &stmt) = 0;

        virtual std::any visitPrintStmt(const std::shared_ptr<Print> &stmt) = 0;

        virtual std::any visitVarStmt(const std::shared_ptr<Var> &stmt) = 0;

        virtual ~StmtVisitor() = default;
    };

    class Stmt {
    public:
        virtual std::any accept(StmtVisitor &visitor) = 0;
    };

    class Block final : public Stmt, public std::enable_shared_from_this<Block> {
    public:
        explicit Block(std::vector<std::shared_ptr<Stmt>> statements)
                : statements{std::move(statements)} {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visitBlockStmt(shared_from_this());
        }

        const std::vector<std::shared_ptr<Stmt>> statements;
    };

    class Expression final : public Stmt, public std::enable_shared_from_this<Expression> {
    public:
        explicit Expression(std::shared_ptr<Expr> expression)
                : expression{std::move(expression)} {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visitExpressionStmt(shared_from_this());
        }

        const std::shared_ptr<Expr> expression;
    };

    class Print final : public Stmt, public std::enable_shared_from_this<Print> {
    public:
        explicit Print(std::shared_ptr<Expr> expression)
                : expression{std::move(expression)} {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visitPrintStmt(shared_from_this());
        }

        const std::shared_ptr<Expr> expression;
    };

    class Var final : public Stmt, public std::enable_shared_from_this<Var> {
    public:
        explicit Var(Token name, std::shared_ptr<Expr> initializer)
                : name{std::move(name)}, initializer{std::move(initializer)} {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visitVarStmt(shared_from_this());
        }

        const Token name;
        const std::shared_ptr<Expr> initializer;
    };
}
#endif //YARE_YARE_DAWA_STATEMENT_H
