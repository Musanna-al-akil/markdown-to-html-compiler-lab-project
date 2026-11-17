#pragma once

#include <string>
#include <vector>

using namespace std;

enum TokenType {
    TEXT,
    STAR,
    DSTAR,
    BACKTICK,
    LBRACKET,
    RBRACKET,
    LPAREN,
    RPAREN,
    BANG,
    ESCAPE
};

struct Token {
    TokenType type;
    string text;
};

vector<Token> lexInline(string text);

string rawText(Token token);

string tokenTypeName(TokenType type);

string dumpTokens(vector<Token>& tokens);
