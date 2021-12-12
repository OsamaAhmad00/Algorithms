#include <iostream>
#include <vector>
#include <set>

struct CyclicShift
{
    int start_index;
    int rank;

    // Comparisons and indexing are done
    //  by the rank of the cyclic shift.
    operator int() const { return rank; }
};

std::vector<CyclicShift> stable_sort(const std::vector<CyclicShift> &shifts)
{
    // Count Sort - O(N + M) where N is the size of
    // the string and M is the size of count array.

    int max_val = 0;
    for (const CyclicShift& shift : shifts)
        max_val = std::max(max_val, shift.rank);

    std::vector<int> count(max_val + 1);

    for (const CyclicShift& shift : shifts)
        count[shift.rank]++;

    for (int i = 1; i < count.size(); i++)
        count[i] += count[i-1];

    std::vector<CyclicShift> result(shifts.size());
    for (int i = shifts.size() - 1; i >= 0; i--) {
        int new_index = --count[shifts[i].rank];
        result[new_index] = shifts[i];
    }

    return result;
}

class SuffixArray
{

    // Suffix Array is a compressed form of "Suffix Tree",
    // which in turn is a compressed form of "Suffix Trie".

    int n;
    int current_shift_length;
    std::vector<int> old_ranks;
    std::vector<CyclicShift> shifts;
    std::vector<int> suffix_array;

    void initialize_shifts(const std::string& string)
    {
        shifts.back() = {n - 1, 0};

        for (int i = 0; i < n - 1; i++) {
            shifts[i].start_index = i;
            shifts[i].rank = string[i];
        }

        shifts = stable_sort(shifts);

        shifts[0].rank = 0;
        for (int i = 1; i < n; i++) {
            int curr = shifts[i  ].start_index;
            int prev = shifts[i-1].start_index;
            bool not_equal = string[curr] != string[prev];
            shifts[i].rank = shifts[i-1].rank + not_equal;
        }
    }

    int shifted_index(int index, int steps = 1) const {
        return (index - current_shift_length * steps + n) % n;
    }

    void set_new_ranks()
    {
        shifts[0].rank = 0;
        for (int i = 1; i < n; i++)
        {
            shifts[i].rank = shifts[i-1].rank;

            int curr = shifts[i  ].start_index;
            int prev = shifts[i-1].start_index;

            bool different_first_half = old_ranks[curr] != old_ranks[prev];
            bool different_second_half = false;

            if (!different_first_half) {
                int curr_shifted = shifted_index(curr, -1);
                int prev_shifted = shifted_index(prev, -1);
                int a = old_ranks[curr_shifted];
                int b = old_ranks[prev_shifted];
                different_second_half = (a != b);
            }

            if (different_first_half || different_second_half) {
                shifts[i].rank++;
            }
        }
    }

    void set_old_ranks()
    {
        for (CyclicShift& shift : shifts)
            old_ranks[shift.start_index] = shift.rank;
    }

    void shift()
    {
        for (CyclicShift& shift: shifts) {
            shift.start_index = shifted_index(shift.start_index);
            shift.rank = old_ranks[shift.start_index];
        }

        shifts = stable_sort(shifts);

        set_new_ranks();
    }

    void double_shift_length()
    {
        set_old_ranks();
        shift();
        current_shift_length *= 2;
    }

    bool is_done()
    {
        return shifts.back().rank == (n - 1);
    }

    void compute_suffix_array(const std::string& string)
    {
        initialize_shifts(string);

        while (!is_done())
            double_shift_length();

        for (int i = 1; i < n; i++)
            suffix_array[i - 1] = shifts[i].start_index;
    }

public:

    explicit SuffixArray(const std::string& string) : n(string.size() + 1),
        current_shift_length(1), shifts(n), old_ranks(n), suffix_array(n - 1)
    {
        compute_suffix_array(string);
    }

    const std::vector<int>& get_suffix_array() { return suffix_array; }
};

class LCP
{
    std::vector<int> ranks;
    std::vector<int> LCP_array;

    // These are just references, not going to be
    // used, and not guaranteed to be valid references
    // once the computation of the suffix array is done.
    const std::string& string;
    const std::vector<int>& suffix_array;

    void set_ranks()
    {
        // ranks[i] = rank of suffix that starts
        //  at index i in the original string.
        // rank = the position of the suffix in
        //  the suffix array.
        for (int rank = 0; rank < suffix_array.size(); rank++) {
            int start_index = suffix_array[rank];
            ranks[start_index] = rank;
        }
    }

    bool has_preceding_suffix(int i) const {
        // has preceding suffix in the suffix array.
        return ranks[i] > 0;
    }

    void compute_LCP()
    {
        // Kasai's algorithm
        // Great explanation: https://stackoverflow.com/a/63104083/9140652
        // O(n). If the computation of the suffix array takes O(n * log(n))
        //  the overall complexity will be O(n * log(n)).
        // Why is this linear and not quadratic (since we have two nested loops)?
        //  - matched_characters increases as long as the inner loop is running.
        //  - matched_characters can't exceed string.size().
        //  - on each iteration, matched_characters decreases by 1 at most.
        //  Combining these 3 points, we see that these two nested loops are O(n).

        set_ranks();

        int matched_characters = 0;
        for (int start_index = 0; start_index < string.size(); start_index++) {
            if (has_preceding_suffix(start_index))
            {
                // index in the suffix array of
                // the suffix that starts at start_index
                int rank = ranks[start_index];

                int i = suffix_array[rank    ] + matched_characters;
                int j = suffix_array[rank - 1] + matched_characters;
                while (i < string.size() && j < string.size() && string[i++] == string[j++])
                    matched_characters++;

                LCP_array[rank - 1] = matched_characters;

                if (matched_characters > 0)
                    matched_characters--;
            }
        }
    }

public:

    // LCP_array[i] = the longest common prefix
    // of suffix_array[i] and suffix_array[i + 1]

    LCP(const std::string& string, const std::vector<int>& suffix_array)
            : ranks(string.size()), LCP_array(string.size() - 1), string(string), suffix_array(suffix_array)
    {
        compute_LCP();
    }

    const std::vector<int>& get_LCP() const { return LCP_array; }
};

class CommonSubstrings
{
    int strings_count;

    // Sorted array containing the start of each
    //  string in the combined string. Used to
    //  determine the color of each suffix.
    std::vector<int> start_indices;

    std::string combined;
    std::vector<int> suffix_array;
    std::vector<int> LCP_array;

    int unique_colors_count;
    std::multiset<int> current_LCPs;
    // Count of each color in side the sliding window.
    std::vector<int> colors_count;

    // Pointers of the sliding window.
    int start;
    int end;

    int max_substring_len;
    int current_substring_len;

    bool new_max;
    bool enough_colors;
    bool enough_chars;

    std::set<std::string> result;

    int get_color(int pointer)
    {
        // Colors of strings are represented by numbers starting from 0.
        // For a string s, if its start index = start_indices[i], then
        //  its color is i.
        int suffix_index = suffix_array[pointer];
        auto iterator = std::lower_bound(start_indices.begin(), start_indices.end(), suffix_index);
        int result = (iterator - start_indices.begin());
        result -= (iterator == start_indices.end() || *iterator > suffix_index);
        return result;
    }

    int get_start_index(int color) {
        return start_indices[color];
    }

    void init(const std::vector<std::string>& strings)
    {
        // Sentinel characters put between strings to avoid
        //  intermingling of the suffices when constructing
        //  the suffix array.
        // These sentinel characters should be unique and
        //  lexicographically smaller than all characters
        //  in the provided strings.
        // MAKE SURE NOT TO PASS TOO MANY STRINGS, OTHERWISE
        //  YOU'LL HAVE TO FIND ANOTHER WAY OF REPRESENTING
        //  SENTINEL CHARACTERS.
        char sentinel = 1;

        for (const std::string& string : strings)
        {
            // The start indices will be sorted.
            start_indices.push_back(combined.size());
            combined += string;
            combined += sentinel;
            sentinel++;
        }

        suffix_array = SuffixArray(combined).get_suffix_array();
        LCP_array = LCP(combined, suffix_array).get_LCP();

        min_matched_strings = 2;
    }

    void set_current_substring_len() {
        current_substring_len = *current_LCPs.begin();
    }

    void add_LCP(int index) {
        current_LCPs.insert(LCP_array[index]);
    }

    void erase_LCP(int index) {
        int lcp = LCP_array[index];
        current_LCPs.erase(current_LCPs.lower_bound(lcp));
    }

    void slide_start()
    {
        int color = get_color(start);
        colors_count[color]--;
        unique_colors_count -= (colors_count[color] == 0);

        erase_LCP(start++);
        set_current_substring_len();
    }

    void slide_end()
    {
        add_LCP(end++);
        set_current_substring_len();

        int color = get_color(end);
        colors_count[color]++;
        unique_colors_count += (colors_count[color] == 1);
    }

    void add_match()
    {
        int start_index = suffix_array[start];
        result.insert(combined.substr(start_index, max_substring_len));
    }

    void reset()
    {
        colors_count.clear();
        colors_count.resize(strings_count, 0);

        // First strings_count in the suffix array will
        //  be the suffices starting with the sentinel
        //  characters we added. Will skip them.
        start = end = strings_count;
        colors_count[get_color(start)]++;
        unique_colors_count = 1;

        max_substring_len = 1;

        enough_colors = false;
        new_max = false;
        enough_chars = false;
    }

    void set_booleans()
    {
        new_max = current_substring_len > max_substring_len;
        enough_colors = unique_colors_count >= min_matched_strings;
        enough_chars = current_substring_len >= max_substring_len;
    }

public:

    // You can tweak this parameter
    // to get different results.
    int min_matched_strings;

    CommonSubstrings(const std::vector<std::string>& strings)
        : strings_count(strings.size())
    {
        init(strings);
    }

    std::set<std::string> compute()
    {
        // https://www.youtube.com/watch?v=Ic80xQFWevc

        reset();

        int last_index = combined.size() - 1;

        while (start != last_index)
        {
            set_booleans();

            if (enough_colors && enough_chars)
            {
                if (new_max) {
                    result.clear();
                    max_substring_len = current_substring_len;
                }

                add_match();
            }

            bool shrink_window = (end == last_index) || enough_colors;

            shrink_window ? slide_start() : slide_end();
        }

        return result;
    }
};

void test(const std::vector<std::string>& strings, int min_strings)
{
    CommonSubstrings commonSubstrings(strings);
    commonSubstrings.min_matched_strings = min_strings;

    auto matches = commonSubstrings.compute();

    std::cout << "Strings:" << std::endl;
    for (int i = 0; i < strings.size(); i++)
        std::cout << "  " << i << " - " << strings[i] << std::endl;

    std::cout << "Common substrings (minimum strings to match = " << min_strings << "): " << std::endl;

    for (auto& match : matches)
            std::cout << "  - " << match << std::endl;

    std::cout << std::endl;
}

// TODO This file needs some more testing, refactoring, and documenting.
// TODO return the indices at which the matches start.

int main()
{
    test({"ababaaab", "aaabab", "baba"}, 2);
    test({"anananana", "anana", "bana"}, 3);
    test({"anaba", "anaxaba"}, 2);
    test({"anananana", "bana"}, 2);
    test({"abc", "abc"}, 2);
    test({"abc", "xyz"}, 2);
}