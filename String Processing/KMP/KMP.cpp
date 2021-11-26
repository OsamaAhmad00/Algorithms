#include <iostream>
#include <vector>

std::vector<int> compute_lps_table(const std::string &pattern)
{
    // Also known as "Partial Match Table" or "Failure Function".

    // This is very similar to matching strings
    // with patterns, but simpler.

    // table[i] = longest-prefix-suffix for
    // the substring pattern[0..i] (i inclusive)
    // Being more precise, it's the longest common
    // "proper prefix" and suffix.
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
    // Consider this case:
    // text = aaaabaaaab, pattern = aaaac
    // When i = 4, and j = 4, the algorithm will
    // try to go back to the last match, and j
    // will be 3, then 2, then 1, then 0. Again,
    // when i = 9, and j = 4, j will be 3, then
    // 2, then 1, then 0. This means that it goes
    // back for m times (where m = |pattern|).
    // You might think that going back will make
    // the runtime complexity O(n * m) since it
    // goes back for m times in some cases, but
    // you also have to realize that this happens
    // at most n / m times. This makes the order
    // of this function = O(n + n/m * m) = O(n).
    // The computation of the table takes O(m),
    // so, the overall complexity = O(n + m).

    std::vector<int> result;
    std::vector<int> table = compute_lps_table(pattern);

    int pattern_last_index = (int)pattern.size() - 1;

    for (int i = 0, j = 0; i < text.size(); i++)
    {
        // If j = 0, we can't go back in the pattern
        //  anymore, which means that text[i] can't
        //  be matched, in this case, ignore text[i]
        //  and try matching starting from text[i + 1].
        //  But if you still can go back in the pattern,
        //  go back and try matching again with text[i].
        // We set j = table[j - 1] instead of table[j]
        //  because we know that the pattern matched
        //  up until the (j-1)th character, jth character
        //  is the character we were trying to match with
        //  currently but failed.
        while (j > 0 && text[i] != pattern[j])
            j = table[j - 1];

        if (text[i] == pattern[j])
        {
            if (j == pattern_last_index) {
                int start_index = i - pattern_last_index;
                result.push_back(start_index);

                // To find the next match
                j = table[pattern_last_index];
            } else j++;
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