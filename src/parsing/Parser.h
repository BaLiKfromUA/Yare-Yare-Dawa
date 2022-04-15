//
// Created by balik on 4/11/2022.
//

#ifndef YARE_YARE_DAWA_PARSER_H
#define YARE_YARE_DAWA_PARSER_H


#include <vector>
#include <cassert>
#include "YareYareDawaRuntime.h"
#include "scanning/Token.h"
#include "scanning/TokenType.h"
#include "ast/Expression.h"


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

        std::shared_ptr<ast::Expr> parse() {
            try {
                return expression();
            } catch (const ParseError &ignored) {
                return nullptr;
            }
        }

    private:
        /*== Helpers ==*/

        static ParseError error(const scanning::Token &token, std::string_view message) {
            YareYareDawaRuntime::error(token, message);
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