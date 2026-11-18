#include "ast.h"
#include "block_parser.h"
#include "html_gen.h"
#include "inline_parser.h"
#include "semantic.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

string inputPath = "";
bool showTokens = false;
bool showAst = false;

void printUsage() {
    cerr << "usage: mdc input.md [--tokens] [--ast]\n";
}

bool readArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--tokens") {
            showTokens = true;
        } else if (arg == "--ast") {
            showAst = true;
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

bool byLine(Warning a, Warning b) {
    return a.line < b.line;
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
    parseAllInlines(doc);
    analyze(doc, warnings);

    stable_sort(warnings.begin(), warnings.end(), byLine);
    for (int i = 0; i < (int)warnings.size(); i++) {
        cerr << inputPath << ":" << warnings[i].line << ": warning: " << warnings[i].message << "\n";
    }
    if (showAst) cout << dumpAst(doc, 0);

    if (showTokens || showAst) return 0;

    string html = generateHtml(doc);
    string outputPath = htmlFileName(inputPath);
    if (!writeFile(outputPath, html)) {
        cerr << "mdc: error: cannot write '" << outputPath << "'\n";
        return 1;
    }
    cout << "Wrote " << outputPath << "\n";
    return 0;
}
