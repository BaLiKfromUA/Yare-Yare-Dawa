//
// Created by balik on 07.05.22.
//
#ifdef __linux__
#include "CodeGenerator.h"

namespace visitor {
    std::any CodeGenerator::visitAssignExpr(const std::shared_ptr <ast::Assign> &expr) {
        // todo: maybe use `return Builder_->CreateStore(Rhs, V);`
        auto value = std::any_cast<llvm::Value *>(evaluate(expr->value));
        environment->assign(expr->name, value);
        return value;
    }

    std::any CodeGenerator::visitBinaryExpr(const std::shared_ptr <ast::Binary> &expr) {
        auto left = std::any_cast<llvm::Value *>(evaluate(expr->left));
        auto right = std::any_cast<llvm::Value *>(evaluate(expr->right));
        // https://llvm.org/doxygen/classllvm_1_1CmpInst.html#a2be3583dac92a031fa1458d4d992c78b
        switch (expr->op.type) {
            case scanning::BANG_EQUAL:
                if (left->getType() == getDoubleTy() && right->getType() == getDoubleTy()) {
                    return builder->CreateFCmpONE(left, right);
                }

                if ((left->getType() == getBoolTy() && right->getType() == getBoolTy())
                    || (left->getType() == getStringTy() && right->getType() == getStringTy())) {
                    return builder->CreateICmpNE(left, right);
                }

                return static_cast<llvm::Value *>(builder->getTrue());
            case scanning::EQUAL_EQUAL:
                if (left->getType() == getDoubleTy() && right->getType() == getDoubleTy()) {
                    return builder->CreateFCmpOEQ(left, right);
                }

                if ((left->getType() == getBoolTy() && right->getType() == getBoolTy())
                    || (left->getType() == getStringTy() && right->getType() == getStringTy())) {
                    return builder->CreateICmpEQ(left, right);
                }

                return static_cast<llvm::Value *>(builder->getFalse());
            case scanning::GREATER:
                checkNumberOperands(expr->op, left, right);
                // True if ordered and greater than
                return builder->CreateFCmpOGT(left, right);
            case scanning::GREATER_EQUAL:
                checkNumberOperands(expr->op, left, right);
                // True if ordered and greater than or equal
                return builder->CreateFCmpOGE(left, right);
            case scanning::LESS:
                checkNumberOperands(expr->op, left, right);
                // True if ordered and less than
                return builder->CreateFCmpOLT(left, right);
            case scanning::LESS_EQUAL:
                checkNumberOperands(expr->op, left, right);
                // True if ordered and less than or equal
                return builder->CreateFCmpOLE(left, right);
            case scanning::MINUS:
                checkNumberOperands(expr->op, left, right);
                return builder->CreateFSub(left, right);
            case scanning::PLUS:
                if (left->getType() == getDoubleTy() && right->getType() == getDoubleTy()) {
                    return builder->CreateFAdd(left, right);
                }

                /* todo:
                if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
                    return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
                }*/

                throw RuntimeError{expr->op, "Operands must be two numbers or two strings."};
            case scanning::SLASH:
                checkNumberOperands(expr->op, left, right);
                return builder->CreateFDiv(left, right);
            case scanning::STAR:
                checkNumberOperands(expr->op, left, right);
                return builder->CreateFMul(left, right);
            default:
                // Unreachable.
                throw RuntimeError{expr->op, "Unknown binary operation."};
        }
    }

    std::any CodeGenerator::visitGroupingExpr(const std::shared_ptr <ast::Grouping> &expr) {
        return evaluate(expr->expression);
    }

    /* NOTE: you must return llvm::Value* */
    std::any CodeGenerator::visitLiteralExpr(const std::shared_ptr <ast::Literal> &expr) {
        auto value = expr->value;
        if (value.type() == typeid(double)) {
            auto tmp = llvm::ConstantFP::get(*context, llvm::APFloat(std::any_cast<double>(value)));
            return static_cast<llvm::Value *>(tmp);
        } else if (value.type() == typeid(bool)) {
            auto tmp = std::any_cast<bool>(value) ? builder->getTrue() : builder->getFalse();
            return static_cast<llvm::Value *>(tmp);
        } else if (value.type() == typeid(std::string)) {
            auto casted_value = std::any_cast<std::string>(value);
            return convertStringToIR(casted_value);
        } else {
            // todo: throw error in case of invalid type
            return nullptr;
        }
    }

    std::any CodeGenerator::visitUnaryExpr(const std::shared_ptr <ast::Unary> &expr) {
        auto right = std::any_cast<llvm::Value *>(evaluate(expr->right));

        switch (expr->op.type) {
            case scanning::BANG:
                return builder->CreateXor(convertToBoolean(right), builder->getTrue());
            case scanning::MINUS:
                checkNumberOperand(expr->op, right);
                return builder->CreateFNeg(right);
            default:
                // Unreachable.
                throw RuntimeError{expr->op, "Unknown unary operation."};
        }
    }

    std::any CodeGenerator::visitVariableExpr(const std::shared_ptr <ast::Variable> &expr) {
        return environment->get(expr->name);
    }

    std::any CodeGenerator::visitLogicalExpr(const std::shared_ptr <ast::Logical> &expr) {
        auto left = std::any_cast<llvm::Value *>(evaluate(expr->left));
        auto right = std::any_cast<llvm::Value *>(evaluate(expr->right));

        auto leftAsBool = convertToBoolean(left);
        auto rightAsBool = convertToBoolean(right);


        if (expr->op.type == scanning::OR) {
            return builder->CreateOr(leftAsBool, rightAsBool);
        } else if (expr->op.type == scanning::AND) {
            return builder->CreateAnd(leftAsBool, rightAsBool);
        }

        // Unreachable.
        throw RuntimeError{expr->op, "Unknown logical operation."};
    }

    std::any CodeGenerator::visitBlockStmt(const std::shared_ptr <ast::Block> &stmt) {
        executeBlock(stmt->statements, std::make_shared < Environment < llvm::Value * >> (environment));
        return {};
    }

    void CodeGenerator::executeBlock(const std::vector <std::shared_ptr<ast::Stmt>> &statements,
                                     const std::shared_ptr <Environment<llvm::Value *>> &env) {
        ++scope_id;

        auto previous = this->environment;
        this->environment = env;

        auto blockParent = builder->GetInsertBlock()->getParent();
        auto block = llvm::BasicBlock::Create(*context, "begin-" + std::to_string(scope_id), blockParent);
        auto afterBlock = llvm::BasicBlock::Create(*context, "end-" + std::to_string(scope_id));

        builder->CreateBr(block);
        builder->SetInsertPoint(block);
        try {
            for (const std::shared_ptr <ast::Stmt> &statement: statements) {
                execute(statement);
            }
        } catch (...) {
            this->environment = previous;
            blockParent->getBasicBlockList().push_back(afterBlock);
            builder->CreateBr(afterBlock);
            builder->SetInsertPoint(afterBlock);
            throw;
        }

        this->environment = previous;
        blockParent->getBasicBlockList().push_back(afterBlock);
        builder->CreateBr(afterBlock);
        builder->SetInsertPoint(afterBlock);
    }

    std::any CodeGenerator::visitExpressionStmt(const std::shared_ptr <ast::Expression> &stmt) {
        return evaluate(stmt->expression);
    }

    std::any CodeGenerator::visitPrintStmt(const std::shared_ptr <ast::Print> &stmt) {
        auto value = std::any_cast<llvm::Value *>(evaluate(stmt->expression));
        std::string function_name = "__yyd_print_double";

        if (value->getType() == getDoubleTy()) {
            function_name = "__yyd_print_double";
        } else if (value->getType() == getBoolTy()) {
            function_name = "__yyd_print_bool";
        } else if (value->getType() == getStringTy()) {
            function_name = "__yyd_print_string";
        } else {
            // todo: throw error
        }

        llvm::Function *calleeF = module->getFunction(function_name);

        return builder->CreateCall(calleeF, {value});
    }

    std::any CodeGenerator::visitVarStmt(const std::shared_ptr <ast::Var> &stmt) {
        llvm::Value *value = nullptr;
        if (stmt->initializer != nullptr) {
            value = std::any_cast<llvm::Value *>(evaluate(stmt->initializer));
        }

        environment->define(stmt->name.lexeme, value);
        return {};
    }

    std::any CodeGenerator::visitIfStmt(const std::shared_ptr <ast::If> &stmt) {
        auto condition = std::any_cast<llvm::Value *>(evaluate(stmt->condition));

        if (!condition) {
            return nullptr; // todo: fix;
        }

        condition = convertToBoolean(condition);

        auto parent = builder->GetInsertBlock()->getParent();
        // Create blocks for the then and else cases.  Insert the 'then' block at the
        // end of the function.
        auto thenBranch =
                llvm::BasicBlock::Create(*context, "then ", parent);
        auto elseBranch = llvm::BasicBlock::Create(*context, "else ");
        auto mergeBranch = llvm::BasicBlock::Create(*context, "ifcont ");

        builder->CreateCondBr(condition, thenBranch, elseBranch);

        // Emit then value.
        builder->SetInsertPoint(thenBranch);
        if (stmt->thenBranch == nullptr) {
            // todo: throw RuntimeError{stmt, "Expected then branch for if statement!"};
        }

        execute(stmt->thenBranch);
        builder->CreateBr(mergeBranch);

        // Emit else block.
        parent->getBasicBlockList().push_back(elseBranch);
        builder->SetInsertPoint(elseBranch);

        if (stmt->elseBranch != nullptr) {
            execute(stmt->elseBranch);
        }

        builder->CreateBr(mergeBranch);

        // Emit merge block.
        parent->getBasicBlockList().push_back(mergeBranch);
        builder->SetInsertPoint(mergeBranch);

        return {};
    }

    // WARNING!!! IT'S EXTREMELY UGLY AND NOT EFFICIENT SOLUTION!!! DON'T LOOK AT IT!!!
    // todo: refactor !!!
    std::any CodeGenerator::visitWhileStmt(const std::shared_ptr <ast::While> &stmt) {

        auto condition = std::any_cast<llvm::Value *>(evaluate(stmt->condition));
        // todo: handle that body or condition is null
        condition = convertToBoolean(condition);

        while (condition != nullptr && condition == builder->getTrue()) {
            execute(stmt->body);

            condition = std::any_cast<llvm::Value *>(evaluate(stmt->condition));
            condition = convertToBoolean(condition);
        }

        return {};
    }
}
#endif