//
// Created by balik on 4/20/2022.
//
#include <gtest/gtest.h>
#include "scanning/Scanner.h"

TEST(ScannerTest, BasicScanning) {
    using namespace scanning;
    std::string input = "// this is a comment\n"
                        "(( )){} // grouping stuff\n"
                        "!*+-/=<> <= == // operators";

    auto scanner = Scanner(input);

    std::vector<TokenType> expectedTokens = {LEFT_PAREN, LEFT_PAREN, RIGHT_PAREN, RIGHT_PAREN,
                                             LEFT_BRACE, RIGHT_BRACE,
                                             BANG, STAR, PLUS, MINUS, SLASH,
                                             EQUAL, LESS, GREATER, LESS_EQUAL, EQUAL_EQUAL, END_OF_FILE};
    auto actualTokens = scanner.scanTokens();

    EXPECT_EQ(expectedTokens.size(), actualTokens.size());

    for (int i = 0; i < expectedTokens.size(); ++i) {
        EXPECT_EQ(expectedTokens[i], actualTokens[i].type);
    }
}