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

        return primary();
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
}