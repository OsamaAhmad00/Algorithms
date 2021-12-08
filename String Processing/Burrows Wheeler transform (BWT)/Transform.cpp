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

std::string invert_BWT_fast(const std::string& transform, const std::vector<int>& suffix_array)
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
    // In general, for any character c, the ith appearance of the
    //  character c in the first column is the corresponds to the ith
    //  appearance of the character c in the last column.
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
    //  This is the order in which these cyclic shifts appear in the second half.
    //  This is called the "First-Last Property".
    // We can make use of this property to construct the original string.
    // We'll start with the last character (call it c) of the cyclic shift at
    //  index i with shift = 0 (suffix_array[i] = 0), then, since we can map
    //  between characters from the first and the last column, we'll go to
    //  character c in the first column (the cyclic shift that starts with
    //  the character c). The character at the end of this cyclic shift is the
    //  next character to add to the result string (add from the front).
    //  We'll repeat until we construct the whole string.

    int n = transform.size();

    // This can be set as a constant.
    int max_char = 0;
    for (int i = 0; i < n; i++)
        max_char = std::max(max_char, (int)transform[i]);

    // first_column_index[c][i] = the index of the ith appearance
    //  of the characters c in the first column.
    std::vector<std::vector<int>> first_column_index(max_char + 1);

    // last_column_count[i] = the count of the appearances of the
    // ith character in the first column so far.
    std::vector<int> last_column_count(n);

    {
        // The variable name "count" will be used later,
        //  thus, putting this "count" vector in a block
        //  so that the name is available again outside
        //  this block without warnings :(
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

    int index = 0;
    // You can append a character that is not in the
    // whole string, and lexicographically smaller
    // than any character in the string, then you'll
    // know for sure that the index of the original
    // string (cyclic shift with shift = 0) will be 0.
    for (int i = 0; i < n; i++) {
        if (suffix_array[i] == 0) {
            index = i;
            break;
        }
    }

    std::string result;
    result.resize(n);

    // We're constructing the original string backwards.
    // This is because we have the last column, and not
    //  the first column. If you wanted to construct the
    //  original string forwards, you'll have to construct
    //  the first column of the cyclic shifts, which is
    //  not needed.
    // This is because we need to get characters by index,
    //  and unless we can get characters by index from the
    //  first column, we won't be able to construct the
    //  original string forwards.
    for (int i = n - 1; i >= 0; i--) {
        char c = transform[index];
        result[i] = c;
        int count = last_column_count[index];
        index = first_column_index[c][count];
    }

    return result;
}

std::string invert_BWT_slow(const std::string& transform, const std::vector<int>& suffix_array)
{
    // We create all cyclic shifts, of the original string,
    //  then return the cyclic shift with shift = 0. This cyclic
    //  shift should be at the index i such that suffix_array[index] = 0.
    // We make use of the fact that we know that the first column
    //  in the rotations = all characters of the string, sorted.
    //  Now, we have the first column, but, we also have the last
    //  column, which is the transform itself. From these two columns,
    //  we can deduce the first two columns of the sorted cyclic shifts.
    // Since the last column is the characters before the characters of
    //  the first column, we can form valid substrings of size 2 for each
    //  cyclic shift. Each substring will be the character from the last
    //  column (the transformation) + the character from the first column.
    //  If we sort these substrings with size 2, we'll get the first two
    //  rows of the cyclic shifts.
    // But..., again, we have the last column, and using this column, along
    //  with the substrings of size 2, we can deduce the first 3 columns of
    //  the sorted cyclic shifts, and then we can deduce the first 4 column,
    //  5 columns, ... and so on. Doing it n times will give us the sorted
    //  cyclic shifts.

    // This is O(n^3 * log(n)). This is because we sort inside a loop, and
    //  the sorting compares strings. Comparing strings takes O(n), and
    //  sorting takes O(n * log(n)) and the loop takes O(n).

    int n = transform.size();

    std::vector<std::string> cyclic_shifts(n);

    for (int _ = 0; _ < n; _++) {
        for (int i = 0; i < n; i++) {
            cyclic_shifts[i] = transform[i] + cyclic_shifts[i];
        }
        std::sort(cyclic_shifts.begin(), cyclic_shifts.end());
    }

    // You can append a character that is not in the
    // whole string, and lexicographically smaller
    // than any character in the string, then you'll
    // know for sure that the index of the original
    // string (cyclic shift with shift = 0) will be 0.
    for (int i = 0; i < n; i++) {
        if (suffix_array[i] == 0) {
            return cyclic_shifts[i];
        }
    }
}

void test(const std::string& string)
{
    std::vector<int> suffix_array = SuffixArray(string).get_suffix_array();
    std::string transform = get_BWT(string, suffix_array);
    std::cout << "String   : " << string << std::endl;
    std::cout << "Transform: " << transform << std::endl;
    std::cout << "Inverted : " << invert_BWT_fast(transform, suffix_array) << std::endl;
    std::cout << std::endl;
}

int main()
{
    test("mississippi$");
}