#pragma once

#include <string>
#include <vector>

using namespace std;

enum NodeType {
    Document,
    Heading,
    Paragraph,
    List,
    ListItem,
    CodeBlock,
    Text,
    Strong,
    Emph,
    Code,
    Link,
    Image
};

struct Node {
    NodeType type;
    string text;
    string attr;
    int level;
    bool ordered;
    int line;
    vector<Node> children;
};

struct Warning {
    int line;
    string message;
};

Node makeNode(NodeType type, string text, int line);

void addWarning(vector<Warning>& warnings, int line, string message);

bool hasInlineText(Node& node);

string plainText(Node& node);

string nodeTypeName(NodeType type);

string dumpAst(Node& node, int depth);
