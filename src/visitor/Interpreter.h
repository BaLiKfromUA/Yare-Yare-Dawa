//
// Created by balik on 4/15/2022.
//

#ifndef YARE_YARE_DAWA_INTERPRETER_H
#define YARE_YARE_DAWA_INTERPRETER_H

#include <iostream>
#include <memory>
#include <utility>
#include "Errors.h"
#include "scanning/Token.h"
#include "scanning/TokenType.h"
#include "ast/Expression.h"
#include "ast/Statement.h"
#include "Environment.h"
#include "AstVisitor.h"
#include "interpreter/YareYareDawaFunction.h"
#include "interpreter/StandardLibrary.h"

namespace visitor {
    class Interpreter final : public AstVisitor {
        friend class YareYareDawaFunction;

        std::shared_ptr<Environment<std::any>> globals{new Environment};
        std::shared_ptr<Environment<std::any>> environment = globals;

    public:
        Interpreter() {
            enableStandardLibrary();
        }

        void visitAST(const std::vector<std::shared_ptr<ast::Stmt>> &statements) override {
            try {
                for (const std::shared_ptr<ast::Stmt> &statement: statements) {
                    execute(statement);
                }
            } catch (RuntimeError &error) {
                Errors::errorRuntime(error);
            }
        }

    protected:
        void enableStandardLibrary() override {
            globals->define("now", std::make_shared<NativeClock>());
            globals->define("len", std::make_shared<StrLen>());
            globals->define("text", std::make_shared<ToStringHelper>());
        }

        std::any
        validateType(scanning::TokenType requiredToken, const std::any &candidateValue, bool checkVoid) override;

    public:
        std::any visitGroupingExpr(const std::shared_ptr<ast::Grouping> &expr) override {
            return evaluate(expr->expression);
        }

        std::any visitLiteralExpr(const std::shared_ptr<ast::Literal> &expr) override {
            return expr->value;
        }

        std::any visitUnaryExpr(const std::shared_ptr<ast::Unary> &expr) override;

        std::any visitBinaryExpr(const std::shared_ptr<ast::Binary> &expr) override;

        std::any visitAssignExpr(const std::shared_ptr<ast::Assign> &expr) override;

        std::any visitVariableExpr(const std::shared_ptr<ast::Variable> &expr) override;

        std::any visitCallExpr(const std::shared_ptr<ast::Call> &expr) override;

        std::any visitInputExpr(const std::shared_ptr<ast::Input> &expr) override;

        std::any visitBlockStmt(const std::shared_ptr<ast::Block> &stmt) override;

        std::any visitExpressionStmt(const std::shared_ptr<ast::Expression> &stmt) override;

        std::any visitPrintStmt(const std::shared_ptr<ast::Print> &stmt) override;

        std::any visitVarStmt(const std::shared_ptr<ast::Var> &stmt) override;

        std::any visitLogicalExpr(const std::shared_ptr<ast::Logical> &expr) override;

        std::any visitIfStmt(const std::shared_ptr<ast::If> &stmt) override;

        std::any visitWhileStmt(const std::shared_ptr<ast::While> &stmt) override;

        std::any visitFunctionStmt(std::shared_ptr<ast::Function> stmt) override;

        std::any visitReturnStmt(std::shared_ptr<ast::Return> stmt) override;

        static std::string stringify(const std::any &object);

    private:
        void executeBlock(const std::vector<std::shared_ptr<ast::Stmt>> &statements,
                          const std::shared_ptr<Environment<>> &env);

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

        static bool isTruthy(const std::any &object) {
            if (object.type() == typeid(nullptr)) return false;

            if (object.type() == typeid(bool)) {
                return std::any_cast<bool>(object);
            }

            if (object.type() == typeid(double)) {
                return std::any_cast<double>(object) != 0.0;
            }

            return true;
        }

        static bool isEqual(const std::any &a, const std::any &b);

        static std::shared_ptr<YareYareDawaCallable>
        findFunctionInStandardLibrary(std::any callee, const scanning::Token &exprParen) {
            if (callee.type() == typeid(std::shared_ptr<NativeClock>)) {
                return std::any_cast<std::shared_ptr<NativeClock>>(callee);
            } else if (callee.type() == typeid(std::shared_ptr<StrLen>)) {
                return std::any_cast<std::shared_ptr<StrLen>>(callee);
            } else if (callee.type() == typeid(std::shared_ptr<ToStringHelper>)) {
                return std::any_cast<std::shared_ptr<ToStringHelper>>(callee);
            } else {
                throw RuntimeError{exprParen,
                                   "Can only call functions and classes."};
            }
        }
    };
}


#endif //YARE_YARE_DAWA_INTERPRETER_H
