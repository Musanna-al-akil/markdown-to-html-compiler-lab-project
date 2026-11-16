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

int headingLevel(string line) {
    int hashes = 0;
    while (hashes < (int)line.size() && line[hashes] == '#') hashes++;
    if (hashes == 0 || hashes > 6) return 0;
    if (hashes < (int)line.size() && line[hashes] != ' ') return 0;
    return hashes;
}

bool startsNewBlock(string line) {
    return isBlank(line) || headingLevel(line) > 0;
}

Node parseHeading(vector<string>& lines, int& pos) {
    string line = lines[pos];
    int level = headingLevel(line);
    Node heading = makeNode(Heading, trim(line.substr(level)), pos + 1);
    heading.level = level;
    pos++;
    return heading;
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

Node parseBlocks(vector<string>& lines) {
    Node doc = makeNode(Document, "", 1);
    int pos = 0;

    while (pos < (int)lines.size()) {
        string line = lines[pos];
        if (isBlank(line)) {
            pos++;
        } else if (headingLevel(line) > 0) {
            doc.children.push_back(parseHeading(lines, pos));
        } else {
            doc.children.push_back(parseParagraph(lines, pos));
        }
    }
    return doc;
}
