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

int get_longest_palindrome_suffix(std::string text)
{
    int n = text.size();
    char char_that_is_not_in_the_string = '@';
    std::string reversed = text;
    std::reverse(reversed.begin(), reversed.end());
    text = reversed + char_that_is_not_in_the_string + text;
    return n - compute_lps_table(text).back();
}

std::string min_str_to_append(const std::string& text)
{
    int length = get_longest_palindrome_suffix(text);
    std::string result = text.substr(0, length);
    std::reverse(result.begin(), result.end());
    return result;
}

void test(const std::string& text)
{
    auto result = min_str_to_append(text);
    std::cout << "Text: " << text << std::endl;
    std::cout << "Characters to append to be a palindrome: " << result << std::endl;
    std::cout << "Result: " << text << result << std::endl;
    std::cout << std::endl << std::endl;
}

int main()
{
    test("aaaaa");
    test("abcba");
    test("accxeaba");
}