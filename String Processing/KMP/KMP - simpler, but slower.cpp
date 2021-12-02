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

std::vector<int> get_matches(const std::string &text, const std::string &pattern)
{
    if (pattern.empty() || text.empty()) {
        return {};
    }

    char char_that_is_not_in_the_string = '@';
    std::string combined = pattern + char_that_is_not_in_the_string + text;

    std::vector<int> result;
    std::vector<int> table = compute_lps_table(combined);

    int text_offset = pattern.size() + 1;
    for (int i = text_offset; i < table.size(); i++)
    {
        if (table[i] == pattern.size()) {
            // int start_index = i - (text_offset + (pattern.size() - 1));
            int start_index = i - 2 * pattern.size();
            result.push_back(start_index);
        }
    }

    return result;
}

void test(const std::string& text, const std::string& pattern)
{
    std::cout << "Text: " <<  text << std::endl;
    std::cout << "Pattern: " << pattern << std::endl;
    std::cout << "Matches start at: ";
    for (int i : get_matches(text, pattern)) {
        std::cout << i << ' ';
    }
    std::cout << std::endl << std::endl;
}

int main()
{
    test("aaaa", "a");
    test("ababababa", "aba");
    test("ababdabacdababcababcabab", "ababcabab");
    test("aaaabaaaab", "aaaac");
    test("ababab", "abababa");
    test("ababab", "");
    test("", "abababa");
}