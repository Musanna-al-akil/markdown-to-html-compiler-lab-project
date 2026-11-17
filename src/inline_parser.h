#pragma once

#include "ast.h"
#include "inline_lexer.h"

#include <string>
#include <vector>

using namespace std;

vector<Node> parseInlineTokens(vector<Token>& tokenList, int line);

void parseAllInlines(Node& node);

string dumpAllTokens(Node& node);
