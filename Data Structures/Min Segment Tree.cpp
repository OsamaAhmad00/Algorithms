#include <iostream>
#include <vector>

const int MAX_VALUE = INT_MAX;

struct Range
{
    int start;
    int end;
};

template <typename T>
class MinSegmentTree
{
    // MAKE SURE TO USE A DATA TYPE THAT WON'T
    //  OVERFLOW. THE RESULTS ARE NOT GUARANTEED
    //  TO BE CORRECT ONCE AN OVERFLOW OCCURS.

    // The leaves will not be updated with
    //  their delta values (will always be 0).
    // Do we need both min and delta for the
    //  leaves or is only one is enough?

    const int root = 0;

    // Min and delta are set to be mutable
    //  to allow for pushing the deltas down
    //  the tree for const segment trees.
    mutable std::vector<T> min;
    mutable std::vector<T> delta;
    std::vector<Range> ranges;

    int left(int parent) const {
        return parent * 2 + 1;
    }

    int right(int parent) const {
        return left(parent) + 1;
    }

    bool is_complete_coverage(int node, const Range& range) const
    {
        // The given range covers the range of this
        //  node completely
        return ranges[node].start >= range.start &&
               ranges[node].end   <= range.end;
    }

    bool is_out_of_range(int node, const Range& range) const
    {
        return ranges[node].start > range.end ||
               ranges[node].end   < range.start;
    }

    void push_delta(int node) const
    {
        delta[left (node)] += delta[node];
        delta[right(node)] += delta[node];
        delta[node] = 0;
    }

    void increment(int node, const Range& range, const T& value)
    {
        if (is_out_of_range(node, range))
            return;

        // You can only update the delta and
        // array and return in case of a complete
        // coverage, otherwise, you'll have to
        // keep incrementing the children.

        if (is_complete_coverage(node, range))
        {
            delta[node] += value;
            return;
        }

        int l = left (node);
        int r = right(node);

        // Partial coverage case
        push_delta(node);
        increment(l, range, value);
        increment(r, range, value);
        min[node] = std::min(min[l] + delta[l], min[r] + delta[r]);
    }

    T query(int node, const Range& range) const
    {
        if (is_out_of_range(node, range))
            return MAX_VALUE;

        if (is_complete_coverage(node, range))
            return min[node] + delta[node];

        int l = left (node);
        int r = right(node);

        // Partial coverage case
        min[node] += delta[node];
        push_delta(node);
        T result = std::min(query(l, range), query(r, range));
        return result;
    }

    void init_ranges(int node, const Range& range)
    {
        ranges[node] = range;

        // a leaf node
        if (range.start == range.end)
            return;

        int middle = (range.start + range.end) / 2;
        init_ranges(left (node), {range.start, middle});
        init_ranges(right(node), {middle + 1, range.end});
    }

    void init(const std::vector<T>& array)
    {
        // A complete binary tree with n
        //  leaves will have n - 1 internal
        //  nodes (a total of 2n -1 nodes).
        // We have no guarantee that the
        //  binary tree will be complete,
        //  thus, we have to reserve one layer
        //  deeper in the tree to count for the
        //  case where the tree is not complete.
        // On each layer of a binary tree, the
        //  number of nodes double. This means
        //  that for us to reserve for the layer
        //  below the leaves (with n nodes), we'll
        //  have to reserve additional 2n nodes.
        int n = 4 * array.size() + 1;
        min.resize(n);
        delta.resize(n);
        ranges.resize(n);

        init_ranges(root, {0, (int)array.size() - 1});

        for (int i = 0; i < array.size(); i++)
            increment({i, i}, array[i]);
    }

public:

    void increment(const Range& range, const T& value) {
        return increment(root, range, value);
    }

    T query(const Range& range) const {
        return query(root, range);
    }

    explicit MinSegmentTree(const std::vector<T>& array) {
        init(array);
    }
};

Range get_random_range(int size)
{
    int l = rand() % size;
    int r = rand() % size;
    if (l > r) std::swap(l, r);
    return {l, r};
}

void test_random_query(const std::vector<int>& v, const MinSegmentTree<int>& s)
{
    int min = MAX_VALUE;
    Range r = get_random_range(v.size());

    for (int i = r.start; i <= r.end; i++)
        min = std::min(min, v[i]);

    if (s.query(r) == min) {
//        std::cout << "Test Passed" << std::endl;
    } else {
        std::cout << "Test Failed" << std::endl;
    }
}

void test(int size, int queries)
{
    std::vector<int> v;
    for (int i = 0; i < size; i++)
        v.push_back(rand());

    MinSegmentTree<int> s(v);

    while (queries--)
    {
        Range r = get_random_range(v.size());

        int value = rand() % 100;
        s.increment(r, value);
        for (int i = r.start; i <= r.end; i++)
            v[i] += value;

        test_random_query(v, s);
    }
}

int main()
{
    test(1000, 1000000);
}