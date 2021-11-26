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

std::vector<int> get_prefixes_frequencies(const std::string &text, const std::string &prefixes)
{
    char char_that_is_not_in_the_string = '@';
    std::string combined = prefixes + char_that_is_not_in_the_string + text;

    std::vector<int> table = compute_lps_table(combined);
    std::vector<int> frequencies(prefixes.size() + 1, 0);

    for (int i = prefixes.size() + 1; i < combined.size(); i++) {
        frequencies[table[i]]++;
    }

    for (int i = prefixes.size(); i > 0; i--) {
        frequencies[table[i - 1]] += frequencies[i];
    }

    frequencies.erase(frequencies.begin());

    return frequencies;
}

void test(const std::string &text, const std::string &prefixes)
{
    std::vector<int> frequencies = get_prefixes_frequencies(text, prefixes);
    std::cout << "Text: " << text << std::endl;
    for (int i = 0; i < prefixes.size(); i++) {
        std::cout << "Frequency of " << prefixes.substr(0, i+1) << ": " << frequencies[i] << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test("aabaaab", "aab");
    test("abcabc", "abcabc");
    test("abab", "abab");
    test("abxyzaab", "abab");
}