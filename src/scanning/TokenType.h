//
// Created by balik on 4/8/2022.
//

#ifndef YARE_YARE_DAWA_TOKENTYPE_H
#define YARE_YARE_DAWA_TOKENTYPE_H

#include <string>

namespace scanning {

    enum TokenType {
        LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        IDENTIFIER, STRING, NUMBER,

        AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
        PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

        END_OF_FILE,

        // Types
        VOID_TYPE, NUM_TYPE, STR_TYPE, BOOL_TYPE,

        // Input functions
        INPUT_NUM, INPUT_STR, INPUT_BOOL
    };


    inline std::string toString(TokenType type) {
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
                "END_OF_FILE",
                "VOID_TYPE", "NUM_TYPE", "STR_TYPE", "BOOL_TYPE",
                "INPUT_NUM", "INPUT_STR", "INPUT_BOOL"
        };

        return strings[static_cast<int>(type)];
    }
}

#endif //YARE_YARE_DAWA_TOKENTYPE_H
