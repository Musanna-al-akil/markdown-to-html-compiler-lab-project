#pragma once

#include "ast.h"

#include <string>
#include <vector>

using namespace std;

Node parseBlocks(vector<string>& lines, vector<Warning>& warnings);
