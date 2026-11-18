#pragma once

#include "ast.h"

#include <string>
#include <vector>

using namespace std;

string slugify(string text);

void analyze(Node& doc, vector<Warning>& warnings);
