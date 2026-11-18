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

string plainText(Node& node) {
    if (node.type == Text || node.type == Code || node.type == Image) return node.text;
    string out = "";
    for (int i = 0; i < (int)node.children.size(); i++) out += plainText(node.children[i]);
    return out;
}

string nodeTypeName(NodeType type) {
    switch (type) {
        case Document:   return "Document";
        case Heading:    return "Heading";
        case Paragraph:  return "Paragraph";
        case List:       return "List";
        case ListItem:   return "ListItem";
        case CodeBlock:  return "CodeBlock";
        case Text:       return "Text";
        case Strong:     return "Strong";
        case Emph:       return "Emph";
        case Code:       return "Code";
        case Link:       return "Link";
        case Image:      return "Image";
    }
    return "?";
}

string quoted(string s) {
    string out = "\"";
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '\n') out += "\\n";
        else out += s[i];
    }
    return out + "\"";
}

string nodeDetails(Node& node) {
    if (node.type == Heading) return " level=" + to_string(node.level);
    if (node.type == List) {
        if (node.ordered) return " ordered";
        return " bullet";
    }
    if (node.type == CodeBlock) return " lang=" + quoted(node.attr) + " " + quoted(node.text);
    if (node.type == Link) return " href=" + quoted(node.attr);
    if (node.type == Image) return " src=" + quoted(node.attr) + " alt=" + quoted(node.text);
    if (node.type == Text || node.type == Code) return " " + quoted(node.text);
    return "";
}

string dumpAst(Node& node, int depth) {
    string out = "";
    for (int i = 0; i < depth * 2; i++) out += " ";
    out += nodeTypeName(node.type) + nodeDetails(node) + "\n";
    for (int i = 0; i < (int)node.children.size(); i++) {
        out += dumpAst(node.children[i], depth + 1);
    }
    return out;
}
