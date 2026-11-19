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
string outputPath = "";
bool showTokens = false;
bool showAst = false;

void printUsage() {
    cerr << "usage: mdc input.md [-o out] [--tokens] [--ast]\n";
    cerr << "  writes input.html next to input.md\n";
}

bool readArguments(int argc, char* argv[], string& error) {
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--tokens") {
            showTokens = true;
        } else if (arg == "--ast") {
            showAst = true;
        } else if (arg == "-o") {
            if (i + 1 >= argc || string(argv[i + 1]) == "") {
                error = "-o needs a file name";
                return false;
            }
            i++;
            outputPath = argv[i];
        } else if (arg[0] == '-') {
            error = "unknown option '" + arg + "'";
            return false;
        } else if (inputPath != "") {
            error = "only one input file is allowed";
            return false;
        } else {
            inputPath = arg;
        }
    }
    if (inputPath == "") {
        error = "no input file";
        return false;
    }
    return true;
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
    string error;
    if (!readArguments(argc, argv, error)) {
        cerr << "mdc: error: " << error << "\n";
        printUsage();
        return 2;
    }

    vector<string> lines;
    if (!readLines(inputPath, lines) || lines.size() == 0) {
        cerr << "mdc: error: cannot read '" << inputPath << "' (missing, empty or a folder)\n";
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

    bool dumping = showTokens || showAst;
    if (dumping && outputPath == "") return 0;

    string html = generateHtmlPage(doc);
    if (outputPath == "") outputPath = htmlFileName(inputPath);
    if (!writeFile(outputPath, html)) {
        cerr << "mdc: error: cannot write '" << outputPath << "'\n";
        return 1;
    }
    if (!dumping) cout << "Wrote " << outputPath << "\n";
    return 0;
}
