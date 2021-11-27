#include <iostream>
#include <vector>

std::vector<int> compute_lps_table(const std::string &pattern, int start = 0)
{
    int n = pattern.size() - start;
    std::vector<int> table(n, 0);

    for (int i = 0, j = 1; j < n; j++)
    {
        while (i > 0 && pattern[start + i] != pattern[start + j])
            i = table[i - 1];

        if (pattern[start + i] == pattern[start + j])
            table[j] = ++i;

        // else table[j] = 0
    }

    return table;
}

std::vector<bool> get_unique_prefixes(const std::string &text, int start = 0)
{
    // prefixes that appear in the
    // text only once (as a prefix).
    std::vector<int> table = compute_lps_table(text, start);
    std::vector<bool> is_unique(table.size(), true);
    for (int i : table) {
        if (i > 0) {
            is_unique[i - 1] = false;
        }
    }
    return is_unique;
}

int unique_prefixes_count(const std::string& text, int start)
{
    int result = 0;
    std::vector<bool> is_unique = get_unique_prefixes(text, start);
    for (bool b : is_unique) {
        result += b;
    }
    return result;
}

int unique_substrings_count(const std::string& text)
{
    // O(n^2)
    // Sum the number of unique prefixes of each suffix.

    // Intuition:
    // Consider the string "abc":
    // First, we start by the suffix "c", that's a unique substring up
    // until now.
    // Next, we consider the suffix "bc", adding "b" to "c" will
    // introduce some more unique substrings.
    // The number of the new substrings that will be introduced is 1 for
    // the substring "b" itself (if it's unique) + the number of the unique
    // prefixes of the substring "c" that will be appended to the "b"
    // = 1 + 1 = 2. These 2 equals the number of unique prefixes of the
    // suffix "bc".
    // Lastly, lets consider the suffix "abc", adding "a" to "bc" will
    // introduce unique_prefixes_count("abc") new substrings.
    // The final result = 1 ("c") + 2 ("b", "bc") + 3 ("a", "ab", "abc") = 6.

    int result = 0;
    for (int i = 0; i < text.size(); i++) {
        result += unique_prefixes_count(text, i);
    }
    return result;
}

std::vector<std::string> get_unique_substrings(const std::string &text)
{
    // O(n^3)

    std::vector<std::string> result;
    for (int start = 0; start < text.size(); start++) {
        std::vector<bool> is_unique = get_unique_prefixes(text, start);
        for (int i = 0; i < is_unique.size(); i++) {
            if (is_unique[i]) {
                result.push_back(text.substr(start, i + 1));
            }
        }
    }
    return result;
}

void test(const std::string& text)
{
    std::cout << "Text: " <<  text << std::endl;
    std::cout << "Count of unique substrings: " << unique_substrings_count(text) << std::endl;
    std::cout << "Unique substrings: " << std::endl;
    for (auto& str : get_unique_substrings(text)) {
        std::cout << str << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test("abc");
    test("aaaaa");
    test("ababababa");
    test("abcdefghe");
    test("ababab");
    test("aabab");
}