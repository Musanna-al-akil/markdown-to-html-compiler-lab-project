#include "inline_parser.h"

using namespace std;

vector<Token> tokens;
int tokenPos = 0;
int inlineLine = 0;
vector<TokenType> closers;

bool atEnd() {
    return tokenPos >= (int)tokens.size();
}

bool currentIs(TokenType type) {
    return !atEnd() && tokens[tokenPos].type == type;
}

bool atAnyCloser() {
    if (atEnd()) return false;
    for (int i = 0; i < (int)closers.size(); i++) {
        if (tokens[tokenPos].type == closers[i]) return true;
    }
    return false;
}

void addNode(vector<Node>& out, Node node) {
    if (node.type == Text && out.size() > 0 && out[out.size() - 1].type == Text) {
        out[out.size() - 1].text += node.text;
    } else {
        out.push_back(node);
    }
}

void addText(vector<Node>& out, string text) {
    addNode(out, makeNode(Text, text, inlineLine));
}

void addAll(vector<Node>& out, vector<Node>& nodes) {
    for (int i = 0; i < (int)nodes.size(); i++) addNode(out, nodes[i]);
}

vector<Node> parseSequence();

vector<Node> parseUntil(TokenType closer) {
    closers.push_back(closer);
    vector<Node> inner = parseSequence();
    closers.pop_back();
    return inner;
}

void parseCodeSpan(vector<Node>& out) {
    int k = tokenPos + 1;
    string code = "";
    while (k < (int)tokens.size() && tokens[k].type != BACKTICK) {
        code += rawText(tokens[k]);
        k++;
    }
    if (k == (int)tokens.size()) {
        addText(out, "`");
        tokenPos++;
        return;
    }
    out.push_back(makeNode(Code, code, inlineLine));
    tokenPos = k + 1;
}

bool endsWithSpace(vector<Node>& nodes) {
    if (nodes.size() == 0) return false;
    Node last = nodes[nodes.size() - 1];
    return last.type == Text && last.text[last.text.size() - 1] == ' ';
}

void parseEmphasis(vector<Node>& out, TokenType delim, NodeType type) {
    string opener = tokens[tokenPos].text;
    tokenPos++;
    if (atEnd() || (currentIs(TEXT) && tokens[tokenPos].text[0] == ' ')) {
        addText(out, opener);
        return;
    }
    vector<Node> inner = parseUntil(delim);
    if (currentIs(delim) && inner.size() > 0 && !endsWithSpace(inner)) {
        tokenPos++;
        Node node = makeNode(type, "", inlineLine);
        node.children = inner;
        out.push_back(node);
    } else {
        addText(out, opener);
        addAll(out, inner);
    }
}

bool isUrlToken(Token t) {
    if (t.type == ESCAPE) return true;
    if (t.type != TEXT) return false;
    for (int i = 0; i < (int)t.text.size(); i++) {
        if (t.text[i] == ' ' || t.text[i] == '\t') return false;
    }
    return true;
}

bool readUrl(string& url) {
    int k = tokenPos + 1;
    if (k >= (int)tokens.size() || tokens[k].type != LPAREN) return false;
    k++;
    while (k < (int)tokens.size() && isUrlToken(tokens[k])) {
        url += tokens[k].text;
        k++;
    }
    if (k >= (int)tokens.size() || tokens[k].type != RPAREN) return false;
    tokenPos = k + 1;
    return true;
}

void parseLinkOrImage(vector<Node>& out, bool isImage) {
    if (isImage) tokenPos += 2;
    else tokenPos += 1;
    vector<Node> inner = parseUntil(RBRACKET);

    string url = "";
    if (currentIs(RBRACKET) && readUrl(url)) {
        Node node;
        if (isImage) {
            node = makeNode(Image, "", inlineLine);
            for (int i = 0; i < (int)inner.size(); i++) node.text += plainText(inner[i]);
        } else {
            node = makeNode(Link, "", inlineLine);
            node.children = inner;
        }
        node.attr = url;
        out.push_back(node);
        return;
    }

    if (isImage) addText(out, "![");
    else addText(out, "[");
    addAll(out, inner);
    if (currentIs(RBRACKET)) {
        addText(out, "]");
        tokenPos++;
    }
}

void parseInline(vector<Node>& out) {
    Token tok = tokens[tokenPos];
    bool nextIsBracket = tokenPos + 1 < (int)tokens.size() && tokens[tokenPos + 1].type == LBRACKET;

    if (tok.type == BACKTICK) {
        parseCodeSpan(out);
    } else if (tok.type == STAR) {
        parseEmphasis(out, STAR, Emph);
    } else if (tok.type == DSTAR) {
        parseEmphasis(out, DSTAR, Strong);
    } else if (tok.type == LBRACKET) {
        parseLinkOrImage(out, false);
    } else if (tok.type == BANG && nextIsBracket) {
        parseLinkOrImage(out, true);
    } else {
        addText(out, tok.text);
        tokenPos++;
    }
}

vector<Node> parseSequence() {
    vector<Node> out;
    while (!atEnd() && !atAnyCloser()) parseInline(out);
    return out;
}

vector<Node> parseInlineTokens(vector<Token>& tokenList, int line) {
    tokens = tokenList;
    tokenPos = 0;
    inlineLine = line;
    closers.clear();
    return parseSequence();
}

void parseAllInlines(Node& node) {
    if (hasInlineText(node)) {
        vector<Token> tokenList = lexInline(node.text);
        node.children = parseInlineTokens(tokenList, node.line);
        return;
    }
    for (int i = 0; i < (int)node.children.size(); i++) parseAllInlines(node.children[i]);
}

string dumpAllTokens(Node& node) {
    if (hasInlineText(node)) {
        vector<Token> tokenList = lexInline(node.text);
        string out = nodeTypeName(node.type) + " (line " + to_string(node.line) + "): " + node.text + "\n";
        return out + dumpTokens(tokenList);
    }
    string out = "";
    for (int i = 0; i < (int)node.children.size(); i++) out += dumpAllTokens(node.children[i]);
    return out;
}
