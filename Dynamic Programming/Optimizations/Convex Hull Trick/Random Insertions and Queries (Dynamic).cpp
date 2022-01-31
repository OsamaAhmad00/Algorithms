#include <iostream>
#include <set>
#include <vector>
#include <functional>

template <typename T>
struct Line
{
    T slope;
    T y_intercept;

    static const T query_value = INT_MAX;
    mutable std::function<bool(const Line&)> process_query;

    T evaluate(const T& x) const { return x * slope + y_intercept; };

    bool is_query(const Line& line) const { return line.slope == query_value; }

    bool operator==(const Line& line) const {
        return slope == line.slope && y_intercept == line.y_intercept;
    }

    bool operator<(const Line& line) const
    {
        // Ideally, we would be using a custom self-balancing binary tree
        //  that allows us to traverse it. If that was the case, we would
        //  have been able to have two methods, one for insertions, and
        //  the other for queries.
        // Since we're using an std::multiset, which doesn't provide the
        //  ability to traverse the tree, we'll perform a hack here.
        // We'll make a special value called "query_value", and if the
        //  slope of the line = query_value, then we know that this is
        //  not a comparison, rather a query, in which x = line.y_intercept.
        // This is almost the only difference between this case and the
        //  random insertions and monotonic queries case.
        if (is_query(line))
            return process_query(line);
        return slope < line.slope;
    }

    std::string to_string() const {
        return std::to_string(slope) + "x + " + std::to_string(y_intercept);
    }
};

template <typename T, bool max_query = true>
class ConvexHullTrick
{
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

    std::function<bool(const Line<T>&)> create_query_functor(const line_iterator& current)
    {
        return [current, this](const Line<T>& query)
        {
            auto successor = std::next(current);
            if (successor == set.end())
                return false;

            const T& x = query.y_intercept;
            return compare((successor->slope - current->slope) * x,
                           current->y_intercept - successor->y_intercept);
        };
    }

public:

    void add_line(const Line<T>& line)
    {
        // This similar to the case of the random insertions
        //  and monotonic queries, except that we associate
        //  a functor for processing queries with each line.

        auto current = set.insert(line);
        if (not_better(current)) {
            set.erase(current);
            return;
        }

        current->process_query = create_query_functor(current);

        auto next = std::next(current);
        while (next != set.end() && not_better(next))
            set.erase(next), next = std::next(current);

        while (current != set.begin() && not_better(std::prev(current)))
            set.erase(std::prev(current));
    }

    const Line<T>& query_line(const T& x)
    {
        // Here, we're relying on the functor associated
        //  with the lines.
        return *set.lower_bound({Line<T>::query_value, x});
    }

    T query(const T& x)
    {
        return query_line(x).evaluate(x);
    }
};

template <bool max_query>
void test(const std::vector<Line<int>>& lines, const std::vector<int>& queries)
{
    ConvexHullTrick<int, max_query> ch;

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

std::vector<int> get_sample_queries()
{
    return {-8, 8, 5, -5, -2, -1, 3, 1, 2, 0};
}

int main()
{
    test<true >(get_sample_lines_max(), get_sample_queries());
    test<false>(get_sample_lines_min(), get_sample_queries());
}