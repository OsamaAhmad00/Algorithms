#include <iostream>
#include <deque>
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

    std::string to_string() const {
        return std::to_string(slope) + "x + " + std::to_string(y_intercept);
    }
};

template <typename T, bool max_query = true, bool increasing_slopes = true>
class ConvexHullTrick
{
    // Note that negating the slope has the effect of mirroring lines about the Y-axis. You can
    //  do this if the slopes are sorted in the reverse order.
    // Also, note that negating both slope and Y-intersect has the effect of mirroring about the
    //  X-axis. You can do this if you want to query the minimum/maximum instead of the maximum/minimum.

    std::deque<Line<T>> deque;

    bool compare(const T& a, const T& b) {
        return (a < b) ^ max_query;
    }

    bool is_better(const Line<T>& line)
    {
        auto n = deque.size();

        if (n < 2) return false;

        const Line<T> &l1 = line, &l2 = deque[n - 1], &l3 = deque[n - 2];

        // Here, we need to make sure that there are no equal
        //  lines in the deque for the binary search to work.
        if (l1 == l2) return true;

        // Let's consider the case when max_query = false, increasing_slopes = false:
        //  1 - Since this assumes that the insertions of the lines are monotonic,
        //       specifically, the slopes are decreasing, meaning that lines with bigger
        //       slopes are added first, we get that l1.slope <= l2.slope <= l3.slope.
        //  2 - Let x13 = the intersection point of l1 and l3, x23 = the intersection
        //      point of l2 and l3, since l1.slope <= l2.slope, if x13 <= x23, meaning
        //      that l1 intersects with l3 before l2 intersects with it, then l1 is better
        //      than l2 everywhere, in which case, we don't need l2 anymore.
        //  3 - We can check the intersection point of l1 and l3 (x13) by equating the
        //      equations of the 2 lines:
        //       x13 * l3.slope + l3.y_intercept = x13 * l1.slope + l1.y_intercept
        //       x13 * l3.slope - x13 * l1.slope = l1.y_intercept - l3.y_intercept
        //       x13 * (l3.slope - l1.slope) = l1.y_intercept - l3.y_intercept
        //       x13 = (l1.y_intercept - l3.y_intercept) / (l3.slope - l1.slope)
        //      Similarly, we can calculate x23 to be (l2.y_intercept - l3.y_intercept) / (l3.slope - l2.slope).
        //  We need to check if x13 <= x23, and for this, we can evaluate the 2 expressions
        //   above, but this will involve using doubles, which we would prefer to avoid if we can.
        //  We can observe that since l1.slope <= l2.slope <= l3.slope, we can multiply both
        //   sides of the equality by (l3.slope - l1.slope)(l3.slope - l2.slope), and not change
        //   the inequality (since it's positive). This way, we don't need to use doubles at all.
        //  Note that if the slopes are increasing, meaning that l1.slope >= l2.slope >= l3.slope,
        //   (l3.slope - l1.slope)(l3.slope - l2.slope) would still be positive, thus, we don't
        //   need to change the inequality in both cases.
        // The inequality will change based on the value off max_query and increasing_slopes.

        bool result = (l1.y_intercept - l3.y_intercept) * (l3.slope - l2.slope) <=
                      (l2.y_intercept - l3.y_intercept) * (l3.slope - l1.slope);

        return result ^ increasing_slopes ^ max_query;
    }

public:

    void add_line(const Line<T>& line)
    {
        // Pop back while "line" gives better
        //  results for all points than the
        //  last line in the deque.
        while (is_better(line))
            deque.pop_back();
        deque.push_back(line);
    }

    const Line<T>& query_line(const T& x)
    {
        // Assuming that there exist no equal lines in the deque,
        //  this is the only method that will change from the code
        //  for the monotonic queries. If the code for monotonic
        //  queries doesn't guarantee that there will be no equal
        //  lines in the deque, then the add_line method will be
        //  modified too.
        // Because we have no guarantee on the order of the
        //  queries, we can't remove lines from the front of
        //  the deque.
        // We can make use of the fact that the lines are sorted,
        //  and binary search for the best line at the point x.

        int left = 0, right = deque.size() - 1;
        while (left < right)
        {
            int mid = left + (right - left) / 2;
            if (mid + 1 <= right && compare(deque[mid+1].evaluate(x), deque[mid].evaluate(x)))
                left = mid + 1;
            else if (mid - 1 >= left && compare(deque[mid-1].evaluate(x), deque[mid].evaluate(x)))
                right = mid - 1;
            else
                return deque[mid];
        }
        return deque[left];
    }

    T query(const T& x)
    {
        return query_line(x).evaluate(x);
    }
};

template <bool max_query, bool increasing_slopes>
void test(const std::vector<Line<int>>& lines, const std::vector<int>& queries)
{
    ConvexHullTrick<int, max_query, increasing_slopes> ch;

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

std::vector<Line<int>> get_sample_lines_max_increasing()
{
    return {{-2, -1}, {-1, 4}, {0, 4}, {1, 2}, {2, -3}};
}

std::vector<Line<int>> get_sample_lines_max_decreasing()
{
    return {{2, -3}, {1, 2}, {0, 4}, {-1, 4}, {-2, -1}};
}

std::vector<Line<int>> get_sample_lines_min_increasing()
{
    return {{-2, 3}, {-1, -2}, {0, -4}, {0, -4}, {0, -4}, {0, -4},
            {0, -4}, { 0, -4}, {0, -4}, {0, -4}, {1, -4}, {2,  1}};
}

std::vector<Line<int>> get_sample_lines_min_decreasing()
{
    return {{2, 1}, {1, -4}, {0, -4}, {-1, -2}, {-2, 3}};
}

std::vector<int> get_sample_queries()
{
    return {-8, 8, 5, -5, -2, -1, 3, 1, 2, 0};
}

int main()
{
    test<true, true>(get_sample_lines_max_increasing(), get_sample_queries());
    test<true, false>(get_sample_lines_max_decreasing(), get_sample_queries());
    test<false, true>(get_sample_lines_min_increasing(), get_sample_queries());
    test<false, false>(get_sample_lines_min_decreasing(), get_sample_queries());
}