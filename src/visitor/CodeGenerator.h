//
// Created by balik on 07.05.22.
//

#ifndef YARE_YARE_DAWA_CODEGENERATOR_H
#define YARE_YARE_DAWA_CODEGENERATOR_H

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>
#include <vector>
#include "ast/Statement.h"
#include "Environment.h"
#include "AstVisitor.h"

namespace visitor {
    class CodeGenerator final : public AstVisitor {
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> module;
        std::unique_ptr<llvm::IRBuilder<>> builder;

    public:
        CodeGenerator() {
            // Open a new context and module.
            context = std::make_unique<llvm::LLVMContext>();
            module = std::make_unique<llvm::Module>("default code generator", *context);

            // Create a new builder for the module.
            builder = std::make_unique<llvm::IRBuilder<>>(*context);
        }

        void dumpIR();

        std::string dumpIRToString();

        void visitAST(const std::vector<std::shared_ptr<ast::Stmt>> &statements) override;

        std::any visitAssignExpr(const std::shared_ptr<ast::Assign> &expr) override;

        std::any visitBinaryExpr(const std::shared_ptr<ast::Binary> &expr) override;

        std::any visitGroupingExpr(const std::shared_ptr<ast::Grouping> &expr) override;

        std::any visitLiteralExpr(const std::shared_ptr<ast::Literal> &expr) override;

        std::any visitUnaryExpr(const std::shared_ptr<ast::Unary> &expr) override;

        std::any visitVariableExpr(const std::shared_ptr<ast::Variable> &expr) override;

        std::any visitLogicalExpr(const std::shared_ptr<ast::Logical> &expr) override;

        std::any visitBlockStmt(const std::shared_ptr<ast::Block> &stmt) override;

        std::any visitExpressionStmt(const std::shared_ptr<ast::Expression> &stmt) override;

        std::any visitPrintStmt(const std::shared_ptr<ast::Print> &stmt) override;

        std::any visitVarStmt(const std::shared_ptr<ast::Var> &stmt) override;

        std::any visitIfStmt(const std::shared_ptr<ast::If> &stmt) override;

        std::any visitWhileStmt(const std::shared_ptr<ast::While> &stmt) override;
    };
}


#endif //YARE_YARE_DAWA_CODEGENERATOR_H
