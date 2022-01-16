#include <iostream>
#include <vector>
#include <queue>
#include <random>

// This problem is solvable in a single pass in O(n).
// This is meant to be a simpler version of the maximum
//  sum-matrix problem, used to illustrate the algorithm
//  in a simpler way.

template <typename T>
class PrefixSum1D
{
    std::vector<T> sums;

public:

    PrefixSum1D(const std::vector<T>& array) : sums(array.size() + 1)
    {
        sums[0] = 0;

        for (int i = 0; i < array.size(); i++) {
            sums[i + 1] = sums[i] + array[i];
        }
    }

    T query(int l, int r) {
        return sums[r + 1] - sums[l];
    }
};

template <typename T>
class MaxSumSubarray
{

public:

    struct Range
    {
        int from;
        int to;

        std::pair<Range, Range> split() const {
            int mid = (from + to) / 2;
            return {{from, mid}, {mid + 1, to}};
        }

        int size() const { return to - from + 1; }
    };

private:

    struct Node
    {
        // Each node is responsible for the maximum subarray in
        // the ranges represented by the following two nested loops:
        // for (int range_start = start.from; range_start <= start.to; range_start++)
        //     for (int range_end = end.from; range_end <= end.to; range_end++)
        //         [range_start, range_end]
        Range start;
        Range end;

        int max_sum;

        bool can_branch() const {
            // If the whole node represents a single range, then
            //  it can't branch anymore.
            return !(start.from == start.to && end.from == end.to);
        }

        bool is_valid() const { return start.from <= end.to; }

        bool operator <(const Node& node) const { return max_sum < node.max_sum; }
    };

    int n;
    PrefixSum1D<T> prefix_sum;
    // Prefix sum of the array where all
    // negative numbers are set to 0.
    PrefixSum1D<T> positive_prefix_sum;

    std::priority_queue<Node> queue;

    std::vector<T> get_positive_array(const std::vector<T>& array)
    {
        std::vector<T> result(array);
        for (T& t : result)
            if (t < 0) t = 0;
        return result;
    }

    void add_to_queue(const Node& node)
    {
        if (node.is_valid())
            queue.push(node);
    }

    bool is_intersecting(const Range& r1, const Range& r2) {
        return r1.to > r2.from;
    }

    int compute_max_sum(const Node& node)
    {
        /*
         * The idea here is simple, just ignore negative numbers!
         *  That's it. By only summing the positive numbers, this
         *  will give an upper bound to any possible subarray that
         *  this node represents.
         * But... We need to guarantee that if the node represents
         *  a single range, then the upper bound is exactly the sum
         *  of the range represented by this node.
         * There is an observation that will make it easy to guarantee this
         *  requirement: note that the range [node.start.to, node.end.from]
         *  is included in every range this node represents. This is only
         *  true if node.start.to <= node.end.from. If not, then some ranges
         *  will not include [node.start.to, node.end.from].
         * Utilizing this observation, this means that we can include
         *  the negative values in the range [node.start.to, node.end.from]
         *  and still guarantee that this is an upper bound. This is because
         *  the negative numbers in this range will appear in every range
         *  this node represents.
         * But, how would this help?
         *  We can see that if node represents a single range, then node.start.from
         *  is equal to node.start.to, and node.end.from is equal to node.end.to,
         *  which means that the range [node.start.to, node.end.from] represents
         *  the single range represented by the node, and since we include the
         *  negative numbers as well in this range, then we know for sure that
         *  the upper bound = the sum of the range.
         */

        int sum = positive_prefix_sum.query(node.start.from, node.end.to);
        if (!is_intersecting(node.start, node.end)) {
            sum -= positive_prefix_sum.query(node.start.to, node.end.from);
            sum += prefix_sum.query(node.start.to, node.end.from);
        }
        return sum;
    }

    std::pair<Node, Node> branch_node(const Node& node)
    {
        // TODO is there a cleaner way of doing this?

        // The range with bigger size is
        //  the one that will be split.
        // This is guaranteed that after
        //  some branching, each node will
        //  represent a single range.

        auto& start = node.start;
        auto& end = node.end;

        std::pair<Node, Node> result;

        if (start.size() > end.size()) {
            auto starts = start.split();
            result = {{starts.first, end}, {starts.second, end}};
        } else {
            auto ends = end.split();
            result = {{start, ends.first}, {start, ends.second}};
        }

        result.first.max_sum = compute_max_sum(result.first);
        result.second.max_sum = compute_max_sum(result.second);

        return result;
    }


public:

    MaxSumSubarray(const std::vector<T>& array)
        : prefix_sum(array), positive_prefix_sum(get_positive_array(array)), n(array.size()) {}

    struct Result
    {
        int sum;
        Range range;
    };

    Result compute()
    {
        /*
         * This is very similar to having a double nested
         *  loops iterating over all possible ranges, using
         *  prefix sum to compute the sum over each range,
         *  and returning the maximum result.
         * The only difference here is that the ranges are
         *  not being tested SEQUENTIALLY like how a nested
         *  loops would perform. Instead, the most "promising"
         *  ranges are being tested first.
         * A node here represents a set of ranges, and on each
         *  iteration, the number of ranges represented by a node
         *  is split in half, until the node can no longer "branch",
         *  in which case, the node represents a single range.
         * Note that we don't halve the ranges themselves, instead
         *  we halve the number of ranges represented by a node.
         * The technique used here is called "Branch and Bound".
         *  "branch" because every node branches into two sub-nodes,
         *  and "bound" because we ignore the nodes that are less
         *  "promising", and branch from the more promising nodes.
         * Each node contains an "indicator". This indicator indicates
         *  how promising the node is. In this case, the indicator
         *  is max_sum. This is an upper bound on the maximum sum
         *  of all the ranges the node represents. How this upper
         *  bound is calculated is described in its own function.
         * Note that by branching from each node, we're conceptually
         *  forming a binary tree, and on each iteration, we extend
         *  the most promising leaf node.
         * To process the most promising node first, we use a priority
         *  queue to get the most promising node (the node with the
         *  maximum "max_sum" so far) on each iteration.
         * Something that plays a big role here is that the function
         *  that computes the upper bound guarantees that if a node
         *  represents a single range, then its upper bound (max_sum)
         *  will be the actual sum of the range.
         * Since the upper bound >= the maximum sum of all ranges represented
         *  by a node, if the top of the priority queue is a node that
         *  represents a single range, then we know for sure that this
         *  node represents the maximum sum subarray. This is because
         *  the upper bound of this node is its actual range sum, not
         *  more and not less. And since it's at the top of the priority
         *  queue, then the sum of the range it represents is bigger
         *  than (or equal to) every other possible subarray sum.
         * If the function that computes the upper bound didn't guarantee
         *  that the upper bound of a node with a single range will be
         *  exactly the sum of the range, then we would've had to keep
         *  processing the nodes until the upper bound of all nodes becomes
         *  less than the current maximum value.
         */

        Range whole_array = {0, n-1};
        Node root = {whole_array, whole_array};
        root.max_sum = compute_max_sum(root);

        queue.push(root);

        while (!queue.empty())
        {
            Node node = queue.top();
            queue.pop();

            if (!node.can_branch()) {
                // A node that represents a single range.
                return {node.max_sum, {node.start.from, node.end.to}};
            }

            auto split = branch_node(node);
            add_to_queue(split.first);
            add_to_queue(split.second);
        }

        // Shouldn't reach this case.
        return {-1, {-1, -1}};
    }
};

template <typename T>
struct Range
{
    int from;
    int to;
    T sum;
};

// Fast O(n) method.
template <typename T>
Range<T> get_max_sum(const std::vector<T>& array)
{
    Range<T> result = {array[0], 0, 0};
    Range<T> current = {0, 0, 0};

    for (int i = 0; i < array.size(); i++)
    {
        if (current.sum < 0) {
            current.sum = 0;
            current.from = i;
        }

        current.sum += array[i];
        current.to = i;

        if (current.sum > result.sum)
            result = current;
    }

    return result;
}

void test(int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 200 + 1);

    std::vector<int> v(size);

    for (int &i : v)
        i = (int)distrib(gen) - 100;

    auto correct = get_max_sum(v);
    auto result = MaxSumSubarray<int>(v).compute();

    if (result.sum != correct.sum) {
        std::cout << "Fail..." << std::endl;
    }

//    std::cout << "Array: ";
//    for (int i : v) std::cout << i << ' ';
//    std::cout << std::endl;
//    std::cout << "Max sum = " << result.sum << " (from index " << result.range.from;
//    std::cout << " to " << result.range.to << ")" << std::endl << std::endl;
}

int main()
{
    for (int size = 1; size < 1000; size++) {
        test(size);
    }
}