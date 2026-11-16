#include "ast.h"
#include "block_parser.h"
#include "html_gen.h"
#include "inline_lexer.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

string inputPath = "";
bool showTokens = false;

void printUsage() {
    cerr << "usage: mdc input.md [--tokens]\n";
}

bool readArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--tokens") {
            showTokens = true;
        } else {
            inputPath = arg;
        }
    }
    return inputPath != "";
}

bool endsWith(string text, string ending) {
    if (text.size() < ending.size()) return false;
    return text.substr(text.size() - ending.size()) == ending;
}

string htmlFileName(string path) {
    if (endsWith(path, ".md")) path = path.substr(0, path.size() - 3);
    return path + ".html";
}

bool readLines(string path, vector<string>& lines) {
    ifstream in(path);
    if (!in) return false;
    string line;
    while (getline(in, line)) {
        lines.push_back(line);
    }
    return true;
}

bool writeFile(string path, string text) {
    ofstream out(path);
    if (!out) return false;
    out << text;
    return !out.fail();
}

string dumpAllTokens(Node& node) {
    if (hasInlineText(node)) {
        vector<Token> tokenList = lexInline(node.text);
        string out = nodeTypeName(node.type) + " (line " + to_string(node.line) + "): " + node.text + "\n";
        return out + dumpTokens(tokenList);
    }
    string out = "";
    for (int i = 0; i < (int)node.children.size(); i++) out += dumpAllTokens(node.children[i]);
    return out;
}

int main(int argc, char* argv[]) {
    if (!readArguments(argc, argv)) {
        printUsage();
        return 2;
    }

    vector<string> lines;
    if (!readLines(inputPath, lines)) {
        cerr << "mdc: error: cannot read '" << inputPath << "'\n";
        return 1;
    }

    vector<Warning> warnings;
    Node doc = parseBlocks(lines, warnings);
    if (showTokens) cout << dumpAllTokens(doc);

    for (int i = 0; i < (int)warnings.size(); i++) {
        cerr << inputPath << ":" << warnings[i].line << ": warning: " << warnings[i].message << "\n";
    }
    if (showTokens) return 0;

    string html = generateHtml(doc);
    string outputPath = htmlFileName(inputPath);
    if (!writeFile(outputPath, html)) {
        cerr << "mdc: error: cannot write '" << outputPath << "'\n";
        return 1;
    }
    cout << "Wrote " << outputPath << "\n";
    return 0;
}
