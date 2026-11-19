#pragma once

#include "ast.h"

#include <string>

using namespace std;

string escapeHtml(string text);

string generateHtml(Node& node);

string generateHtmlPage(Node& doc);
