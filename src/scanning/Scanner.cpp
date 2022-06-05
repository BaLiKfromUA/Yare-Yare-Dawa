//
// Created by balik on 4/11/2022.
//

#include "Errors.h"
#include "scanning/Scanner.h"

namespace scanning {
    std::vector<Token> scanning::Scanner::scanTokens() {
        while (!isAtEnd()) {
            // We are at the beginning of the next lexeme.
            start = current;
            scanToken();
        }

        tokens.emplace_back(END_OF_FILE, "", nullptr, line);
        return tokens;
    }

    void Scanner::scanToken() {
        char c = advance();
        switch (c) {
            case '(':
                addToken(LEFT_PAREN);
                break;
            case ')':
                addToken(RIGHT_PAREN);
                break;
            case '{':
                addToken(LEFT_BRACE);
                break;
            case '}':
                addToken(RIGHT_BRACE);
                break;
            case ',':
                addToken(COMMA);
                break;
            case '.':
                addToken(DOT);
                break;
            case '-':
                addToken(MINUS);
                break;
            case '+':
                addToken(PLUS);
                break;
            case ';':
                addToken(SEMICOLON);
                break;
            case '*':
                addToken(STAR);
                break;
            case '!':
                addToken(match('=') ? BANG_EQUAL : BANG);
                break;
            case '=':
                addToken(match('=') ? EQUAL_EQUAL : EQUAL);
                break;
            case '<':
                addToken(match('=') ? LESS_EQUAL : LESS);
                break;
            case '>':
                addToken(match('=') ? GREATER_EQUAL : GREATER);
                break;
            case '/':
                if (match('/')) {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    addToken(SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            case '\n':
                ++line;
                break;
            case '"':
                string();
                break;
            default:
                if (std::isdigit(c)) {
                    number();
                } else if (std::isalpha(c) || c == '?') {
                    identifier();
                } else {
                    Errors::error(line, "Unexpected character.");
                }
                break;
        }
    }

    void Scanner::string() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') ++line;
            advance();
        }

        if (isAtEnd()) {
            Errors::error(line, "Unterminated string.");
            return;
        }

        // The closing ".
        advance();

        // Trim the surrounding quotes.
        std::string value{source.substr(start + 1, current - 2 - start)};
        addToken(STRING, value);
    }

    void Scanner::number() {
        while (std::isdigit(peek())) advance();

        // Look for a fractional part.
        if (peek() == '.' && std::isdigit(peekNext())) {
            // Consume the "."
            advance();

            while (std::isdigit(peek())) advance();
        }

        addToken(NUMBER,
                 std::stod(std::string{source.substr(start, current - start)}));
    }

    void Scanner::identifier() {
        while (std::isalnum(peek())) advance();


        std::string text = std::string{source.substr(start, current - start)};

        TokenType type;
        auto match = keywords.find(text);
        if (match == keywords.end()) {
            type = IDENTIFIER;
        } else {
            type = match->second;
        }

        addToken(type);
    }
}
