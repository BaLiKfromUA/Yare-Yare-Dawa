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
#include "Errors.h"
#include "Environment.h"
#include "AstVisitor.h"

namespace visitor {
    class CodeGenerator final : public AstVisitor {
    private:
        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> module;
        std::unique_ptr<llvm::IRBuilder<>> builder;
        std::shared_ptr<Environment<llvm::Value *>> environment{new Environment<llvm::Value *>};
        llvm::Function *Function_ = nullptr;

        void dumpIR() {
            module->print(llvm::outs(), nullptr);
        }

        std::string dumpVariablesToString() {
            std::string outstr;
            llvm::raw_string_ostream oss(outstr);
            auto vars = environment->get_values();
            for (auto var: vars) {
                var->print(oss);
                oss << "\n";
            }

            return oss.str();
        }

        std::string dumpIRToString() {
            std::string outstr;
            llvm::raw_string_ostream oss(outstr);

            module->print(oss, nullptr);

            return oss.str();
        }

        void startFunction(const std::string &name) {
            // single __pcl_start function for void module
            llvm::FunctionType *FT = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*context), /* va args? */ false);

            Function_ = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name,
                                               *module);

            // basic block to start instruction insertion
            llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "entry", Function_);
            builder->SetInsertPoint(BB);
        }

        void endCurrentFunction() { builder->CreateRetVoid(); }

        llvm::Type *getInt32Ty() { return llvm::Type::getInt32Ty(*context); }

        llvm::Type *getDoubleTy() { return llvm::Type::getDoubleTy(*context); }

        llvm::Type *getVoidTy() { return llvm::Type::getVoidTy(*context); }

        void createFnDecl(llvm::FunctionType *FT, const std::string &name) {
            llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, *module);
        }

        static llvm::Value *recastExpression(std::any expression) {
            if (expression.type() == typeid(llvm::Value *)) {
                return std::any_cast<llvm::Value *>(expression);
            } else if (expression.type() == typeid(llvm::ConstantFP *)) {
                auto tmp = std::any_cast<llvm::ConstantFP *>(expression);
                return static_cast<llvm::Value *>(tmp);
            }

            //todo: throw exception
            return nullptr;
        }

        void checkNumberOperands(const scanning::Token &op, llvm::Value *left, llvm::Value *right) {
            if (left->getType() == getDoubleTy() && right->getType() == getDoubleTy()) {
                return;
            }

            throw RuntimeError{op, "Operands must be numbers."};
        }

    public:
        CodeGenerator() {
            // Open a new context and module.
            context = std::make_unique<llvm::LLVMContext>();
            module = std::make_unique<llvm::Module>("default code generator", *context);

            // Create a new builder for the module.
            builder = std::make_unique<llvm::IRBuilder<>>(*context);
        }

        void visitAST(const std::vector<std::shared_ptr<ast::Stmt>> &statements) override {
            startFunction("__yyd_start");
            // add standard lib
            // prototype for print and scan functions
            llvm::Type *Tys[] = {getDoubleTy()};
            llvm::FunctionType *FTPrint = llvm::FunctionType::get(getVoidTy(), Tys, /* va args? */ false);
            // creating decls for modules
            createFnDecl(FTPrint, "__yyd_print");

            try {
                for (const std::shared_ptr<ast::Stmt> &statement: statements) {
                    execute(statement);
                }
            } catch (RuntimeError &error) {
                Errors::errorRuntime(error);
            }
            endCurrentFunction();

            dumpIR();
        }

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
