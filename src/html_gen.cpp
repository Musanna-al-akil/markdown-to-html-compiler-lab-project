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

string htmlCodeBlock(Node& node) {
    string open = "<code>";
    if (node.attr != "") open = "<code class=\"language-" + escapeHtml(node.attr) + "\">";
    string body = "";
    if (node.text != "") body = escapeHtml(node.text) + "\n";
    return "<pre>" + open + body + "</code></pre>\n";
}

string generateHtml(Node& node) {
    if (node.type == Document) return htmlChildren(node);
    if (node.type == Heading) {
        string tag = "h" + to_string(node.level);
        return "<" + tag + " id=\"" + escapeHtml(node.attr) + "\">" + htmlChildren(node) + "</" + tag + ">\n";
    }
    if (node.type == Paragraph) return wrap("p", htmlChildren(node)) + "\n";
    if (node.type == List) {
        string tag = "ul";
        if (node.ordered) tag = "ol";
        return "<" + tag + ">\n" + htmlChildren(node) + "</" + tag + ">\n";
    }
    if (node.type == ListItem) return wrap("li", htmlChildren(node)) + "\n";
    if (node.type == CodeBlock) return htmlCodeBlock(node);
    if (node.type == Text) return escapeHtml(node.text);
    if (node.type == Strong) return wrap("strong", htmlChildren(node));
    if (node.type == Emph) return wrap("em", htmlChildren(node));
    if (node.type == Code) return wrap("code", escapeHtml(node.text));
    if (node.type == Link) {
        return "<a href=\"" + escapeHtml(node.attr) + "\">" + htmlChildren(node) + "</a>";
    }
    return "<img src=\"" + escapeHtml(node.attr) + "\" alt=\"" + escapeHtml(node.text) + "\">";
}
