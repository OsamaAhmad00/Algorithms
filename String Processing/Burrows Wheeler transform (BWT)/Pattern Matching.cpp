#include <iostream>
#include <vector>
#include <algorithm>

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

std::string get_BWT(const std::string& string, const std::vector<int>& suffix_array)
{
    int n = string.size();

    std::string result;
    result.resize(n);

    for (int i = 0; i < n; i++) {
        int index = (suffix_array[i] + n - 1) % n;
        result[i] = string[index];
    }

    return result;
}

class BurrowsWheelerMatcher
{
    int n;
    std::string transform;
    std::vector<int> suffix_array;

    // first_column_index[c][i] = the index of the ith appearance
    //  of the characters c in the first column.
    std::vector<std::vector<int>> first_column_index;

    // last_column_count[i] = the count of the appearances of the
    // ith character in the first column so far.
    std::vector<int> last_column_count;

    void init()
    {
        // This can be set as a constant.
        int max_char = 0;
        for (int i = 0; i < n; i++)
            max_char = std::max(max_char, (int)transform[i]);

        first_column_index.resize(max_char + 1);
        last_column_count.resize(n);

        std::vector<int> count(max_char + 1);

        for (int i = 0; i < n; i++)
            count[transform[i]]++;

        {
            int i = n - 1;
            int c = max_char;
            first_column_index[c].resize(count[c]);

            while (i >= 0)
            {
                if (count[c] == 0) {
                    c--;
                    first_column_index[c].resize(count[c]);
                    continue;
                }

                first_column_index[c][--count[c]] = i--;
            }
        }

        // By now, for any index i, count[i] = 0.
        for (int i = 0; i < n; i++) {
            char c = transform[i];
            last_column_count[i] = count[c]++;
        }
    }

    int get_first_column_index(int i)
    {
        char c = transform[i];
        int count = last_column_count[i];
        return first_column_index[c][count];
    }

public:

    explicit BurrowsWheelerMatcher(const std::string& string) : n(string.size())
    {
        suffix_array = SuffixArray(string).get_suffix_array();
        transform = get_BWT(string, suffix_array);
        init();
    }

    std::vector<int> get_matches(const std::string& pattern)
    {
        // Note: we can get the first column of the cyclic shifts
        //  by sorting the last column (the transform).
        // Consider the string "panamabananas$":
        // Sorted Rotations:
        // -----------------
        //  $panamabananas
        //  abananas$panam
        //  amabananas$pan
        //  anamabananas$p      < first half
        //  ananas$panamab
        //  anas$panamaban
        //  as$panamabanan
        // -----------------
        //  bananas$panama
        //  mabananas$pana
        //  namabananas$pa
        //  nanas$panamaba      < second half
        //  nas$panamabana
        //  panamabananas$
        //  s$panamabanana
        // -----------------
        // Notice that the first 'a' in the first column corresponds to
        //  the first 'a' in the last column. Also, the second 'a' in the
        //  first column corresponds to the second 'a' in the last column,
        //  also, the third and the fourth...
        // In general, for any character c, its ith appearance
        //  in the first column corresponds to its ith appearance
        //  in the last column.
        // Why is this true?
        //  Consider the cyclic shifts in the first half that start with 'a':
        //   abananas$panam
        //   amabananas$pan
        //   anamabananas$p
        //   ananas$panamab
        //   anas$panamaban
        //   as$panamabanan
        //  Notice that if we chop off the first character ('a'), they'll remain sorted:
        //   bananas$panam
        //   mabananas$pan
        //   namabananas$p
        //   nanas$panamab
        //   nas$panamaban
        //   s$panamabanan
        //  Also, if we add the chopped character at the end, they'll remain sorted:
        //   bananas$panama
        //   mabananas$pana
        //   namabananas$pa
        //   nanas$panamaba
        //   nas$panamabana
        //   s$panamabanana
        // Notice that we just did a cyclic shift!
        // This is the order in which these cyclic shifts appear in the second half.
        // This property of the cyclic shifts appearing in the same order is called
        //  the "First-Last Property".
        // Note that since the first column is sorted, for any pattern, if existed
        //  in the original string, all cyclic shifts that start with this pattern
        //  will be next to each other in the first column, nothing in between.
        // All we need to do now is to find the range of cyclic shifts that start
        //  with the pattern. We need to have two pointers to define the range
        //  containing all cyclic shifts starting with the given pattern.
        // Initially, the two pointers will point to the last column, one (will call
        //  it bottom) = 0, and the other (will call it top) = n - 1. Then for each
        //  character in the pattern, we'll start to narrow down the range, until
        //  we reach the desired range that contains only the matches.
        // Note that since the range is the smallest in the first column, and is
        //  the biggest in the last column (since it narrows down on each iteration),
        //  we need to start from the last column, and match the pattern backwards,
        //  and start narrowing down the range. We can't start from the first column.
        // Using the First-Last property, after finding the range containing the
        //  last character of the pattern, we'll determine what are the indices
        //  of the cyclic shifts that starts with the characters pointed to by the
        //  bottom and the top pointers (indices in the first column), then, we'll
        //  move again to the last column, and match the character before the
        //  last character in the pattern, then narrow down the matching range,
        //  then, again, move to the first column, and so on.
        // Example:
        // string = "panamabananas$", pattern = "ana":
        //
        //           $panamabananas < bottom
        //           abananas$panam
        //           amabananas$pan
        //           anamabananas$p
        //           ananas$panamab
        //           anas$panamaban
        //           as$panamabanan
        //           bananas$panama
        //           mabananas$pana
        //           namabananas$pa
        //           nanas$panamaba
        //           nas$panamabana
        //           panamabananas$
        //           s$panamabanana < top
        //
        //           $panamabananas
        //           abananas$panam
        //           amabananas$pan
        //           anamabananas$p
        //           ananas$panamab
        //           anas$panamaban
        //           as$panamabanan
        //           bananas$panama < bottom
        //           mabananas$pana
        //           namabananas$pa
        //           nanas$panamaba
        //           nas$panamabana
        //           panamabananas$
        //           s$panamabanana < top
        //
        //           $panamabananas
        //  bottom > abananas$panam
        //           amabananas$pan
        //           anamabananas$p
        //           ananas$panamab
        //           anas$panamaban
        //     top > as$panamabanan
        //           bananas$panama
        //           mabananas$pana
        //           namabananas$pa
        //           nanas$panamaba
        //           nas$panamabana
        //           panamabananas$
        //           s$panamabanana
        //
        //           $panamabananas
        //           abananas$panam < bottom
        //           amabananas$pan
        //           anamabananas$p
        //           ananas$panamab
        //           anas$panamaban
        //           as$panamabanan < top
        //           bananas$panama
        //           mabananas$pana
        //           namabananas$pa
        //           nanas$panamaba
        //           nas$panamabana
        //           panamabananas$
        //           s$panamabanana
        //
        //           $panamabananas
        //           abananas$panam
        //           amabananas$pan < bottom
        //           anamabananas$p
        //           ananas$panamab
        //           anas$panamaban
        //           as$panamabanan < top
        //           bananas$panama
        //           mabananas$pana
        //           namabananas$pa
        //           nanas$panamaba
        //           nas$panamabana
        //           panamabananas$
        //           s$panamabanana
        //
        //           $panamabananas
        //           abananas$panam
        //           amabananas$pan
        //           anamabananas$p
        //           ananas$panamab
        //           anas$panamaban
        //           as$panamabanan
        //           bananas$panama
        //           mabananas$pana
        //  bottom > namabananas$pa
        //           nanas$panamaba
        //     top > nas$panamabana
        //           panamabananas$
        //           s$panamabanana
        //
        //           $panamabananas
        //           abananas$panam
        //           amabananas$pan
        //           anamabananas$p
        //           ananas$panamab
        //           anas$panamaban
        //           as$panamabanan
        //           bananas$panama
        //           mabananas$pana
        //           namabananas$pa < bottom
        //           nanas$panamaba
        //           nas$panamabana < top
        //           panamabananas$
        //           s$panamabanana
        //
        //           $panamabananas
        //           abananas$panam
        //           amabananas$pan
        //  bottom > anamabananas$p
        //           ananas$panamab
        //     top > anas$panamaban
        //           as$panamabanan
        //           bananas$panama
        //           mabananas$pana
        //           namabananas$pa
        //           nanas$panamaba
        //           nas$panamabana
        //           panamabananas$
        //           s$panamabanana
        // After knowing the range of the matches, we can determine
        //  the index of each match through the suffix array.

        int index = pattern.size() - 1;
        int bottom = 0, top = n - 1;

        while (true)
        {
            while (top >= bottom && transform[top] != pattern[index])
                top--;

            while (bottom <= top && transform[bottom] != pattern[index])
                bottom++;

            if (top < bottom) break;

            top = get_first_column_index(top);
            bottom = get_first_column_index(bottom);

            if (index == 0) break;

            index--;
        }

        std::vector<int> result;
        for (int i = bottom; i <= top; i++) {
            result.push_back(suffix_array[i]);
        }

        return result;
    }
};

void test(const std::string& string, const std::string& pattern)
{
    std::cout << "String : " << string << std::endl;
    std::cout << "Pattern: " << pattern << std::endl;
    std::cout << "Matches: ";
    for (int i : BurrowsWheelerMatcher(string).get_matches(pattern))
        std::cout << i << ' ';
    std::cout << std::endl << std::endl;
}

int main()
{
    test("anana", "ana");
    test("panamabananas", "ana");
    test("mississippi", "ssi");
    test("mississippi", "i");
    test("mississippi", "x");
}