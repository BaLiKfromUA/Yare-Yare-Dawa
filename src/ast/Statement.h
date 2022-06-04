//
// Created by balik on 4/27/2022.
//

#ifndef YARE_YARE_DAWA_STATEMENT_H
#define YARE_YARE_DAWA_STATEMENT_H

#include <any>
#include <memory>
#include <utility>  // std::move
#include <vector>

#include "scanning/Token.h"
#include "Expression.h"

namespace ast {
    using scanning::Token;

    class Block;

    class Expression;

    class Function;

    class Return;

    class Print;

    class Var;

    class If;

    class While;

    class StmtVisitor {
    public:
        virtual std::any visitBlockStmt(const std::shared_ptr<ast::Block> &stmt) = 0;

        virtual std::any visitExpressionStmt(const std::shared_ptr<ast::Expression> &stmt) = 0;

        virtual std::any visitPrintStmt(const std::shared_ptr<ast::Print> &stmt) = 0;

        virtual std::any visitVarStmt(const std::shared_ptr<ast::Var> &stmt) = 0;

        virtual std::any visitIfStmt(const std::shared_ptr<ast::If> &stmt) = 0;

        virtual std::any visitWhileStmt(const std::shared_ptr<ast::While> &stmt) = 0;

        virtual std::any visitFunctionStmt(std::shared_ptr<ast::Function> stmt) = 0;

        virtual std::any visitReturnStmt(std::shared_ptr<ast::Return> stmt) = 0;

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

    class If final : public Stmt, public std::enable_shared_from_this<If> {
    public:
        If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
                : condition{std::move(condition)}, thenBranch{std::move(thenBranch)},
                  elseBranch{std::move(elseBranch)} {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visitIfStmt(shared_from_this());
        }

        const std::shared_ptr<Expr> condition;
        const std::shared_ptr<Stmt> thenBranch;
        const std::shared_ptr<Stmt> elseBranch;
    };

    class While final : public Stmt, public std::enable_shared_from_this<While> {
    public:
        While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
                : condition{std::move(condition)}, body{std::move(body)} {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visitWhileStmt(shared_from_this());
        }

        const std::shared_ptr<Expr> condition;
        const std::shared_ptr<Stmt> body;
    };

    class Function final : public Stmt, public std::enable_shared_from_this<Function> {
    public:
        Function(Token name, Token returnType, std::vector<std::pair<Token, Token>> params,
                 std::vector<std::shared_ptr<Stmt>> body)
                : name{std::move(name)}, returnType(std::move(returnType)), params{std::move(params)},
                  body{std::move(body)} {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visitFunctionStmt(shared_from_this());
        }

        const Token name;
        const Token returnType;
        const std::vector<std::pair<Token, Token>> params;
        const std::vector<std::shared_ptr<Stmt>> body;
    };

    class Return final : public Stmt, public std::enable_shared_from_this<Return> {
    public:
        Return(Token keyword, std::shared_ptr<Expr> value)
                : keyword{std::move(keyword)}, value{std::move(value)} {}

        std::any accept(StmtVisitor &visitor) override {
            return visitor.visitReturnStmt(shared_from_this());
        }

        const Token keyword;
        const std::shared_ptr<Expr> value;
    };
}
#endif //YARE_YARE_DAWA_STATEMENT_H
