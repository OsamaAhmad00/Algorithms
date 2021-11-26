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

std::vector<int> get_prefixes_frequencies_slow(const std::string &text)
{
    // O(n^2)
    // A little more intuitive
    // Intuition:
    // Consider the string aabaaab:
    // the table will be {0, 1, 0, 1, 2, 2, 3}
    // The number 2 occurring twice in the table means that
    // there are two prefixes that has the first two letters
    // as a suffix. This means that the prefix "aa" is showing
    // as a suffix in two prefixes. The number of occurrences
    // of "aa" = 1 + the number of it being a suffix = 1 + 2 = 3.
    // We add 1 because there is one occurrence that is not
    // counted, the occurrence of it as a prefix.
    // But what about the prefix "a"? It has occurred 5 times,
    // but using this way, it'll be considered as occurring only
    // 1 + 2 times = 3 times. The thing about this table is that
    // it contains the "longest" common proper-prefix and suffix.
    // to get all occurrences of "a", we have to also count the
    // times where it occurres in the other prefixes. When we
    // fail at index 4, we go back to the index table[4 - 1] = 1.
    // This means that we also have to add the two times where
    // the prefix "aa" has occurred.

    std::vector<int> frequencies(text.size(), 0);
    std::vector<int> table = compute_lps_table(text);

    for (int i = 0; i < text.size(); i++) {
        int k = table[i];
        while (k > 0) {
            frequencies[k - 1]++;
            k = table[k - 1];
        }
    }

    for (int& i : frequencies) i++;

    return frequencies;
}

std::vector<int> get_prefixes_frequencies(const std::string &text)
{
    // O(n)
    // Optimized version
    std::vector<int> frequencies(text.size(), 0);
    std::vector<int> table = compute_lps_table(text);

    for (int i = 0; i < text.size(); i++) {
        frequencies[table[i]]++;
    }

    for (int i = text.size() - 1; i > 0; i--) {
        frequencies[table[i - 1]] += frequencies[i];
    }

    for (int& i : frequencies) i++;

    frequencies.erase(frequencies.begin());
    frequencies.push_back(1);

    return frequencies;
}

void test(const std::string &text)
{
    std::vector<int> frequencies = get_prefixes_frequencies(text);
    std::cout << "Text: " << text << std::endl;
    for (int i = 0; i < text.size(); i++) {
        std::cout << "Frequency of " << text.substr(0, i+1) << ": " << frequencies[i] << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test("aabaaab");
    test("abcabc");
}