#include "semantic.h"

#include <cctype>
#include <set>

using namespace std;

string slugify(string text) {
    string slug = "";
    for (int i = 0; i < (int)text.size(); i++) {
        char c = text[i];
        if (c >= 'A' && c <= 'Z') c = tolower(c);
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
            slug += c;
        } else if (c == ' ' || c == '-' || c == '_') {
            if (slug.size() > 0 && slug[slug.size() - 1] != '-') slug += '-';
        }
    }
    while (slug.size() > 0 && slug[slug.size() - 1] == '-') slug.erase(slug.size() - 1);
    if (slug == "") return "section";
    return slug;
}

void assignIds(Node& node, set<string>& ids, vector<Warning>& warnings) {
    if (node.type == Heading) {
        string slug = slugify(plainText(node));
        string id = slug;
        int k = 1;
        while (ids.count(id) > 0) {
            id = slug + "-" + to_string(k);
            k++;
        }
        node.attr = id;
        ids.insert(id);

        if (id != slug) {
            addWarning(warnings, node.line, "heading id '" + slug + "' is already used; '" +
                                                plainText(node) + "' gets id '" + id + "'");
        }
    }
    for (int i = 0; i < (int)node.children.size(); i++) {
        assignIds(node.children[i], ids, warnings);
    }
}

void checkLinks(Node& node, set<string>& ids, vector<Warning>& warnings) {
    if (node.type == Link && node.attr.size() > 0 && node.attr[0] == '#') {
        string target = node.attr.substr(1);
        if (ids.count(target) == 0) {
            addWarning(warnings, node.line, "broken link '" + node.attr + "': no heading has that id");
        }
    }
    for (int i = 0; i < (int)node.children.size(); i++) {
        checkLinks(node.children[i], ids, warnings);
    }
}

void analyze(Node& doc, vector<Warning>& warnings) {
    set<string> ids;
    assignIds(doc, ids, warnings);
    checkLinks(doc, ids, warnings);
}
