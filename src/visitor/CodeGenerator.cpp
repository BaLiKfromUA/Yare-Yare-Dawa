//
// Created by balik on 07.05.22.
//

#include "CodeGenerator.h"

namespace visitor {
    std::any CodeGenerator::visitAssignExpr(const std::shared_ptr<ast::Assign> &expr) {
        auto value = std::any_cast<llvm::Value *>(evaluate(expr->value));
        environment->assign(expr->name, value);
        return value;
    }

    std::any CodeGenerator::visitBinaryExpr(const std::shared_ptr<ast::Binary> &expr) {
        auto left = recastExpression(evaluate(expr->left));
        auto right = recastExpression(evaluate(expr->right));
        switch (expr->op.type) {
            case scanning::BANG_EQUAL:
                // todo:
            case scanning::EQUAL_EQUAL:
                // todo:
            case scanning::GREATER:
                // todo:
            case scanning::GREATER_EQUAL:
                // todo:
            case scanning::LESS:
                // todo:
            case scanning::LESS_EQUAL:
                // todo:
                return nullptr;
            case scanning::MINUS:
                checkNumberOperands(expr->op, left, right);
                return builder->CreateFSub(left, right, "subtmp");
            case scanning::PLUS:
                if (left->getType() == getDoubleTy() && right->getType() == getDoubleTy()) {
                    return builder->CreateFAdd(left, right, "addtmp");
                }

                /* todo:
                if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
                    return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
                }*/

                throw RuntimeError{expr->op, "Operands must be two numbers or two strings."};
            case scanning::SLASH:
                checkNumberOperands(expr->op, left, right);
                return builder->CreateFDiv(left, right, "divtmp");
            case scanning::STAR:
                checkNumberOperands(expr->op, left, right);
                return builder->CreateFMul(left, right, "multmp");
            default:
                // Unreachable.
                return {};
        }
    }

    std::any CodeGenerator::visitGroupingExpr(const std::shared_ptr<ast::Grouping> &expr) {
        return evaluate(expr->expression);
    }

    std::any CodeGenerator::visitLiteralExpr(const std::shared_ptr<ast::Literal> &expr) {
        auto value = expr->value;
        if (value.type() == typeid(double)) {
            return llvm::ConstantFP::get(*context, llvm::APFloat(std::any_cast<double>(value)));
        } else {
            // todo: handle string and throw error in case of invalid type
            return nullptr;
        }
    }

    std::any CodeGenerator::visitUnaryExpr(const std::shared_ptr<ast::Unary> &expr) {
        return std::any();
    }

    std::any CodeGenerator::visitVariableExpr(const std::shared_ptr<ast::Variable> &expr) {
        return environment->get(expr->name);
    }

    std::any CodeGenerator::visitLogicalExpr(const std::shared_ptr<ast::Logical> &expr) {
        return std::any();
    }

    std::any CodeGenerator::visitBlockStmt(const std::shared_ptr<ast::Block> &stmt) {
        return std::any();
    }

    std::any CodeGenerator::visitExpressionStmt(const std::shared_ptr<ast::Expression> &stmt) {
        evaluate(stmt->expression);
        return {};
    }

    std::any CodeGenerator::visitPrintStmt(const std::shared_ptr<ast::Print> &stmt) {
        llvm::Function *calleeF = module->getFunction("__yyd_print");
        if (!calleeF) {
            // todo: throw error
        }
        std::vector<llvm::Value *> ArgsV;
        ArgsV.push_back(std::any_cast<llvm::Value *>(evaluate(stmt->expression)));

        return builder->CreateCall(calleeF, ArgsV);
    }

    std::any CodeGenerator::visitVarStmt(const std::shared_ptr<ast::Var> &stmt) {
        llvm::Value *value = nullptr;
        if (stmt->initializer != nullptr) {
            value = recastExpression(evaluate(stmt->initializer));
        }

        environment->define(stmt->name.lexeme, value);
        return {};
    }

    std::any CodeGenerator::visitIfStmt(const std::shared_ptr<ast::If> &stmt) {
        return std::any();
    }

    std::any CodeGenerator::visitWhileStmt(const std::shared_ptr<ast::While> &stmt) {
        return std::any();
    }
}
