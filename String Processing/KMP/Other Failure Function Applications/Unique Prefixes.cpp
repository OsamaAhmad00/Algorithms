#include <iostream>
#include <vector>

std::vector<int> compute_lps_table(const std::string &pattern)
{
    std::vector<int> table(pattern.size(), 0);

    for (int i = 0, j = 1; j < pattern.size(); j++)
    {
        while (i > 0 && pattern[i] != pattern[j])
            i = table[i - 1];

        if (pattern[i] == pattern[j])
            table[j] = ++i;

        // else table[j] = 0
    }

    return table;
}

std::vector<bool> get_unique_prefixes(const std::string &text)
{
    // prefixes that appear in the
    // text only once (as a prefix).
    std::vector<int> table = compute_lps_table(text);
    std::vector<bool> is_unique(text.size(), true);
    for (int i : table) {
        if (i > 0) {
            is_unique[i - 1] = false;
        }
    }
    return is_unique;
}

void test(const std::string &text)
{
    std::vector<bool> is_unique = get_unique_prefixes(text);
    std::cout << "Text: " << text << std::endl;
    for (int i = 0; i < is_unique.size(); i++) {
        std::cout << text.substr(0, i + 1) << ": ";
        std::cout << ((is_unique[i]) ? "Unique" : "Not Unique") << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test("aaaa");
    test("abcde");
    test("ababababa");
    test("ababab");
}