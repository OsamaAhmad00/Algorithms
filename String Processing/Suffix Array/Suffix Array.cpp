#include <iostream>
#include <vector>

struct CyclicShift
{
    // index of the first character
    // of the original string.
    int start_index;

    // Also known as "Group" or "Equivalence Class"
    // This is an optimization that is used for a
    //  faster comparisons between cyclic shifts.
    // To test whether two cyclic shifts are equal
    //  or not, it's sufficient to just compare
    //  their ranks.
    // x.rank <  y.rank <=> x <  y
    // x.rank == y.rank <=> x == y
    // x.rank >  y.rank <=> x >  y
    int rank;

    // Comparisons and indexing are done
    //  by the rank of the cyclic shift.
    operator int() const { return rank; }
};

std::vector<CyclicShift> stable_sort(const std::vector<CyclicShift> &shifts)
{
    // Count Sort - O(N + M) where N is the size of
    // the string and M is the size of count array.

    // M = max_val + 1
    // This can be optimized away, and be passed as a
    // parameter. Done this way just for convenience.
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
        // This happens only when initializing.
        // Just for this time, during the initialization,
        //  we sort the shifts by the characters and not
        //  the rank. It shouldn't cost any more since
        //  the length of the shift is 1 when initializing.
        // Since the sorting function sorts by the rank of
        //  the cyclic shift, we can set the rank of each
        //  cyclic shift to be its first character, this
        //  way, comparisons will be by the characters.

        // If a suffix x is a prefix of another suffix y,
        //  then x < y. To avoid checking for this special
        //  case, we append an imaginary character (call it c)
        //  to the end of the string. This character has the
        //  smallest rank, and doesn't appear in the string.
        //  This way, every single suffix will be unique.
        //  Also, since x < y, and c < any character in the
        //  string, xc < yc, which means that the sorting will
        //  be correct without checking for this special case.
        //  When the computations are done, we can get rid of c
        //  and obtain correct results.
        // Since the character is appended at the end, it won't
        //  shift the characters and won't affect the indices.
        shifts.back() = {n - 1, 0};

        for (int i = 0; i < n - 1; i++) {
            shifts[i].start_index = i;
            shifts[i].rank = string[i];
        }

        shifts = stable_sort(shifts);

        shifts[0].rank = 0;
        for (int i = 1; i < n; i++) {
            // Since the shifts are sorted, if shifts[i-1] != shifts[i]
            //  it can only be the case that shifts[i-1] < shifts[i].
            // If the current shift is > the previous shift, then its
            //  rank should be rank of the previous shift + 1.
            // x.rank <  y.rank <=> x <  y
            // x.rank == y.rank <=> x == y
            // x.rank >  y.rank <=> x >  y
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
        // old_ranks store the ranks of the shifts with
        //  length = current_shift_length.
        // We're trying to set the new ranks of shifts
        //  with length = 2 * current_shift_length.
        // Since the shifts are sorted by their first half,
        //  for some index i, and 2 shifts, x = shifts[i-1]
        //  and y = shifts[i], if the rank of the first
        //  half of x != the rank of first half of y, then
        //  we know that x < y and rank of y should be
        //  x.rank + 1.
        //  But, if the rank of their first halves is the
        //  same, then we compare the second half. We know
        //  that before sorting by the first halves, the
        //  shifts were sorted by what is currently considered
        //  to be the second half, and since the sorting
        //  algorithm is stable, then we know that given
        //  that the first halves of x and y are equal,
        //  then they're going to be sorted by their second
        //  half. This means that if the rank of the second
        //  half of x != the rank of the second half of y,
        //  then it can only be the case that rank of second
        //  half of x < the rank of the second half of y.
        //  unless the two halves of x and y are the same,
        //  y.rank should be x.rank + 1.

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
        // This is a mapping from a cyclic shift to its rank.
        // This is set before shifting the cyclic shift.
        // This is used to get the rank of any shift with O(1).
        for (CyclicShift& shift : shifts)
            old_ranks[shift.start_index] = shift.rank;
    }

    void shift()
    {
        // If the cyclic shift is shifted, the rank of the new
        //  shift should be set accordingly.
        // Since the shifts array gets sorted by the rank, we
        //  need to set the rank of each shift to the rank of
        //  itself after getting shifted.
        // As of for now, we only know the rank of the first half and
        //  the second half of the shift, separately, but we don't know
        //  the rank of the two halves combined. Each of the two halves
        //  has a length of "current_shift_length". How are we supposed
        //  to set the rank of the new shift of length 2 * current_shift_length?
        // One thing to note is that, if we know that the first half
        //  of each shift is unique, we don't need to worry about the
        //  second half of the shift, sorting only by the rank of the
        //  first half of the shift will be sufficient.
        // But what if the first halves were not unique among all shifts?
        //  Here, we can utilize the fact that the shifts were already
        //  sorted by their second half, and since we're using a STABLE
        //  sorting algorithm, if the first halves are equal, the shifts
        //  will be sorted by their second half (since it was already
        //  sorted by the second half before).
        // Conclusion: to sort the new shifts, for each new shift, we will
        //  set its rank to the rank of its first half, then sort by the
        //  ranks. This relies on the fact that the sorting algorithm is
        //  STABLE. If the sorting algorithm is not stable, this won't work.

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
        // Each shift has a unique rank.
        // Since the shifts are sorted, and
        //  the ranks are sequential, if the
        //  last shift has an rank = n - 1,
        //  then we know that each shift has
        //  a unique rank.
        return shifts.back().rank == (n - 1);
    }

    void compute_suffix_array(const std::string& string)
    {
        // Let c = a character that is lexicographically
        //  smaller than any character in the string.
        // Fact: the array of the start indices of sorted
        //  suffices = the array of the shift amounts of
        //  sorted cyclic shifts of the string with c
        //  appended to it.
        // We know how to compute the array of sorted cyclic
        //  shifts. Utilizing the fact above, we can just
        //  compute the sorted cyclic shifts array.

        // To compute sorted cyclic shifts, we first compute
        // all sorted cyclic shifts with length 1. This is
        // the initialization phase.
        initialize_shifts(string);

        // After initializing shifts with length 1, we compute
        // the sorted shifts with length 2, then 4, then ...
        // until we reach a cyclic shift of length >= n.
        while (!is_done())
            double_shift_length();

        // The first item in the shifts starts with the imaginary
        // character that we've appended. This is not part of
        // the actual string, so we ignore the first item.
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

void test(const std::string& string)
{
    SuffixArray array(string);

    std::cout << "String: " << string << std::endl;
    std::cout << "Suffix Array: ";
    for (int i : array.get_suffix_array())
        std::cout << i << ' ';

    std::cout << std::endl << std::endl;
}

int main()
{
    test("abracadabra");
    test("aaaa");
    test("aabbcd");
    test("panamabananas");
}