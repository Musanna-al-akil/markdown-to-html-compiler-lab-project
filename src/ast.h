#pragma once

#include <string>
#include <vector>

using namespace std;

enum NodeType {
    Document,
    Heading,
    Paragraph
};

struct Node {
    NodeType type;
    string text;
    int level;
    int line;
    vector<Node> children;
};

Node makeNode(NodeType type, string text, int line);
