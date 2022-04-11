//
// Created by balik on 4/8/2022.
//

#ifndef YARE_YARE_DAWA_TOKEN_H
#define YARE_YARE_DAWA_TOKEN_H

#include "TokenType.h"
#include <any>

namespace scanning {
    class Token {
    public:
        const TokenType type;
        const std::string lexeme;
        const std::any literal;
        const int line;

        Token(TokenType type, std::string lexeme, std::any literal, int line)
                : type{type}, lexeme{std::move(lexeme)},
                  literal{std::move(literal)}, line{line} {

        }

        [[nodiscard]] std::string toString() const{
            std::string literal_text;
            // todo: make this Object.toString-like later!!!
            switch (type) {
                case (IDENTIFIER):
                    literal_text = lexeme;
                    break;
                case (STRING):
                    literal_text = std::any_cast<std::string>(literal);
                    break;
                case (NUMBER):
                    literal_text = std::to_string(std::any_cast<double>(literal));
                    break;
                case (TRUE):
                    literal_text = "true";
                    break;
                case (FALSE):
                    literal_text = "false";
                    break;
                default:
                    literal_text = "nil";
            }

            return ::scanning::toString(type) + " " + lexeme + " " + literal_text;
        }
    };
}

#endif //YARE_YARE_DAWA_TOKEN_H
