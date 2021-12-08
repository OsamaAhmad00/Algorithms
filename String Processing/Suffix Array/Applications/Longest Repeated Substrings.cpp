#include <iostream>
#include <vector>

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
    std::vector<int> old_orders;
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

    void set_new_orders()
    {
        shifts[0].rank = 0;
        for (int i = 1; i < n; i++)
        {
            shifts[i].rank = shifts[i-1].rank;

            int curr = shifts[i  ].start_index;
            int prev = shifts[i-1].start_index;

            bool different_first_half = old_orders[curr] != old_orders[prev];
            bool different_second_half = false;

            if (!different_first_half) {
                int curr_shifted = shifted_index(curr, -1);
                int prev_shifted = shifted_index(prev, -1);
                int a = old_orders[curr_shifted];
                int b = old_orders[prev_shifted];
                different_second_half = (a != b);
            }

            if (different_first_half || different_second_half) {
                shifts[i].rank++;
            }
        }
    }

    void set_old_orders()
    {
        for (CyclicShift& shift : shifts)
            old_orders[shift.start_index] = shift.rank;
    }

    void shift()
    {
        for (CyclicShift& shift: shifts) {
            shift.start_index = shifted_index(shift.start_index);
            shift.rank = old_orders[shift.start_index];
        }

        shifts = stable_sort(shifts);

        set_new_orders();
    }

    void double_shift_length()
    {
        set_old_orders();
        shift();
        current_shift_length *= 2;
    }

    void compute_suffix_array(const std::string& string)
    {
        initialize_shifts(string);

        while (current_shift_length < n)
            double_shift_length();

        for (int i = 1; i < n; i++)
            suffix_array[i - 1] = shifts[i].start_index;
    }

public:

    explicit SuffixArray(const std::string& string) : n(string.size() + 1),
                                                      current_shift_length(1), shifts(n), old_orders(n), suffix_array(n - 1)
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

struct LongestRepeatedSubstringsInfo
{
    int length;
    std::vector<std::vector<int>> start_indices;
};

LongestRepeatedSubstringsInfo get_longest_repeated_substrings_info( const std::vector<int>& LCP_array,
                                                                    const std::vector<int>& suffix_array )
{
    // https://www.youtube.com/watch?v=OptoHwC3D-Y

    LongestRepeatedSubstringsInfo result;

    result.length = 0;
    for (int lcp : LCP_array)
        result.length = std::max(result.length, lcp);

    if (result.length != 0) {
        for (int i = 0; i < LCP_array.size(); i++)
        {
            if (LCP_array[i] == result.length)
            {
                bool is_new_substring = (i == 0 || LCP_array[i-1] != LCP_array[i]);

                if (is_new_substring) {
                    result.start_indices.emplace_back();
                    result.start_indices.back().push_back(suffix_array[i]);
                }

                result.start_indices.back().push_back(suffix_array[i+1]);
            }
        }
    }

    return result;
}

std::vector<std::string> get_longest_repeated_substrings( const std::vector<int>& LCP_array,
                                                          const std::vector<int>& suffix_array,
                                                          const std::string& string )
{
    std::vector<std::string> result;

    auto info = get_longest_repeated_substrings_info(LCP_array, suffix_array);

    for (auto& i : info.start_indices) {
        result.push_back(string.substr(i.front(), info.length));
    }

    return result;
}

void test(const std::string& string)
{
    std::vector<int> suffix_array = SuffixArray(string).get_suffix_array();
    std::vector<int> LCP_array = LCP(string, suffix_array).get_LCP();
    auto substrings =  get_longest_repeated_substrings(LCP_array, suffix_array, string);
    auto info = get_longest_repeated_substrings_info(LCP_array, suffix_array);

    std::cout << "String: " << string << std::endl;
    std::cout << "Longest Repeated Substrings:" << std::endl;
    for (int i = 0; i < substrings.size(); i++) {
        std::cout << "  - " << substrings[i];
        std::cout << "  ( starts at ";
        for (int index : info.start_indices[i])
            std::cout << index << ' ';
        std::cout << ")" << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test("ABRACADABRA");
    test("ABCXABCYABC");
    test("ABABBAABAA");
    test("AAAAA");
    test("AZAZA");
    test("ABCDE");
}