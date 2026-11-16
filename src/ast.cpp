#include "ast.h"

using namespace std;

Node makeNode(NodeType type, string text, int line) {
    Node n;
    n.type = type;
    n.text = text;
    n.attr = "";
    n.level = 0;
    n.ordered = false;
    n.line = line;
    return n;
}

void addWarning(vector<Warning>& warnings, int line, string message) {
    Warning w;
    w.line = line;
    w.message = message;
    warnings.push_back(w);
}
