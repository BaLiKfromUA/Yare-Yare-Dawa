//
// Created by balik on 07.05.22.
//

#ifndef YARE_YARE_DAWA_ASTVISITOR_H
#define YARE_YARE_DAWA_ASTVISITOR_H

namespace visitor {

    class AstVisitor : public ast::ExprVisitor, public ast::StmtVisitor {
    public:
        virtual void visitAST(const std::vector<std::shared_ptr<ast::Stmt>> &statements) = 0;

    protected:
        void execute(const std::shared_ptr<ast::Stmt> &stmt) {
            stmt->accept(*this);
        }

        std::any evaluate(const std::shared_ptr<ast::Expr> &expr) {
            return expr->accept(*this);
        }

        virtual std::any validateType(scanning::TokenType requiredToken, const std::any& candidateValue, bool checkVoid) = 0;

        virtual void enableStandardLibrary() = 0;
    };

}

#endif //YARE_YARE_DAWA_ASTVISITOR_H
