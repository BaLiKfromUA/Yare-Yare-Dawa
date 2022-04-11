#include <gtest/gtest.h>
#include <vector>
#include "scanning/Scanner.h"
#include "scanning/TokenType.h"

using namespace scanning;

TEST(ScannerTest, BasicScanning) {
    std::string input = "// this is a comment\n"
                        "(( )){} // grouping stuff\n"
                        "!*+-/=<> <= == // operators";

    auto scanner = Scanner(input);

    std::vector<TokenType> expectedTokens = {LEFT_PAREN, LEFT_PAREN, RIGHT_PAREN, RIGHT_PAREN,
                                             LEFT_BRACE, RIGHT_BRACE,
                                             BANG, STAR, PLUS, MINUS, SLASH,
                                             EQUAL, LESS, GREATER, LESS_EQUAL, EQUAL_EQUAL};
    auto actualTokens = scanner.scanTokens();

    EXPECT_EQ(expectedTokens.size(), actualTokens.size());

    for (int i = 0; i < expectedTokens.size(); ++i) {
        EXPECT_EQ(expectedTokens[i], actualTokens[i].type);
    }
}

