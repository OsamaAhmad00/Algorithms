#include <iostream>
#include <vector>
#include <algorithm>

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

int get_longest_palindrome_prefix(std::string text)
{
    char char_that_is_not_in_the_string = '@';
    std::string reversed = text;
    std::reverse(reversed.begin(), reversed.end());
    text = text + char_that_is_not_in_the_string + reversed;
    return compute_lps_table(text).back();
}

int get_longest_palindrome_suffix(std::string text)
{
    int n = text.size();
    char char_that_is_not_in_the_string = '@';
    std::string reversed = text;
    std::reverse(reversed.begin(), reversed.end());
    text = reversed + char_that_is_not_in_the_string + text;
    return n - compute_lps_table(text).back();
}

void test(const std::string& text)
{
    int length = get_longest_palindrome_prefix(text);
    std::cout << "Text: " << text << std::endl;
    std::cout << "Longest Palindrome Prefix: " << text.substr(0, length);
    std::cout << std::endl << std::endl;
}

int main()
{
    test("aaaaa");
    test("abcba");
    test("abaccxe");
}