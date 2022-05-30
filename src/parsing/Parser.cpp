//
// Created by balik on 4/11/2022.
//

#include "Parser.h"


namespace parsing {

    std::shared_ptr<ast::Expr> Parser::equality() {
        std::shared_ptr<ast::Expr> expr = comparison();

        while (match(scanning::BANG_EQUAL, scanning::EQUAL_EQUAL)) {
            scanning::Token op = previous();
            std::shared_ptr<ast::Expr> right = comparison();
            expr = std::make_shared<ast::Binary>(expr, std::move(op), right);
        }

        return expr;
    }

    std::shared_ptr<ast::Expr> Parser::comparison() {
        std::shared_ptr<ast::Expr> expr = term();

        while (match(scanning::GREATER, scanning::GREATER_EQUAL, scanning::LESS, scanning::LESS_EQUAL)) {
            scanning::Token op = previous();
            std::shared_ptr<ast::Expr> right = term();
            expr = std::make_shared<ast::Binary>(expr, std::move(op), right);
        }

        return expr;
    }

    std::shared_ptr<ast::Expr> Parser::term() {
        std::shared_ptr<ast::Expr> expr = factor();

        while (match(scanning::MINUS, scanning::PLUS)) {
            scanning::Token op = previous();
            std::shared_ptr<ast::Expr> right = factor();
            expr = std::make_shared<ast::Binary>(expr, std::move(op), right);
        }

        return expr;
    }

    std::shared_ptr<ast::Expr> Parser::factor() {
        std::shared_ptr<ast::Expr> expr = unary();

        while (match(scanning::SLASH, scanning::STAR)) {
            scanning::Token op = previous();
            std::shared_ptr<ast::Expr> right = unary();
            expr = std::make_shared<ast::Binary>(expr, std::move(op), right);
        }

        return expr;
    }

    std::shared_ptr<ast::Expr> Parser::unary() {
        if (match(scanning::BANG, scanning::MINUS)) {
            scanning::Token op = previous();
            std::shared_ptr<ast::Expr> right = unary();
            return std::make_shared<ast::Unary>(std::move(op), right);
        }

        return call();
    }

    std::shared_ptr<ast::Expr> Parser::primary() {
        if (match(scanning::FALSE)) return std::make_shared<ast::Literal>(false);
        if (match(scanning::TRUE)) return std::make_shared<ast::Literal>(true);
        if (match(scanning::NIL)) return std::make_shared<ast::Literal>(nullptr);

        if (match(scanning::NUMBER, scanning::STRING)) {
            return std::make_shared<ast::Literal>(previous().literal);
        }

        if (match(scanning::IDENTIFIER)) {
            return std::make_shared<ast::Variable>(previous());
        }

        if (match(scanning::LEFT_PAREN)) {
            std::shared_ptr<ast::Expr> expr = expression();
            consume(scanning::RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_shared<ast::Grouping>(expr);
        }

        throw error(peek(), "Expect expression.");
    }

    void Parser::synchronize() {
        advance();

        while (!isAtEnd()) {
            if (previous().type == scanning::SEMICOLON) return;

            switch (peek().type) {
                case scanning::CLASS:
                case scanning::FUN:
                case scanning::VAR:
                case scanning::FOR:
                case scanning::IF:
                case scanning::WHILE:
                case scanning::PRINT:
                case scanning::RETURN:
                    return;
                default:
                    advance();
            }
        }
    }

    std::shared_ptr<ast::Expr> Parser::orExpression() {
        std::shared_ptr<ast::Expr> expr = andExpression();

        while (match(scanning::OR)) {
            auto op = previous();
            auto right = andExpression();
            expr = std::make_shared<ast::Logical>(expr, std::move(op), right);
        }

        return expr;
    }

    std::shared_ptr<ast::Expr> Parser::andExpression() {
        auto expr = equality();

        while (match(scanning::AND)) {
            auto op = previous();
            auto right = equality();
            expr = std::make_shared<ast::Logical>(expr, std::move(op), right);
        }

        return expr;
    }

    std::shared_ptr<ast::Stmt> Parser::ifStatement() {
        consume(scanning::LEFT_PAREN, "Expect '(' after 'if'.");
        auto condition = expression();
        consume(scanning::RIGHT_PAREN, "Expect ')' after if condition.");

        auto thenBranch = statement();
        std::shared_ptr<ast::Stmt> elseBranch = nullptr;
        if (match(scanning::ELSE)) {
            elseBranch = statement();
        }

        return std::make_shared<ast::If>(condition, thenBranch, elseBranch);
    }

    std::shared_ptr<ast::Stmt> Parser::whileStatement() {
        consume(scanning::LEFT_PAREN, "Expect '(' after 'while'.");
        auto condition = expression();
        consume(scanning::RIGHT_PAREN, "Expect ')' after condition.");
        auto body = statement();

        return std::make_shared<ast::While>(condition, body);
    }

    std::shared_ptr<ast::Stmt> Parser::forStatement() {
        consume(scanning::LEFT_PAREN, "Expect '(' after 'for'.");

        std::shared_ptr<ast::Stmt> initializer;
        if (match(scanning::SEMICOLON)) {
            initializer = nullptr;
        } else if (match(scanning::VAR)) {
            initializer = varDeclaration();
        } else {
            initializer = expressionStatement();
        }

        std::shared_ptr<ast::Expr> condition = nullptr;
        if (!check(scanning::SEMICOLON)) {
            condition = expression();
        }
        consume(scanning::SEMICOLON, "Expect ';' after loop condition.");

        std::shared_ptr<ast::Expr> increment = nullptr;
        if (!check(scanning::RIGHT_PAREN)) {
            increment = expression();
        }
        consume(scanning::RIGHT_PAREN, "Expect ')' after for clauses.");
        std::shared_ptr<ast::Stmt> body = statement();

        if (increment != nullptr) {
            body = std::make_shared<ast::Block>(
                    std::vector<std::shared_ptr<ast::Stmt>>{
                            body,
                            std::make_shared<ast::Expression>(increment)});
        }

        if (condition == nullptr) {
            condition = std::make_shared<ast::Literal>(true);
        }
        body = std::make_shared<ast::While>(condition, body);

        if (initializer != nullptr) {
            body = std::make_shared<ast::Block>(
                    std::vector<std::shared_ptr<ast::Stmt>>{initializer, body});
        }

        return body;
    }

    std::shared_ptr<ast::Expr> Parser::call() {
        auto expr = primary();

        while (true) {
            if (match(scanning::LEFT_PAREN)) {
                expr = finishCall(expr);
            } else {
                break;
            }
        }

        return expr;
    }

    std::shared_ptr<ast::Expr> Parser::finishCall(const std::shared_ptr<ast::Expr> &callee) {
        std::vector<std::shared_ptr<ast::Expr>> arguments;
        if (!check(scanning::RIGHT_PAREN)) {
            do {
                if (arguments.size() >= 255) {
                    error(peek(), "Can't have more than 255 arguments.");
                }
                arguments.push_back(expression());
            } while (match(scanning::COMMA));
        }

        auto paren = consume(scanning::RIGHT_PAREN,
                             "Expect ')' after arguments.");

        return std::make_shared<ast::Call>(callee,
                                           std::move(paren),
                                           std::move(arguments));
    }

}