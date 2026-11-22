#include "block_parser.h"

using namespace std;

bool isSpaceOrTab(char c) {
    return c == ' ' || c == '\t';
}

string trim(string s) {
    int start = 0;
    while (start < (int)s.size() && isSpaceOrTab(s[start])) start++;
    int end = (int)s.size() - 1;
    while (end >= start && isSpaceOrTab(s[end])) end--;
    return s.substr(start, end - start + 1);
}

bool isBlank(string line) {
    return trim(line) == "";
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

int headingLevel(string line) {
    int hashes = 0;
    while (hashes < (int)line.size() && line[hashes] == '#') hashes++;
    if (hashes == 0 || hashes > 6) return 0;
    if (hashes < (int)line.size() && line[hashes] != ' ') return 0;
    return hashes;
}

bool isFence(string line) {
    return trim(line).substr(0, 3) == "```";
}

bool isListItem(string line, bool& ordered, int& contentStart) {
    if (line.size() >= 2 && (line[0] == '-' || line[0] == '*') && line[1] == ' ') {
        ordered = false;
        contentStart = 2;
        return true;
    }
    int digits = 0;
    while (digits < (int)line.size() && isDigit(line[digits])) digits++;
    if (digits > 0 && digits + 1 < (int)line.size() && line[digits] == '.' && line[digits + 1] == ' ') {
        ordered = true;
        contentStart = digits + 2;
        return true;
    }
    return false;
}

bool startsListItem(string line) {
    bool ordered;
    int contentStart;
    return isListItem(line, ordered, contentStart);
}

bool startsNewBlock(string line) {
    return isBlank(line) || headingLevel(line) > 0 || isFence(line) || startsListItem(line);
}

Node parseHeading(vector<string>& lines, int& pos) {
    string line = lines[pos];
    int level = headingLevel(line);
    Node heading = makeNode(Heading, trim(line.substr(level)), pos + 1);
    heading.level = level;
    pos++;
    return heading;
}

Node parseCodeBlock(vector<string>& lines, int& pos, vector<Warning>& warnings) {
    Node code = makeNode(CodeBlock, "", pos + 1);
    code.attr = trim(trim(lines[pos]).substr(3));
    pos++;
    vector<string> body;
    while (pos < (int)lines.size() && !isFence(lines[pos])) {
        body.push_back(lines[pos]);
        pos++;
    }
    if (pos < (int)lines.size()) {
        pos++;
    } else {
        addWarning(warnings, code.line, "unclosed code fence (code block runs to end of file)");
    }
    for (int k = 0; k < (int)body.size(); k++) {
        if (k > 0) code.text += "\n";
        code.text += body[k];
    }
    return code;
}

bool continuesList(string line, bool ordered) {
    bool itemOrdered;
    int contentStart;
    return isListItem(line, itemOrdered, contentStart) && itemOrdered == ordered;
}

Node parseListItem(vector<string>& lines, int& pos) {
    bool ordered;
    int contentStart;
    isListItem(lines[pos], ordered, contentStart);
    Node item = makeNode(ListItem, trim(lines[pos].substr(contentStart)), pos + 1);
    pos++;
    while (pos < (int)lines.size() && !startsNewBlock(lines[pos])) {
        item.text += " " + trim(lines[pos]);
        pos++;
    }
    return item;
}

Node parseList(vector<string>& lines, int& pos) {
    Node list = makeNode(List, "", pos + 1);
    int contentStart;
    isListItem(lines[pos], list.ordered, contentStart);

    while (pos < (int)lines.size() && continuesList(lines[pos], list.ordered)) {
        list.children.push_back(parseListItem(lines, pos));
        if (pos + 1 < (int)lines.size() && isBlank(lines[pos]) && continuesList(lines[pos + 1], list.ordered)) {
            pos++;
        }
    }
    return list;
}

Node parseParagraph(vector<string>& lines, int& pos) {
    Node para = makeNode(Paragraph, trim(lines[pos]), pos + 1);
    pos++;
    while (pos < (int)lines.size() && !startsNewBlock(lines[pos])) {
        para.text += " " + trim(lines[pos]);
        pos++;
    }
    return para;
}

Node parseBlocks(vector<string>& lines, vector<Warning>& warnings) {
    Node doc = makeNode(Document, "", 1);
    int pos = 0;

    while (pos < (int)lines.size()) {
        string line = lines[pos];
        if (isBlank(line)) {
            pos++;
        } else if (headingLevel(line) > 0) {
            doc.children.push_back(parseHeading(lines, pos));
        } else if (isFence(line)) {
            doc.children.push_back(parseCodeBlock(lines, pos, warnings));
        } else if (startsListItem(line)) {
            doc.children.push_back(parseList(lines, pos));
        } else {
            doc.children.push_back(parseParagraph(lines, pos));
        }
    }
    return doc;
}
