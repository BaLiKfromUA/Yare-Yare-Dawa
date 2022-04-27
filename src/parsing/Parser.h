//
// Created by balik on 4/11/2022.
//

#ifndef YARE_YARE_DAWA_PARSER_H
#define YARE_YARE_DAWA_PARSER_H


#include <vector>
#include <cassert>
#include "Errors.h"
#include "scanning/Token.h"
#include "scanning/TokenType.h"
#include "ast/Expression.h"
#include "ast/Statement.h"


namespace parsing {

    class Parser {
    private:
        struct ParseError : public std::runtime_error {
            using std::runtime_error::runtime_error;
        };

        const std::vector<scanning::Token> &tokens;
        int current = 0;

    public:
        explicit Parser(const std::vector<scanning::Token> &tokens)
                : tokens{tokens} {}

        // program        → statement* EOF ;
        std::vector<std::shared_ptr<ast::Stmt>> parse() {
            std::vector<std::shared_ptr<ast::Stmt>> statements;
            while (!isAtEnd()) {
                statements.push_back(statement());
                //statements.push_back(declaration());
            }

            return statements;
        }

    private:
        /*== Helpers ==*/

        static ParseError error(const scanning::Token &token, std::string_view message) {
            Errors::error(token, message);
            return ParseError{""};
        }

        void synchronize();

        scanning::Token advance() {
            if (!isAtEnd()) ++current;
            return previous();
        }

        bool isAtEnd() {
            return peek().type == scanning::END_OF_FILE;
        }

        scanning::Token peek() {
            return tokens.at(current);
        }

        scanning::Token previous() {
            return tokens.at(current - 1);
        }

        bool check(scanning::TokenType type) {
            if (isAtEnd()) return false;
            return peek().type == type;
        }

        scanning::Token consume(scanning::TokenType type, std::string_view message) {
            if (check(type)) return advance();

            throw error(peek(), message);
        }


        template<class... T>
        bool match(T... type) {
            assert((... && std::is_same_v<T, scanning::TokenType>));

            if ((... || check(type))) {
                advance();
                return true;
            }

            return false;
        }

        /*== Grammar ==*/

        // statement      → exprStmt | printStmt ;
        std::shared_ptr<ast::Stmt> statement() {
            if (match(scanning::PRINT)) return printStatement();

            return expressionStatement();
        }

        // printStmt      → "print" expression ";" ;
        std::shared_ptr<ast::Stmt> printStatement() {
            std::shared_ptr<ast::Expr> value = expression();
            consume(scanning::SEMICOLON, "Expect ';' after value.");
            return std::make_shared<ast::Print>(value);
        }

        // exprStmt       → expression ";" ;
        std::shared_ptr<ast::Stmt> expressionStatement() {
            std::shared_ptr<ast::Expr> expr = expression();
            consume(scanning::SEMICOLON, "Expect ';' after expression.");
            return std::make_shared<ast::Expression>(expr);
        }

        // expression     → equality ;
        std::shared_ptr<ast::Expr> expression() {
            return equality();
        }

        // equality       → comparison ( ( "!=" | "==" ) comparison )* ;
        std::shared_ptr<ast::Expr> equality();

        // comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
        std::shared_ptr<ast::Expr> comparison();

        // term           → factor ( ( "-" | "+" ) factor )* ;
        std::shared_ptr<ast::Expr> term();

        // factor         → unary ( ( "/" | "*" ) unary )* ;
        std::shared_ptr<ast::Expr> factor();

        // unary          → ( "!" | "-" ) unary | primary ;
        std::shared_ptr<ast::Expr> unary();

        // primary        → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
        std::shared_ptr<ast::Expr> primary();

    };
}


#endif //YARE_YARE_DAWA_PARSER_H
