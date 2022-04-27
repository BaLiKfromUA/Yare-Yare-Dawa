//
// Created by balik on 4/15/2022.
//

#ifndef YARE_YARE_DAWA_INTERPRETER_H
#define YARE_YARE_DAWA_INTERPRETER_H

#include <iostream>
#include <utility>
#include "Errors.h"
#include "scanning/Token.h"
#include "scanning/TokenType.h"
#include "ast/Expression.h"
#include "ast/Statement.h"


namespace interpreter {
    class Interpreter final : public ast::ExprVisitor, public ast::StmtVisitor {
    public:
        void interpret(const std::vector<std::shared_ptr<ast::Stmt>> &statements) {
            try {
                for (const std::shared_ptr<ast::Stmt> &statement: statements) {
                    execute(statement);
                }
            } catch (RuntimeError &error) {
                Errors::errorRuntime(error);
            }
        }

    private:
        std::any evaluate(const std::shared_ptr<ast::Expr> &expr) {
            return expr->accept(*this);
        }

        void execute(const std::shared_ptr<ast::Stmt> &stmt) {
            stmt->accept(*this);
        }

    public:
        std::any visitGroupingExpr(const std::shared_ptr<ast::Grouping> &expr) override {
            return evaluate(expr->expression);
        }

        std::any visitLiteralExpr(const std::shared_ptr<ast::Literal> &expr) override {
            return expr->value;
        }

        std::any visitUnaryExpr(const std::shared_ptr<ast::Unary> &expr) override;

        std::any visitBinaryExpr(const std::shared_ptr<ast::Binary> &expr) override;

        std::any visitBlockStmt(const std::shared_ptr<ast::Block> &stmt) override;

        std::any visitExpressionStmt(const std::shared_ptr<ast::Expression> &stmt) override;

        std::any visitPrintStmt(const std::shared_ptr<ast::Print> &stmt) override;

        std::any visitVarStmt(const std::shared_ptr<ast::Var> &stmt) override;

    private:

        static void checkNumberOperand(const scanning::Token &op,
                                       const std::any &operand) {
            if (operand.type() == typeid(double)) return;
            throw RuntimeError{op, "Operand must be a number."};
        }

        static void checkNumberOperands(const scanning::Token &op, const std::any &left, const std::any &right) {
            if (left.type() == typeid(double) && right.type() == typeid(double)) {
                return;
            }

            throw RuntimeError{op, "Operands must be numbers."};
        }

        // we follow Ruby’s simple rule: false and nil are false, and everything else is true.
        static bool isTruthy(const std::any &object) {
            if (object.type() == typeid(nullptr)) return false;
            if (object.type() == typeid(bool)) {
                return std::any_cast<bool>(object);
            }
            return true;
        }

        static bool isEqual(const std::any &a, const std::any &b);

        static std::string stringify(const std::any &object);
    };
}


#endif //YARE_YARE_DAWA_INTERPRETER_H
