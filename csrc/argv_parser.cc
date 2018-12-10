#include "common.h"
#include "argv_parser.h"


char *string2chars(const string &str) {
    int len = str.length();
    auto *chars = new char[len + 1];
    for (int i = 0; i < str.length(); i++) {
        chars[i] = str[i];
    }
    chars[len] = 0;
    return chars;
}

string ensure_one_newline(const string &s) {
    string str(s);
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    return str + "\n";
}

args_t parse(const string &s) {
    list<string> argv;
    ostringstream token;

    bool in_token;
    bool in_container;
    bool escaped;
    char container_start;
    char c;
    int len;
    int i;

    string str = ensure_one_newline(s);

    container_start = 0;
    in_token = false;
    in_container = false;
    escaped = false;

    len = static_cast<int>(str.length());

    for (i = 0; i < len; i++) {
        c = str[i];

        switch (c) {
            /* handle whitespace */
            case ' ':
            case '\t':
            case '\n':
                if (!in_token)
                    continue;

                if (in_container) {
                    token << c;
                    continue;
                }

                if (escaped) {
                    escaped = false;
                    token << c;
                    continue;
                }

                /* if reached here, we're at end of token */
                in_token = false;
                argv.push_back(token.str());
                token.str(string());
                token.clear();
                break;

                /* handle quotes */
            case '\'':
            case '\"':

                if (escaped) {
                    token << c;
                    escaped = false;
                    continue;
                }

                if (!in_token) {
                    in_token = true;
                    in_container = true;
                    container_start = c;
                    continue;
                }

                if (in_container) {
                    if (c == container_start) {
                        in_container = false;
                        in_token = false;
                        argv.push_back(token.str());
                        token.str(string());
                        token.clear();
                        continue;
                    } else {
                        token << c;
                        continue;
                    }
                }

                /* XXX in this case, we:
                 *    1. have a quote
                 *    2. are in a token
                 *    3. and not in a container
                 * e.g.
                 *    hell"o
                 *
                 * what'str done here appears shell-dependent,
                 * but overall, it'str an error.... i *think*
                 */
                throw runtime_error("Parse Error! Bad quotes");
            case '\\':

                if (in_container && str[i + 1] != container_start) {
                    token << c;
                    continue;
                }

                if (escaped) {
                    token << c;
                    continue;
                }

                escaped = true;
                break;

            default:
                if (!in_token) {
                    in_token = true;
                }

                token << c;
        }
    }

    if (in_container)
        throw runtime_error("Parse Error! Still in container\n");

    if (escaped)
        throw runtime_error("Parse Error! Unused escape (\\)\n");

    return args_t(argv);
}
