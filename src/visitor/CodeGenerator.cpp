//
// Created by balik on 07.05.22.
//
#ifdef __linux__

#include "CodeGenerator.h"
#include "visitor/interpreter/YareYareDawaReturn.h"

namespace visitor {
    std::any CodeGenerator::visitAssignExpr(const std::shared_ptr<ast::Assign> &expr) {
        auto value = std::any_cast<llvm::Value *>(evaluate(expr->value));

        // compare types
        auto oldRecord = environment->get(expr->name);
        if (value == nullptr || oldRecord.type != value->getType()) {
            throw RuntimeError(expr->name, "Trying to assign incorrect type!");
        }

        builder->CreateStore(value, oldRecord.allocation);

        return value;
    }

    std::any CodeGenerator::visitBinaryExpr(const std::shared_ptr<ast::Binary> &expr) {
        auto left = std::any_cast<llvm::Value *>(evaluate(expr->left));
        auto right = std::any_cast<llvm::Value *>(evaluate(expr->right));
        // https://llvm.org/doxygen/classllvm_1_1CmpInst.html#a2be3583dac92a031fa1458d4d992c78b
        switch (expr->op.type) {
            case scanning::BANG_EQUAL:
                if (left->getType() == getDoubleTy() && right->getType() == getDoubleTy()) {
                    return builder->CreateFCmpONE(left, right, "tmp_cmpNE");
                }

                if ((left->getType() == getBoolTy() && right->getType() == getBoolTy())
                    || (left->getType() == getStringTy() && right->getType() == getStringTy())) {
                    return builder->CreateICmpNE(left, right, "tmp_cmpNE");
                }

                return static_cast<llvm::Value *>(builder->getTrue());
            case scanning::EQUAL_EQUAL:
                if (left->getType() == getDoubleTy() && right->getType() == getDoubleTy()) {
                    return builder->CreateFCmpOEQ(left, right, "tmp_cmpEQ");
                }

                if ((left->getType() == getBoolTy() && right->getType() == getBoolTy())
                    || (left->getType() == getStringTy() && right->getType() == getStringTy())) {
                    return builder->CreateICmpEQ(left, right, "tmp_cmpEQ");
                }

                return static_cast<llvm::Value *>(builder->getFalse());
            case scanning::GREATER:
                checkNumberOperands(expr->op, left, right);
                // True if ordered and greater than
                return builder->CreateFCmpOGT(left, right, "tmp_cmpOGT");
            case scanning::GREATER_EQUAL:
                checkNumberOperands(expr->op, left, right);
                // True if ordered and greater than or equal
                return builder->CreateFCmpOGE(left, right, "tmp_cmpOGE");
            case scanning::LESS:
                checkNumberOperands(expr->op, left, right);
                // True if ordered and less than
                return builder->CreateFCmpOLT(left, right, "tmp_cmpOLT");
            case scanning::LESS_EQUAL:
                checkNumberOperands(expr->op, left, right);
                // True if ordered and less than or equal
                return builder->CreateFCmpOLE(left, right, "tmp_cmpOLE");
            case scanning::MINUS:
                checkNumberOperands(expr->op, left, right);
                return builder->CreateFSub(left, right, "tmp_sub");
            case scanning::PLUS:
                if (left->getType() == getDoubleTy() && right->getType() == getDoubleTy()) {
                    return builder->CreateFAdd(left, right, "tmp_add");
                }

                if (left->getType() == getStringTy() && right->getType() == getStringTy()) {
                    llvm::Function *calleeF = module->getFunction("__yyd_string_concat");
                    return static_cast<llvm::Value *>(builder->CreateCall(calleeF, {left, right}));
                }

                throw RuntimeError{expr->op, "Operands must be two numbers or two strings."};
            case scanning::SLASH:
                checkNumberOperands(expr->op, left, right);
                return builder->CreateFDiv(left, right, "tmp_div");
            case scanning::STAR:
                checkNumberOperands(expr->op, left, right);
                return builder->CreateFMul(left, right, "tmp_mul");
            default:
                // Unreachable.
                throw RuntimeError{expr->op, "Unknown binary operation."};
        }
    }

    std::any CodeGenerator::visitGroupingExpr(const std::shared_ptr<ast::Grouping> &expr) {
        return evaluate(expr->expression);
    }

    /* NOTE: you must return llvm::Value* */
    std::any CodeGenerator::visitLiteralExpr(const std::shared_ptr<ast::Literal> &expr) {
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

    std::any CodeGenerator::visitUnaryExpr(const std::shared_ptr<ast::Unary> &expr) {
        auto right = std::any_cast<llvm::Value *>(evaluate(expr->right));

        switch (expr->op.type) {
            case scanning::BANG:
                return builder->CreateXor(convertToBoolean(right), builder->getTrue(), "tmp_xor");
            case scanning::MINUS:
                checkNumberOperand(expr->op, right);
                return builder->CreateFNeg(right, "tmp_minus");
            default:
                // Unreachable.
                throw RuntimeError{expr->op, "Unknown unary operation."};
        }
    }

    std::any CodeGenerator::visitVariableExpr(const std::shared_ptr<ast::Variable> &expr) {
        try {
            auto record = environment->get(expr->name);
            auto load = builder->CreateLoad(record.type, record.allocation, expr->name.lexeme);
            return static_cast<llvm::Value *>(load);
        } catch (...) {
            auto function = module->getFunction(expr->name.lexeme);

            if (function != nullptr) {
                return static_cast<llvm::Function *>(function);
            } else {
                throw;
            }
        }
    }

    std::any CodeGenerator::visitLogicalExpr(const std::shared_ptr<ast::Logical> &expr) {
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

    std::any CodeGenerator::visitBlockStmt(const std::shared_ptr<ast::Block> &stmt) {
        executeBlock(stmt->statements, std::make_shared<Environment<EnvRecord>>(environment));
        return {};
    }

    void CodeGenerator::executeBlock(const std::vector<std::shared_ptr<ast::Stmt>> &statements,
                                     const std::shared_ptr<Environment<EnvRecord>> &env) {
        ++scopeId;

        auto previous = this->environment;
        this->environment = env;

        auto blockParent = builder->GetInsertBlock()->getParent();
        auto block = llvm::BasicBlock::Create(*context, "begin-" + std::to_string(scopeId), blockParent);
        auto afterBlock = llvm::BasicBlock::Create(*context, "end-" + std::to_string(scopeId));

        builder->CreateBr(block);
        builder->SetInsertPoint(block);
        try {
            for (const std::shared_ptr<ast::Stmt> &statement: statements) {
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

    std::any CodeGenerator::visitExpressionStmt(const std::shared_ptr<ast::Expression> &stmt) {
        return evaluate(stmt->expression);
    }

    std::any CodeGenerator::visitPrintStmt(const std::shared_ptr<ast::Print> &stmt) {
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

    std::any CodeGenerator::visitVarStmt(const std::shared_ptr<ast::Var> &stmt) {
        EnvRecord record{};

        if (stmt->initializer != nullptr) {
            auto value = std::any_cast<llvm::Value *>(evaluate(stmt->initializer));
            llvm::Value *alloca = builder->CreateAlloca(value->getType());
            record.type = value->getType();
            record.allocation = alloca;
            builder->CreateStore(value, alloca);
        }

        environment->define(stmt->name.lexeme, record);

        return {};
    }

    std::any CodeGenerator::visitIfStmt(const std::shared_ptr<ast::If> &stmt) {
        auto condition = std::any_cast<llvm::Value *>(evaluate(stmt->condition));

        if (!condition) {
            return nullptr; // todo: fix;
        }

        condition = convertToBoolean(condition);

        auto parent = builder->GetInsertBlock()->getParent();
        // Create blocks for the then and else cases.  Insert the 'then' block at the
        // end of the function.
        auto thenBranch =
                llvm::BasicBlock::Create(*context, "then", parent);
        auto elseBranch = llvm::BasicBlock::Create(*context, "else");
        auto mergeBranch = llvm::BasicBlock::Create(*context, "ifcont");

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

    std::any CodeGenerator::visitWhileStmt(const std::shared_ptr<ast::While> &stmt) {

        auto condition = std::any_cast<llvm::Value *>(evaluate(stmt->condition));
        // todo: handle that body or condition is null
        condition = convertToBoolean(condition);

        auto parent = builder->GetInsertBlock()->getParent();
        auto loopBody =
                llvm::BasicBlock::Create(*context, "loop", parent);
        auto mergeBranch = llvm::BasicBlock::Create(*context, "loopcont");

        builder->CreateCondBr(condition, loopBody, mergeBranch);
        builder->SetInsertPoint(loopBody);
        if (stmt->body != nullptr) {
            execute(stmt->body);
        }

        condition = std::any_cast<llvm::Value *>(evaluate(stmt->condition));
        // todo: handle that body or condition is null
        condition = convertToBoolean(condition);
        builder->CreateCondBr(condition, loopBody, mergeBranch);
        // Emit merge block.
        parent->getBasicBlockList().push_back(mergeBranch);
        builder->SetInsertPoint(mergeBranch);

        return {};
    }

    std::any CodeGenerator::visitCallExpr(const std::shared_ptr<ast::Call> &expr) {

        auto calleeFunc = std::any_cast<llvm::Function *>(evaluate(expr->callee));

        std::vector<llvm::Value *> arguments;
        for (const std::shared_ptr<ast::Expr> &argument: expr->arguments) {
            arguments.push_back(std::any_cast<llvm::Value *>(evaluate(argument)));
        }

        if (arguments.size() != calleeFunc->arg_size()) {
            throw RuntimeError{expr->paren, "Expected " +
                                            std::to_string(calleeFunc->arg_size()) + " arguments but got " +
                                            std::to_string(arguments.size()) + "."};
        }


        return static_cast<llvm::Value *>(builder->CreateCall(calleeFunc, arguments));
    }

    std::any CodeGenerator::visitFunctionStmt(std::shared_ptr<ast::Function> stmt) {
        std::vector<llvm::Type *> argsType;
        for (const auto &param: stmt->params) {
            llvm::Type *type;
            switch (param.first.type) {
                case scanning::STR:
                    type = getStringTy();
                    break;
                case scanning::NUM:
                    type = getDoubleTy();
                    break;
                case scanning::BOOL:
                    type = getBoolTy();
                    break;
                default:
                    // todo: refactor
                    throw std::runtime_error("invalid input type");
            }
            argsType.push_back(type);
        }

        llvm::Type *returnType;
        switch (stmt->returnType.type) {
            case scanning::STR:
                returnType = getStringTy();
                break;
            case scanning::NUM:
                returnType = getDoubleTy();
                break;
            case scanning::BOOL:
                returnType = getBoolTy();
                break;
            case scanning::VOID:
                returnType = getVoidTy();
                break;
            default:
                // todo: refactor
                throw std::runtime_error("invalid input type");
        }

        auto backupEnv = environment;
        auto backupBlock = builder->GetInsertBlock();

        llvm::FunctionType *funcType = llvm::FunctionType::get(returnType, argsType, false);
        llvm::Function *func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, stmt->name.lexeme,
                                                      module.get());

        llvm::BasicBlock *entryBlock = llvm::BasicBlock::Create(*context, "func-entry", func);
        builder->SetInsertPoint(entryBlock);

        environment = std::make_shared<Environment<EnvRecord>>(environment);

        for (size_t i = 0; i < func->arg_size(); ++i) {
            auto arg = func->getArg(i);
            auto [type, name] = stmt->params[i];
            arg->setName(name.lexeme);
            auto paramAlloca = builder->CreateAlloca(argsType[i]);
            builder->CreateStore(arg, paramAlloca);
            environment->define(name.lexeme, {paramAlloca, argsType[i]});
        }

        try {
            executeBlock(stmt->body, environment);
            if (returnType == getVoidTy()) {
                builder->CreateRetVoid();
            } else {
                // todo: refactor
                throw std::runtime_error("invalid return type");
            }
        } catch (YareYareDawaReturn &yareDawaReturn) {
            auto result = std::any_cast<llvm::Value *>(yareDawaReturn.value);
            if (result == nullptr) {
                if (returnType == getVoidTy()) {
                    builder->CreateRetVoid();
                } else {
                    throw std::runtime_error("invalid return type");
                }
            } else {
                if (result->getType() == returnType) {
                    builder->CreateRet(result);
                } else {
                    // todo: refactor
                    throw std::runtime_error("invalid return type");
                }
            }
        }

        environment = backupEnv;
        builder->SetInsertPoint(backupBlock);
        assert(!llvm::verifyFunction(*func, &llvm::outs()));
        return {};
    }

    std::any CodeGenerator::visitReturnStmt(std::shared_ptr<ast::Return> stmt) {
        llvm::Value *value = nullptr; // void
        if (stmt->value != nullptr) value = std::any_cast<llvm::Value *>(evaluate(stmt->value));

        throw YareYareDawaReturn({value});
    }
}
#endif