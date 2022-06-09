#include <iostream>
#include <vector>
#include <tuple>

struct Range
{
    long long start;
    long long end;

    long long size() const { return end - start + 1; }
};

template <typename T>
struct ExtendedGCDResult
{
    T GCD, x, y;
    // GCD = GCD(a, b) = x * a + y * b
};

template <typename T>
struct LinearDiophantineResult
{
    bool has_solutions = true;
    std::vector<T> xs;
    std::vector<T> ys;
};

template <typename T>
ExtendedGCDResult<T> extended_GCD(T a, T b)
{
    T x = 1, prev_x = 0;
    T y = 0, prev_y = 1;

    while (b != 0)
    {
        T q = a / b;
        std::tie(x, prev_x) = std::make_tuple(prev_x, x - q * prev_x);
        std::tie(y, prev_y) = std::make_tuple(prev_y, y - q * prev_y);
        std::tie(a, b) = std::make_tuple(b, a - q * b);
    }

    return {a, x, y};
}

template <typename T>
LinearDiophantineResult<T> linear_diophantine_equation(const T& a, const T& b, const T& c, const Range& range)
{
    auto answer = extended_GCD(a, b);

    if (c % answer.GCD != 0)
        return {false};

    auto n = range.size();
    LinearDiophantineResult<T> result {
        true,
        std::vector<T>(n),
        std::vector<T>(n)
    };

    // We're using Bezout's identity here.
    // Let's take a look at dx. dy is the same.
    // Notice that unlike the base solution,
    //  dx is not multiplied by (c / GCD(a, b)),
    //  we keep on adding (b / GCD(a, b)) only.
    //  Think of the example where a=5, b=100, c=10:
    //  5 consecutive solutions for 5x - 100y = 5 (5 is GCD(a, b)) are:
    //   x = {1, 21, 41, 61, 81}, y = {0, 1, 2, 3, 4}
    //  5 consecutive solutions for 5x - 100y = 10 (10 is c) are:
    //   x = {2, 22, 42, 62, 82}, y = {0, 1, 2, 3, 4}
    //  Notice that the base got multiplied by 2, which is
    //   (c / GCD(a, b)), but dx remained the same.
    //  The reason is that for every step in x (20 in the
    //   example above), we have a step in y (1 in the example
    //   above) that can cancel it, and it's not affected by
    //   the multiplication.
    auto dy         = a / answer.GCD;
    auto dx         = b / answer.GCD;
    auto multiplier = c / answer.GCD;

    // Base solutions that we'll keep adding
    //  dx and subtracting dy from.
    // We add dx * range.start to the base answer
    //  before multiplying. At the end of the loop,
    //  the last element would have had dx * range.end
    //  added to it. Same goes for dy.
    result.xs[0] = (answer.x + dx * range.start) * multiplier;
    result.ys[0] = (answer.y - dy * range.start) * multiplier;
    for (size_t i = 1; i < n; i++) {
        result.xs[i] = result.xs[i - 1] + dx;
        result.ys[i] = result.ys[i - 1] - dy;
    }

    return result;
}

template <typename T>
LinearDiophantineResult<T> linear_diophantine_equation(const T& a, const T& b, const T& c, size_t answers_count)
{
    return linear_diophantine_equation(a, b, c, {0, answers_count - 1});
}

void test(int a, int b, int c, const Range& range)
{
    auto answer = linear_diophantine_equation(a, b, c, range);

    if (!answer.has_solutions) {
//        std::cout << "There are no solutions for a=" << a;
//        std::cout << " and b=" << b << " and c=" << c << std::endl;
        return;
    }

    size_t n = range.size();
    for (int i = 0; i < n; i++)
        if (answer.xs[i] * a + answer.ys[i] * b != c)
            std::cout << "x and y are not correct" << std::endl;
}

int main()
{
    int n = 100;
    for (int i = 0; i < n; i++)
        for (int j = 1; j < n; j++)
            for (int k = 0; k < n; k++) {
                test(i, j, k, {-5, 5});
                test(i, -j, k, {-5, 5});
                test(-i, j, k, {-5, 5});
                test(-i, -j, k, {-5, 5});
            }
}