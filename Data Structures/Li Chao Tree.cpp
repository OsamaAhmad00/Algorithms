#include <iostream>
#include <vector>
#include <random>

// Note that negating both slope and Y-intersect has the effect of mirroring about the
//  X-axis. Doing this will give the same result as using the other comparator.
template <typename T>
struct MinComparator { bool operator()(const T& a, const T& b) const { return a < b; } };
template <typename T>
struct MaxComparator { bool operator()(const T& a, const T& b) const { return a > b; } };

template <typename T>
struct Line
{
    T slope;
    T y_intercept;

    T evaluate(const T& x) const { return x * slope + y_intercept; };

    std::string to_string() const {
        return std::to_string(slope) + "x + " + std::to_string(y_intercept);
    }
};

template <typename T, template <typename> typename Comparator = MinComparator>
class LiChaoTree
{
    T n;
    T min_x;
    T max_x;
    std::vector<Line<T>> lines;
    // This can be removed, but the code will be a bit messier.
    std::vector<bool> is_assigned;
    Comparator<T> compare;

    const int root = 0;

    int left_child(int parent) const {
        return parent * 2 + 1;
    }

    int right_child(int parent) const {
        return left_child(parent) + 1;
    }

    bool is_better(const Line<T>& a, const Line<T>& b, const T& x) const {
        return compare(a.evaluate(x), b.evaluate(x));
    }

public:

    // https://www.youtube.com/watch?v=-StmrE2gY44
    // The idea here is simple: We have a binary search tree, and each node in
    //  the tree represents a segment, and have a line associated with it. For a
    //  given integer i, one of the lines along the path from the root to the
    //  leaf node representing x will have the best (maximum/minimum) value x = i.
    // You can see that this is very similar to the segment tree.
    // The querying is very simple, just go through the nodes the same way you'd
    //  do with a binary search tree, and get the line with the best result along
    //  the path from the root to the leaf.
    // Well, and how to add a new line?
    // An invariant to maintain is that given a node n, which
    //  represents the segment [l, r], and a line l, line l should
    //  be the line with best value at x = mid where mid = (l + r) / 2.
    // To add a new line X, start from the root, and compare the two
    //  lines, X, and the line at the current node, call it Y, if X gives
    //  a better result at x = mid, then swap X and Y. After swapping, the
    //  current node will be represented by the line X, and you'll proceed
    //  to add the line Y (in both cases, whether you swapped or not, we'll
    //  call the line to be added X). This maintains the invariant above,
    //  which states that the line associated with a node should give the
    //  best result when being evaluated at x = mid.
    // Note that if two lines intersect, then each line is better at some
    //  segment (assuming that other lines are not better). In other words,
    //  one line will be the best before the intersection point, and the
    //  other will be the best after the intersection point.
    // We have 3 cases for the two lines, X and Y:
    //  1 - Their intersection point is in the segment [l, mid], in which
    //      case, we know that there is a subsegment in [l, mid] that we
    //      still need to set its best line. More specifically, it's the
    //      segment [l, intersection_point]. You can check that this is
    //      the case by evaluating X and Y at x = l, and if the current best
    //      line (Y) doesn't give the best result, then this is the case.
    //  2 - The same case as case 1, but if the intersection point is in
    //      [mid, r], in which case, we still need to set the best line
    //      in the interval [mid, r], because the segment [intersection_point, r]
    //      is not represented correctly (yet). You can check that this is
    //      the case the same way, but by evaluating X and Y at r instead of l.
    //  3 - The lines don't intersect at all in the segment [l, r]. This is
    //      the easiest case, since we don't need to do anything here. The
    //      best line is the best across the whole segment, and no changes
    //      need to be done.
    //  Note that you can't have more than one of these cases happening at
    //   the same time.
    //  One extra case is when intersection point is exactly at mid. We can
    //   ignore this case since the mid point is not included in the segment
    //   of the children nodes anyways.
    //  To add a new line, just keep setting the best line for the nodes
    //   along the path from the root up to the leaf node.

    // Definition: A function has a transcending property if, given two functions
    //  f(x), and g(x). If f(t) is greater than/smaller that g(t) for some x = t,
    //  then f(x) will be greater than / smaller than g(x) for x > t. In other words,
    //  once f(x) "win/lose" g(x), f(x) will continue to "win/lose" g(x).
    // This data structure can be used with any function with the transcending property,
    //  and not restricted to linear functions only.

    void add_line(Line<T> line)
    {
        int node = root;
        T left = min_x;
        T right = max_x;

        while (left <= right)
        {
            if (!is_assigned[node])
            {
                // This is an optimization. If this node is a new
                // node, it's sufficient to assign the new line to
                // it, and break. This is because by the time we
                // reach here, other candidate lines (if exists)
                // will be set at the ancestors, and there is nothing
                // else to compare to here.
                is_assigned[node] = true;
                lines[node] = line;
                break;
            }

            T mid = left + (right - left) / 2;

            if (is_better(line, lines[node], mid)) {
                std::swap(line, lines[node]);
            }

            if (is_better(line, lines[node], left)) {
                node = left_child(node);
                right = mid - 1;
            } else if (is_better(line, lines[node], right)) {
                node = right_child(node);
                left = mid + 1;
            } else {
                break;
            }
        }
    }

    const Line<T>& query_line(const T& x)
    {
        int node = root;
        int result_node = node;

        T left = min_x;
        T right = max_x;

        while (left <= right)
        {
            if (!is_assigned[node])
                break;

            if (is_better(lines[node], lines[result_node], x))
                result_node = node;

            T mid = left + (right - left) / 2;

            if (x < mid) {
                node = left_child(node);
                right = mid - 1;
            } else if (x > mid) {
                node = right_child(node);
                left = mid + 1;
            } else {
                break;
            }
        }

        return lines[result_node];
    }

    T query(const T& x) {
        return query_line(x).evaluate(x);
    }

    LiChaoTree(const T& min_x, const T max_x)
        : n(max_x - min_x), min_x(min_x), max_x(max_x),
          lines(4 * n + 1), is_assigned(4 * n + 1, false)
    {}
};

void test(int lines_count, int queries_count)
{
    LiChaoTree<int, MaxComparator> tree(-1'000'000, 1'000'000);

    std::random_device rd1;
    std::mt19937 gen1(rd1());
    std::uniform_int_distribution<> line_distribution(0, 2000);

    std::vector<Line<int>> lines;
    for (int i = 0; i < lines_count; i++) {
        Line<int> line = {line_distribution(gen1) - 1000, line_distribution(gen1) - 1000};
        lines.push_back(line);
        tree.add_line(line);
    }

    std::random_device rd2;
    std::mt19937 gen2(rd2());
    std::uniform_int_distribution<> query_distribution(0, 2'000'000);

    for (int i = 0; i < queries_count; i++) {
        int x = query_distribution(gen2) - 1'000'000;
        auto result = tree.query(x);

        for (auto& line: lines) {
            if (line.evaluate(x) > result) {
                std::cout << "Fail..." << std::endl;
                break;
            }
        }
    }
}

void test(const std::vector<Line<int>>& lines, const std::vector<int>& queries)
{
    int min_x = -10000;
    int max_x =  10000;
    LiChaoTree<int, MaxComparator> tree{min_x, max_x};

    for (auto& line : lines)
        tree.add_line(line);

    for (int x : queries) {
        auto result = tree.query_line(x);
        std::cout << "Maximum value at x = " << x << " is ";
        std::cout << result.evaluate(x) << " (from line " ;
        std::cout << result.to_string() << ")" << std::endl;
    }
}

std::vector<Line<int>> get_sample_lines_1()
{
    return {{1, 2}, {2, -3}, {0, 4}, {-1, 4}, {-2, -1}};
}

std::vector<int> get_sample_queries_1()
{
    return {-8, -5, -2, -1, 0, 1, 2, 3, 5, 8};
}

int main()
{
    test(get_sample_lines_1(), get_sample_queries_1());
    test(10000, 10000);
}