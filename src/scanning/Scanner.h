//
// Created by balik on 4/11/2022.
//

#ifndef YARE_YARE_DAWA_SCANNER_H
#define YARE_YARE_DAWA_SCANNER_H


#include <string>
#include <map>
#include <vector>

#include "Token.h"
#include "TokenType.h"

namespace scanning {
    class Scanner {
    private:
        static const std::map<std::string, TokenType> keywords;

        std::string_view source;
        std::vector<Token> tokens;
        int start = 0;
        int current = 0;
        int line = 1;

    public:

        explicit Scanner(std::string_view source)
                : source{source} {}

        std::vector<Token> scanTokens();

    private:
        void scanToken();

        void identifier();

        /*== literal scanners ==*/
        void string();

        void number();

        /*== helpers ==*/

        char peekNext() {
            if (current + 1 >= source.length()) return '\0';
            return source.at(current + 1);
        }

        char peek() {
            if (isAtEnd()) return '\0';
            return source.at(current);
        }

        bool match(char expected) {
            if (isAtEnd()) return false;
            if (source.at(current) != expected) return false;

            ++current;
            return true;
        }

        bool isAtEnd() { return current >= source.length(); }

        char advance() { return source.at(current++); }

        void addToken(TokenType type) { addToken(type, nullptr); }

        void addToken(TokenType type, std::any literal) {
            std::string text{source.substr(start, current - start)};
            tokens.emplace_back(type, std::move(text), std::move(literal),
                                line);
        }
    };

    inline const std::map<std::string, TokenType> Scanner::keywords =
            {
                    {"and",    TokenType::AND},
                    {"class",  TokenType::CLASS},
                    {"else",   TokenType::ELSE},
                    {"false",  TokenType::FALSE},
                    {"for",    TokenType::FOR},
                    {"fun",    TokenType::FUN},
                    {"if",     TokenType::IF},
                    {"nil",    TokenType::NIL},
                    {"or",     TokenType::OR},
                    {"print",  TokenType::PRINT},
                    {"return", TokenType::RETURN},
                    {"super",  TokenType::SUPER},
                    {"this",   TokenType::THIS},
                    {"true",   TokenType::TRUE},
                    {"var",    TokenType::VAR},
                    {"while",  TokenType::WHILE},
                    {"void",  TokenType::VOID},
                    {"str",   TokenType::STR},
                    {"num",   TokenType::NUM},
                    {"bool",  TokenType::BOOL}
            };
}


#endif //YARE_YARE_DAWA_SCANNER_H
