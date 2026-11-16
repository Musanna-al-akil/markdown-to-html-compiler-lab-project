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

bool hasInlineText(Node& node) {
    return node.type == Heading || node.type == Paragraph || node.type == ListItem;
}

string nodeTypeName(NodeType type) {
    switch (type) {
        case Document:   return "Document";
        case Heading:    return "Heading";
        case Paragraph:  return "Paragraph";
        case List:       return "List";
        case ListItem:   return "ListItem";
        case CodeBlock:  return "CodeBlock";
    }
    return "?";
}
