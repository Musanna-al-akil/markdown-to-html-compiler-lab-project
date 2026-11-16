#include "ast.h"

using namespace std;

Node makeNode(NodeType type, string text, int line) {
    Node n;
    n.type = type;
    n.text = text;
    n.level = 0;
    n.line = line;
    return n;
}
