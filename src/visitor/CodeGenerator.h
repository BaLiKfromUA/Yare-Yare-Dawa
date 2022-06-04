//
// Created by balik on 07.05.22.
//
#ifdef __linux__
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
        struct EnvRecord {

            EnvRecord() = default;

            llvm::Value *allocation;
            llvm::Type *type;
        };

        size_t scopeId = 0;
        std::shared_ptr<Environment<EnvRecord>> environment{new Environment<EnvRecord>};

        std::unique_ptr<llvm::LLVMContext> context;
        std::unique_ptr<llvm::Module> module;
        std::unique_ptr<llvm::IRBuilder<>> builder;
        std::unique_ptr<llvm::Function *> mainFunction;

    public:
        CodeGenerator() {
            // Open a new context and module.
            context = std::make_unique<llvm::LLVMContext>();
            module = std::make_unique<llvm::Module>("Yare Yare Dawa -- IR Generator", *context);

            // Create a new builder for the module.
            builder = std::make_unique<llvm::IRBuilder<>>(*context);
        }

        void visitAST(const std::vector<std::shared_ptr<ast::Stmt>> &statements) override {
            startMainFunction("__yyd_start");
            enableStandardLibrary();
            try {
                for (const std::shared_ptr<ast::Stmt> &statement: statements) {
                    execute(statement);
                }
            } catch (RuntimeError &error) {
                Errors::errorRuntime(error);
            }
            endMainFunction();

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

        std::any visitCallExpr(const std::shared_ptr<ast::Call> &expr) override;

        std::any visitFunctionStmt(std::shared_ptr<ast::Function> stmt) override;

        std::any visitReturnStmt(std::shared_ptr<ast::Return> stmt) override;

    protected:
        std::any
        validateType(scanning::TokenType requiredToken, const std::any &candidateValue, bool checkVoid) override;

    private:
        /* debug helpers */
        void dumpIR() {
            module->print(llvm::outs(), nullptr);
        }

        std::string dumpIRToString() {
            std::string outstr;
            llvm::raw_string_ostream oss(outstr);

            module->print(oss, nullptr);

            return oss.str();
        }

        /* type helpers */
        llvm::Type *getDoubleTy() { return llvm::Type::getDoubleTy(*context); }

        llvm::Type *getBoolTy() { return builder->getTrue()->getType(); }

        llvm::Type *getVoidTy() { return llvm::Type::getVoidTy(*context); }

        llvm::Type *getCharTy() {
            //todo: define encoding in documentation
            return llvm::IntegerType::get(*context, 8);
        }

        llvm::Type *getStringTy() {
            return getCharTy()->getPointerTo();
        }

        void checkNumberOperands(const scanning::Token &op, llvm::Value *left, llvm::Value *right) {
            if (left->getType() == getDoubleTy() && right->getType() == getDoubleTy()) {
                return;
            }

            throw RuntimeError{op, "Operands must be numbers."};
        }

        void checkNumberOperand(const scanning::Token &op,
                                llvm::Value *operand) {
            if (operand->getType() == getDoubleTy()) return;
            throw RuntimeError{op, "Operand must be a number."};
        }

        // taken from https://stackoverflow.com/a/51811344
        //  todo: make this method more efficient
        llvm::Value *convertStringToIR(const std::string &value) {
            auto charType = getCharTy();

            //1. Initialize chars vector
            std::vector<llvm::Constant *> chars(value.length());
            for (unsigned int i = 0; i < value.size(); i++) {
                chars[i] = llvm::ConstantInt::get(charType, value[i]);
            }

            //1b. add a zero terminator too
            chars.push_back(llvm::ConstantInt::get(charType, 0));


            //2. Initialize the string from the characters
            auto stringType = llvm::ArrayType::get(charType, chars.size());

            //3. Create the declaration statement
            auto hash = std::hash<std::string>{}(value);
            auto globalDeclaration = (llvm::GlobalVariable *) module->getOrInsertGlobal(std::to_string(hash) + ".str",
                                                                                        stringType);
            globalDeclaration->setInitializer(llvm::ConstantArray::get(stringType, chars));
            globalDeclaration->setConstant(true);
            globalDeclaration->setLinkage(llvm::GlobalValue::LinkageTypes::PrivateLinkage);
            globalDeclaration->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);


            //4. Return a cast to an i8*
            return static_cast<llvm::Value *>(llvm::ConstantExpr::getBitCast(globalDeclaration,
                                                                             charType->getPointerTo()));
        }

        llvm::Value *convertToBoolean(llvm::Value *val) {
            return val->getType() == getStringTy() ? static_cast<llvm::Value *>(builder->getTrue())
                                                   : builder->CreateFPToUI(val, getBoolTy());
        }

        /* block helpers */
        void startMainFunction(const std::string &name) {
            // single __yyd_start function for void module
            llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getVoidTy(*context), false);

            mainFunction = std::make_unique<llvm::Function *>(
                    llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name,
                                           *module));

            // basic block to start instruction insertion
            llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "entry", *mainFunction);
            builder->SetInsertPoint(BB);
        }

        void endMainFunction() { builder->CreateRetVoid(); }

        void createFnDecl(llvm::FunctionType *FT, const std::string &name) {
            llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, *module);
        }

        void handleReturnValue(llvm::Type *returnType, llvm::Value *returnValue, const scanning::Token &expectedToken) {
            if (returnValue == nullptr) {
                if (returnType == getVoidTy()) {
                    builder->CreateRetVoid();
                } else {
                    throw RuntimeError(expectedToken, "expected return type doesn't match with given");
                }
            } else {
                if (returnValue->getType() == returnType) {
                    builder->CreateRet(returnValue);
                } else {
                    throw RuntimeError(expectedToken, "expected return type doesn't match with given");
                }
            }
        }

        void executeBlock(const std::vector<std::shared_ptr<ast::Stmt>> &statements,
                          const std::shared_ptr<Environment<EnvRecord>> &env);


    protected:
        void enableStandardLibrary() override {
            // print
            llvm::FunctionType *printForDouble = llvm::FunctionType::get(getVoidTy(), {getDoubleTy()}, false);
            createFnDecl(printForDouble, "__yyd_print_double");

            llvm::FunctionType *printForBool = llvm::FunctionType::get(getVoidTy(), {getBoolTy()}, false);
            createFnDecl(printForBool, "__yyd_print_bool");

            llvm::FunctionType *printForStr = llvm::FunctionType::get(getVoidTy(), {getStringTy()}, false);
            createFnDecl(printForStr, "__yyd_print_string");

            // string
            llvm::FunctionType *stringConcat = llvm::FunctionType::get(getStringTy(), {getStringTy(), getStringTy()},
                                                                       false);
            createFnDecl(stringConcat, "__yyd_string_concat");

            llvm::FunctionType *stringMultiply = llvm::FunctionType::get(getStringTy(), {getStringTy(), getDoubleTy()},
                                                                         false);
            createFnDecl(stringMultiply, "__yyd_string_multiply");

            llvm::FunctionType *len = llvm::FunctionType::get(getDoubleTy(), {getStringTy()},
                                                              false);
            createFnDecl(len, "len");

            // stl
            llvm::FunctionType *clock = llvm::FunctionType::get(getDoubleTy(), {},
                                                                false);
            createFnDecl(clock, "now");

        }
    };
}


#endif //YARE_YARE_DAWA_CODEGENERATOR_H
#endif