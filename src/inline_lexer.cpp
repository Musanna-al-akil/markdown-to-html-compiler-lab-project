#include "inline_lexer.h"

using namespace std;

Token makeToken(TokenType type, string text) {
    Token t;
    t.type = type;
    t.text = text;
    return t;
}

bool isSpecial(char c) {
    return c == '*' || c == '`' || c == '[' || c == ']' || c == '(' || c == ')' ||
           c == '!' || c == '\\';
}

bool isPunct(char c) {
    return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') ||
           (c >= '[' && c <= '`') || (c >= '{' && c <= '~');
}

TokenType singleCharType(char c) {
    if (c == '`') return BACKTICK;
    if (c == '[') return LBRACKET;
    if (c == ']') return RBRACKET;
    if (c == '(') return LPAREN;
    if (c == ')') return RPAREN;
    return BANG;
}

vector<Token> lexInline(string text) {
    vector<Token> tokens;
    TokenType lastOpener = DSTAR;
    int n = text.size();
    int i = 0;

    while (i < n) {
        char c = text[i];

        if (c == '*') {
            int stars = 0;
            while (i + stars < n && text[i + stars] == '*') stars++;

            bool closing = i > 0 && text[i - 1] != ' ' && text[i - 1] != '\t';
            bool odd = stars % 2 == 1;
            bool starFirst = odd && (!closing || lastOpener == STAR);

            if (starFirst) tokens.push_back(makeToken(STAR, "*"));
            for (int k = 0; k < stars / 2; k++) tokens.push_back(makeToken(DSTAR, "**"));
            if (odd && !starFirst) tokens.push_back(makeToken(STAR, "*"));

            if (!closing) lastOpener = tokens[tokens.size() - 1].type;
            i += stars;
        } else if (c == '\\' && i + 1 < n && isPunct(text[i + 1])) {
            tokens.push_back(makeToken(ESCAPE, string(1, text[i + 1])));
            i += 2;
        } else if (c != '\\' && isSpecial(c)) {
            tokens.push_back(makeToken(singleCharType(c), string(1, c)));
            i++;
        } else {
            string word = "";
            word += c;
            i++;
            while (i < n && !isSpecial(text[i])) {
                word += text[i];
                i++;
            }
            tokens.push_back(makeToken(TEXT, word));
        }
    }
    return tokens;
}

string rawText(Token token) {
    if (token.type == ESCAPE) return "\\" + token.text;
    return token.text;
}

string tokenTypeName(TokenType type) {
    switch (type) {
        case TEXT:     return "TEXT";
        case STAR:     return "STAR";
        case DSTAR:    return "DSTAR";
        case BACKTICK: return "BACKTICK";
        case LBRACKET: return "LBRACKET";
        case RBRACKET: return "RBRACKET";
        case LPAREN:   return "LPAREN";
        case RPAREN:   return "RPAREN";
        case BANG:     return "BANG";
        case ESCAPE:   return "ESCAPE";
    }
    return "?";
}

string dumpTokens(vector<Token>& tokens) {
    string out = "";
    for (int i = 0; i < (int)tokens.size(); i++) {
        out += "  " + tokenTypeName(tokens[i].type) + " \"" + tokens[i].text + "\"\n";
    }
    return out;
}
