#include <iostream>
#include <deque>
#include <vector>

// This is a solution to this problem: https://open.kattis.com/problems/coveredwalkway

template <typename T>
struct Line
{
    T slope;
    T y_intercept;

    T evaluate(const T& x) const { return slope * x + y_intercept; }
};

template <typename T, bool max_query = true, bool increasing_slopes = true>
class ConvexHullTrick
{
    // Depending on the parameters max_query, and increasing_slopes, the
    //  queries will be either decreasing or increasing.
    // ╔═══════════╤═══════════════════╤════════════╗
    // ║ max_query │ increasing_slopes │ Queries    ║
    // ╠═══════════╪═══════════════════╪════════════╣
    // ║   true    │       true        │ increasing ║
    // ╟───────────┼───────────────────┼────────────╢
    // ║   false   │       false       │ increasing ║
    // ╟───────────┼───────────────────┼────────────╢
    // ║   true    │       false       │ decreasing ║
    // ╟───────────┼───────────────────┼────────────╢
    // ║   false   │       true        │ decreasing ║
    // ╚═══════════╧═══════════════════╧════════════╝

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
        // Let's consider the case when max_query = false, increasing_slopes = false:
        // Since the queries are monotonic, specifically, increasing, we know that
        //  we won't get x values that are less than the current x value, thus,
        //  we can remove lines that won't be useful anymore in the next queries
        //  to make them faster.
        // if deque[0].evaluate(x) is not better than deque[1].evaluate(x), then
        //  we've passed the intersection point of the 2 lines, and deque[0] is no
        //  longer better than deque[1], and we can pop it.
        while (deque.size() > 1 && !compare(deque[0].evaluate(x), deque[1].evaluate(x)))
            deque.pop_front();
        return deque[0];
    }

    T query(const T& x)
    {
        return query_line(x).evaluate(x);
    }
};

struct DynamicProgramming
{
    long long c;
    const std::vector<long long>& array;
    std::vector<std::vector<long long>> memory;

    long long dp(int current, int end)
    {
        auto& result = memory[current][end];

        if (result != -1)
            return result;

        auto diff = array[end] - array[current];
        auto cost = diff * diff + c;

        if (current == 0)
            return cost;

        auto stop = dp(current - 1, current-1) + cost;
        auto no_stop = dp(current - 1, end);

        return result = std::min(stop, no_stop);
    }

public:

    long long solve() { return dp(array.size() - 1, array.size() - 1); }

    DynamicProgramming(long long c, const std::vector<long long>& array)
            : c(c), array(array), memory(array.size(), std::vector<long long>(array.size(), -1))
    {}
};

class SolutionBag
{
    struct Solution
    {
        long long start;
        long long prev_cost;

        long long evaluate(long long end) const
        {
            long long diff = end - start;
            return diff * diff + prev_cost;
        }
    };

    long long c;
    std::vector<Solution> bag;
    const std::vector<long long>& array;

public:

    SolutionBag(long long c, const std::vector<long long>& array)
            : c(c), array(array) {}

    long long solve()
    {
        long long best = 0;
        for (auto x : array)
        {
            bag.push_back({x, best + c});

            best = 1e18;
            for (auto& solution : bag)
                best = std::min(best, solution.evaluate(x));
        }
        return best;
    }
};

int main()
{
    // For a function f(y) = (x - y)^2 + c:
    //  f(y) = x^2 -2xy + y^2 + c.
    //  Note that the same x^2 will be added to all evaluations of a single line.
    //  Note that the same y^2 will be added to all evaluations at the same y.
    //  Making use of the previous 2 notes, we can add the x^2 component to the
    //   y-intercept of the line, and add the y^2 component after the evaluation
    //   of the line. This way, we'll be able to represent f(y) as a line and use
    //   the convex hull trick.
    //  To represent f(y) as a line, we can do the following:
    //   - Slope = -2x
    //   - Y-Intercept = x^2 + c
    //   - y^2 is not added to the equation of the line, rather,
    //     it's added at the end after getting the best line since
    //     it'll be the same in every evaluation of the lines.

    // We can solve this problem using dynamic programming with the following recurrence:
    //  dp[current][end] = min(dp[current-1][end], dp[current-1][current-1] + (array[end] - array[current])^2 + c)
    // Or, it can be solved using "Solution Bags", in which case, on each iteration,
    //  you check for the minimum value, and extend the partial solutions.
    // Both solutions will take O(n^2), which is slow. If you can use the convex hull trick,
    //  you'll be able to solve it in O(n).

    // The solution with the convex hull trick is very similar to the one with the solution bag,
    //  except that we're reforming the solutions to be represented as a line.

    long long n, c; std::cin >> n >> c;
    std::vector<long long> array(n);
    for (int i = 0; i < n; i++)
        std::cin >> array[i];

    ConvexHullTrick<long long, false, false> ch;
    long long best = 0;
    for (long long x : array) {
        ch.add_line({-2 * x, x * x + c + best});
        best = ch.query(x) + x * x;
    }
    std::cout << best << std::endl;

    // DynamicProgramming dp{c, array};
    // std::cout << dp.solve() << std::endl;

    // SolutionBag sb{c, array};
    // std::cout << sb.solve() << std::endl;
}
