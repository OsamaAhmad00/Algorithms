#include <iostream>
#include <set>
#include <vector>

template <typename T>
struct Line
{
    T slope;
    T y_intercept;

    T evaluate(const T& x) const { return x * slope + y_intercept; };

    bool operator==(const Line& line) const {
        return slope == line.slope && y_intercept == line.y_intercept;
    }

    bool operator<(const Line& line) const {
        return slope < line.slope;
    }

    std::string to_string() const {
        return std::to_string(slope) + "x + " + std::to_string(y_intercept);
    }
};

template <typename T, bool max_query = true, bool increasing_queries = true>
class ConvexHullTrick
{
    // Here, since there are no restrictions on the insertions, you can
    //  specify whether the queries are increasing or decreasing.

    // Note that negating the slope has the effect of mirroring lines about the Y-axis. You can
    //  do this if the slopes are sorted in the reverse order.
    // Also, note that negating both slope and Y-intersect has the effect of mirroring about the
    //  X-axis. You can do this if you want to query the minimum/maximum instead of the maximum/minimum.

    std::multiset<Line<T>> set;
    typedef typename std::multiset<Line<T>>::iterator line_iterator;

    bool compare(const T& a, const T& b) {
        return ((a < b) ^ max_query) || a == b;
    }

    bool same_slope_different_intercept(const Line<T>& a, const Line<T>& b) {
        return a.slope == b.slope && compare(a.y_intercept, b.y_intercept);
    }

    bool not_better(const line_iterator& l2)
    {
        auto l3 = std::next(l2);

        if (l2 == set.begin()) {
            if (l3 == set.end())
                return false;
            return same_slope_different_intercept(*l2, *l3);
        }

        auto l1 = std::prev(l2);
        if (l3 == set.end())
            return same_slope_different_intercept(*l2, *l1);

        return compare((l1->y_intercept - l3->y_intercept) * (l3->slope - l2->slope),
                       (l2->y_intercept - l3->y_intercept) * (l3->slope - l1->slope));
    }

    // Slopes in the set are increasing, and since
    //  queries can be increasing or decreasing,
    //  computing the minimum or the maximum,
    //  we need to start removing from the front
    //  or from the back of the set accordingly.
    // If the queries are increasing, starting
    //  from the lines with lower slopes computes
    //  the maximum, while starting with the lines
    //  with bigger slopes computes the minimum.
    // It goes the other way for the case of decreasing
    //  queries. Starting from the lines with higher
    //  slopes computes the maximum while starting with
    //  the lines with lower slopes computes the minimum.

    line_iterator first() const {
        if (increasing_queries == max_query)
            return set.begin();
        return --set.end();
    }

    line_iterator second() const {
        if (increasing_queries == max_query)
            return std::next(first());
        return std::prev(first());
    }

public:

    void add_line(const Line<T>& line)
    {
        // This case is different that the cases of the monotonic insertions.
        // Since we have no restriction on the order of the insertions, we
        //  can't add lines to one end of a deque, rather, we need insert
        //  lines dynamically. We're going to use std::multiset for this.

        // We need to insert the line
        //  first to know its position.
        auto current = set.insert(line);
        if (not_better(current)) {
            set.erase(current);
            return;
        }

        // Since the line is not being pushed at
        //  the back of a deque anymore, rather, is
        //  inserted in a set, it may have lines
        //  before it and lines after it, thus, we
        //  need to check the lines from both ends
        //  (before and after the current line).

        auto next = std::next(current);
        while (next != set.end() && not_better(next))
            set.erase(next), next = std::next(current);

        while (current != set.begin() && not_better(std::prev(current)))
            set.erase(std::prev(current));
    }

    const Line<T>& query_line(const T& x)
    {
        // This is similar to the monotonic insertions and queries case.
        while (set.size() > 1 && !compare(first()->evaluate(x), second()->evaluate(x)))
            set.erase(first());
        return *first();
    }

    T query(const T& x)
    {
        return query_line(x).evaluate(x);
    }
};

template <bool max_query, bool increasing_queries>
void test(const std::vector<Line<int>>& lines, const std::vector<int>& queries)
{
    ConvexHullTrick<int, max_query, increasing_queries> ch;

    for (auto& line : lines)
        ch.add_line(line);

    for (int x : queries) {
        auto result = ch.query_line(x);
        std::cout << "Optimal value at x = " << x << " is ";
        std::cout << result.evaluate(x) << " (from line " ;
        std::cout << result.to_string() << ")" << std::endl;
    }

    std::cout << std::endl;
}

std::vector<Line<int>> get_sample_lines_max()
{
    return {{0, 4}, {-2, -1}, {2, -3}, {-1, 4}, {-2, -1}, {1, 2}};
}

std::vector<Line<int>> get_sample_lines_min()
{
    return {{-2, 3}, {2, 1}, {-1, -2}, {1, -4}, {0, -4}, {-1, -2}, {-2, 3}};
}


std::vector<int> get_sample_queries_increasing()
{
    return {-8, -5, -2, -1, 0, 1, 2, 3, 5, 8};
}

std::vector<int> get_sample_queries_decreasing()
{
    return {8, 5, 3, 2, 1, 0, -1, -2, -5, -8};
}

int main()
{
    test<true, true>(get_sample_lines_max(), get_sample_queries_increasing());
    test<true, false>(get_sample_lines_max(), get_sample_queries_decreasing());
    test<false, true>(get_sample_lines_min(), get_sample_queries_increasing());
    test<false, false>(get_sample_lines_min(), get_sample_queries_decreasing());
}