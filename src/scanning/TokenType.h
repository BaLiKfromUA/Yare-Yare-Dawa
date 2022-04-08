//
// Created by balik on 4/8/2022.
//

#ifndef YARE_YARE_DAWA_TOKENTYPE_H
#define YARE_YARE_DAWA_TOKENTYPE_H

#include <string>

namespace scanning {

    enum TokenType {
        // Single-character tokens.
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

        // One or two character tokens.
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        // Literals.
        IDENTIFIER, STRING, NUMBER,

        // Keywords.
        AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
        PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

        END_OF_FILE
    };

    std::string toString(TokenType type) {
        static const std::string strings[] = {
                "LEFT_PAREN", "RIGHT_PAREN", "LEFT_BRACE", "RIGHT_BRACE",
                "COMMA", "DOT", "MINUS", "PLUS", "SEMICOLON", "SLASH", "STAR",
                "BANG", "BANG_EQUAL",
                "EQUAL", "EQUAL_EQUAL",
                "GREATER", "GREATER_EQUAL",
                "LESS", "LESS_EQUAL",
                "IDENTIFIER", "STRING", "NUMBER",
                "AND", "CLASS", "ELSE", "FALSE", "FUN", "FOR", "IF", "NIL", "OR",
                "PRINT", "RETURN", "SUPER", "THIS", "TRUE", "VAR", "WHILE",
                "END_OF_FILE"
        };

        return strings[static_cast<int>(type)];
    }
}

#endif //YARE_YARE_DAWA_TOKENTYPE_H
