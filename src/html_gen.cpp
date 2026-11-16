#include "html_gen.h"

using namespace std;

string escapeHtml(string text) {
    string out = "";
    for (int i = 0; i < (int)text.size(); i++) {
        char c = text[i];
        if (c == '<') out += "&lt;";
        else if (c == '>') out += "&gt;";
        else if (c == '&') out += "&amp;";
        else if (c == '"') out += "&quot;";
        else out += c;
    }
    return out;
}

string htmlChildren(Node& node) {
    string out = "";
    for (int i = 0; i < (int)node.children.size(); i++) out += generateHtml(node.children[i]);
    return out;
}

string wrap(string tag, string inside) {
    return "<" + tag + ">" + inside + "</" + tag + ">";
}

string generateHtml(Node& node) {
    if (node.type == Document) return htmlChildren(node);
    if (node.type == Heading) {
        string tag = "h" + to_string(node.level);
        return "<" + tag + ">" + escapeHtml(node.text) + "</" + tag + ">\n";
    }
    return wrap("p", escapeHtml(node.text)) + "\n";
}
