//
// Created by balik on 07.05.22.
//

#include "CodeGenerator.h"

namespace visitor {
    static llvm::LLVMContext llvm_context;

    void CodeGenerator::visitAST(const std::vector<std::shared_ptr<ast::Stmt>> &statements) {
        // todo:
    }

    std::any CodeGenerator::visitAssignExpr(const std::shared_ptr<ast::Assign> &expr) {
        return std::any();
    }

    std::any CodeGenerator::visitBinaryExpr(const std::shared_ptr<ast::Binary> &expr) {
        return std::any();
    }

    std::any CodeGenerator::visitGroupingExpr(const std::shared_ptr<ast::Grouping> &expr) {
        return std::any();
    }

    std::any CodeGenerator::visitLiteralExpr(const std::shared_ptr<ast::Literal> &expr) {
        return std::any();
    }

    std::any CodeGenerator::visitUnaryExpr(const std::shared_ptr<ast::Unary> &expr) {
        return std::any();
    }

    std::any CodeGenerator::visitVariableExpr(const std::shared_ptr<ast::Variable> &expr) {
        return std::any();
    }

    std::any CodeGenerator::visitLogicalExpr(const std::shared_ptr<ast::Logical> &expr) {
        return std::any();
    }

    std::any CodeGenerator::visitBlockStmt(const std::shared_ptr<ast::Block> &stmt) {
        return std::any();
    }

    std::any CodeGenerator::visitExpressionStmt(const std::shared_ptr<ast::Expression> &stmt) {
        return std::any();
    }

    std::any CodeGenerator::visitPrintStmt(const std::shared_ptr<ast::Print> &stmt) {
        return std::any();
    }

    std::any CodeGenerator::visitVarStmt(const std::shared_ptr<ast::Var> &stmt) {
        return std::any();
    }

    std::any CodeGenerator::visitIfStmt(const std::shared_ptr<ast::If> &stmt) {
        return std::any();
    }

    std::any CodeGenerator::visitWhileStmt(const std::shared_ptr<ast::While> &stmt) {
        return std::any();
    }

    void CodeGenerator::dumpIR() {

    }

    std::string CodeGenerator::dumpIRToString() {
        return std::string();
    }
}
