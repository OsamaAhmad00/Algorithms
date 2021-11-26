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

std::string get_smallest_repeated_string(const std::string& text)
{
    // Consider the string abcdabcdabcd:
    // The table would be {0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8}
    // The last number, 8, means that the first 8 characters
    //  equal the last 8 characters.
    // Let's divide the above string into 3 blocks, each of size 4:
    // A = first 4 characters, B = second 4 characters, c = last 4 characters.
    // Saying that the first 8 characters = the last 8 characters mean
    //  that AB = BC, which means that A = B = C.
    // The size of each block will be text.size() - last number of the table.
    // If the length of the block doesn't divide the length of the string,
    //  then it's obvious that the result is wrong.
    int length = text.size() - compute_lps_table(text).back();
    if (text.size() % length != 0) return "";
    return text.substr(0, length);
}

void test(const std::string& text)
{
    std::string result = get_smallest_repeated_string(text);
    std::cout << "Text: " << text << std::endl;
    std::cout << "Smallest Repeated String: " << result << std::endl;
    std::cout << std::endl;
}

int main()
{
    test("aaaa");
    test("ababababa");
    test("ababdabacdababcababcabab");
    test("aaaabaaaab");
    test("ababab");
}