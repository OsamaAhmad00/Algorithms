#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>

template <typename T, bool enable_sum = true>
class WaveletTree
{
    // https://www.youtube.com/watch?v=4aSv9PcecDw
    // https://users.dcc.uchile.cl/~jperez/papers/ioiconf16.pdf
    // This tree is like a segment tree, but on the values instead
    //  of indices.
    // Each node represents values in the range [min, max]. The left
    //  child is represents the values in the range [min, mid], and
    //  the right child represents the values in the range [mid + 1, max].
    // We keep dividing the nodes recursively until a leaf node with
    //  min = max is reached. This leaf node can represent multiple equal
    //  elements if the same value is repeated in the array.
    // Surprisingly, having a tree with this structure enables some queries
    //  like "Rank" (count), "Quantile" (kth_smallest), and "Range Counting"
    //  (LTE_count), and some other queries in O(log(A)) where A is the size
    //  of the alphabet, or in other words, the number of different values
    //  that can appear in the array, which is equal to max - min + 1.
    // Note that if the alphabet size is bigger than the size of the array,
    //  you can perform "Coordinate Compression", and compress the size of the
    //  alphabet.

    // TODO add the ability to perform updates.
    // TODO make sure the time complexity of all queries is O(log(A)).

    T min;
    T max;
    std::unique_ptr<WaveletTree> left = nullptr;
    std::unique_ptr<WaveletTree> right = nullptr;
    std::vector<int> left_count_array;
    std::vector<T> prefix_sum;

    T mid() const { return min + (max - min) / 2; }
    T sum(int l, int r) const { return prefix_sum[r + 1] - prefix_sum[l]; }
    bool is_leaf() const { return min == max; }
    static int size(int l, int r) { return r - l + 1; }

    // These map methods will return -1 in case there is no mapping for "index".
    int map_left(int index) const { return left_count_array[index + 1] - 1; }
    int map_right(int index) const { return index - left_count_array[index + 1]; }

    int LTE_count(const T& num, int i)
    {
        // If num < min, then we know that
        //  nothing in this node will be
        //  less than num.
        // If num >= max, then we know that
        //  all values from 0 to i are <= num.
        if (i == -1 || num < min) return 0;
        if (num >= max) return i + 1;
        return left ->LTE_count(num, map_left(i)) +
               right->LTE_count(num, map_right(i));
    }

    T LTE_sum(const T& num, int i)
    {
        // This is exactly the same as LTE_count,
        //  but returns the sum instead of the
        //  number of elements.
        if (i == -1 || num < min) return 0;
        if (num >= max) return prefix_sum[i+1];
        return left ->LTE_sum(num, map_left(i)) +
               right->LTE_sum(num, map_right(i));
    }

    int count(const T& num, int i)
    {
        // If we reach a leaf node, then we know for sure that
        //  this leaf node represents [num, num]. Moreover,
        //  since we're reaching this node with index = i,
        //  then we know that there are i + 1 instances of
        //  num up until this index.
        if (i == -1) return 0;
        if (is_leaf()) return i + 1;
        // If num <= mid, then we know that it'll be in the
        //  left child. Else, in the right child.
        if (num <= mid())
            return left->count(num, map_left(i));
        return right->count(num, map_right(i));
    }

public:

    template <typename Iterator>
    static WaveletTree create(Iterator begin, Iterator end, const T& min, const T& max)
    {
        // This is a helper function used to
        // avoid making a copy of the array.
        std::vector<T> temp(begin, end);
        return WaveletTree<T, enable_sum>(temp.begin(), temp.end(), min, max);
    }

    // WARNING: THIS WILL CHANGE THE ORDER OF
    //  THE ELEMENTS IN THE RANGE [begin, end).
    //  IF THE ARRAY SHOULDN'T BE MODIFIED,
    //  PASS A COPY OF THE ARRAY.
    template <typename Iterator>
    WaveletTree(Iterator begin, Iterator end, const T& min, const T& max)
    {
        // We're performing this check here instead of checking before
        //  creating the node. Having a dummy node makes us not need to
        //  check whether each child is a nullptr or not. Instead, we
        //  just check whether we're in an invalid state after entering
        //  the child node or not.
        if (begin == end) return;

        this->min = min;
        this->max = max;

        T m = mid();
        auto predicate = [&m](const T& x) {
            return x <= m;
        };

        // This array counts the number of elements went to the left child
        //  up until a certain index. This helps us to map an index in the
        //  parent node to an index in both the left and the right child.
        // Basically, if up until index i, there are x elements that went
        //  to the left node, meaning that there are x elements that are
        //  <= mid(), then we know that i maps to the index x in the left
        //  node.
        // Similarly, if up until index i, there are x elements that went
        //  to the left node, then we know that the remaining i - x elements
        //  are in the right node, thus, we know that i maps to i - x in the
        //  right node.
        // left_count_array[i + 1] = the number of elements that went to the
        //  left child up until the index i.
        left_count_array.reserve(end - begin + 1);
        left_count_array.push_back(0);
        for (Iterator it = begin; it != end; it++)
            left_count_array.push_back(left_count_array.back() + predicate(*it));

        if (enable_sum)
        {
            prefix_sum.reserve(end - begin + 1);
            prefix_sum.push_back(0);
            for (Iterator it = begin; it != end; it++)
                prefix_sum.push_back(prefix_sum.back() + *it);
        }

        if (!is_leaf())
        {
            // It's important that the partitioning is stable.
            Iterator pivot = std::stable_partition(begin, end, predicate);
            left = std::make_unique<WaveletTree>(begin, pivot, min, m);
            right = std::make_unique<WaveletTree>(pivot, end, m+1, max);
        }
    }

    T kth_smallest(int k, int l, int r)
    {
        // Unlike other methods, this one is must have a
        //  result (given that k >= 0 && k < size(l, r)),
        //  thus, we don't need to check for the bounds.
        // Also, we can't break this query into 2 like
        //  the other methods.
        if (is_leaf()) return min;

        // The number of elements in the range [l, r] that
        //  are in the left child. If k < left_count, then
        //  we know for sure that the kth smallest element =
        //  the kth smallest element in the left child. Else,
        //  since there already exists left_count elements in
        //  the left child, then it should be the (k - left_count)th
        //  element in the right child.
        int left_count = left_count_array[r + 1] - left_count_array[l];

        // A "<" is used because k starts from 0.
        // If k starts from 1, use "<=" instead.
        // TODO Write why the mapping of l = map(l-1) + 1.
        if (k < left_count)
            return left->kth_smallest(k, map_left(l-1) + 1, map_left(r));
        return right->kth_smallest(k - left_count, map_right(l-1) + 1, map_right(r));
    }

    T kth_biggest(int k, int l, int r) {
        return kth_smallest(size(l, r) - 1 - k, l, r);
    }

    // Note that the methods below breaks each query into
    //  2 queries. The queries can be done in a single query,
    //  but doing it this way is simpler.

    T LTE_sum(const T& num, int l, int r) {
        return LTE_sum(num, r) - LTE_sum(num, l - 1);
    }

    T GT_sum(const T& num, int l, int r) {
        return sum(l, r) - LTE_sum(num, l, r);
    }

    int LTE_count(const T& num, int l, int r) {
        return LTE_count(num, r) - LTE_count(num, l - 1);
    }

    int GT_count(const T& num, int l, int r) {
        return size(l, r) - LTE_count(num, l, r);
    }

    int count(const T& num, int l, int r) {
        return count(num, r) - count(num, l - 1);
    }
};

void test(int size)
{
    int alphabet_size = 100;

    std::vector<int> v(size);
    for (int i = 0; i < size; i++)
        v[i] = rand() % alphabet_size;

    auto wt = WaveletTree<int, true>::create(v.begin(), v.end(), 0, alphabet_size);

    for (int l = 0; l < v.size(); l++) {
        for (int r = l; r < v.size(); r++)
        {
            std::vector<int> sorted(v.begin() + l, v.begin() + r + 1);
            std::sort(sorted.begin(), sorted.end());

            for (int number = 0 ; number < alphabet_size; number++)
            {
                int lt = 0;
                int gt = 0;
                int count = 0;
                int lte_sum = 0;
                int gt_sum = 0;
                for (int i = l; i <= r; i++) {
                    if (v[i] == number) count++, lte_sum += v[i];
                    else if (v[i] < number) lt++, lte_sum += v[i];
                    else gt++, gt_sum += v[i];
                }

                if (count != wt.count(number, l, r))
                    std::cout << "Fail in count" << std::endl;
                if (lt + count != wt.LTE_count(number, l, r))
                    std::cout << "Fail in LTE_count" << std::endl;
                if (gt != wt.GT_count(number, l, r))
                    std::cout << "Fail in GT_count" << std::endl;
                if (lte_sum != wt.LTE_sum(number, l, r))
                    std::cout << "Fail in LTE_sum" << std::endl;
                if (gt_sum != wt.GT_sum(number, l, r))
                    std::cout << "Fail in GT_sum" << std::endl;
            }

            for (int i = 0; l + i <= r; i++)
            {
                if (wt.kth_smallest(i, l, r) != sorted[i])
                    std::cout << "Fail in kth_smallest" << std::endl;
                if (wt.kth_biggest(i, l, r) != sorted[sorted.size() - 1 - i])
                    std::cout << "Fail in kth_biggest" << std::endl;
            }
        }
    }
}

int main()
{
    test(100);
}