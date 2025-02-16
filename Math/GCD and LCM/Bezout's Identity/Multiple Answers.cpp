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
struct BezoutsIdentityResult
{
    T GCD;
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
BezoutsIdentityResult<T> bezouts_identity(const T& a, const T& b, const Range& range)
{
    /*
     * GCD = x * a + y * b
     * Notice that for some number k, if we add b/k to x,
     * and subtract a/k from y, the equation won't change.
     *  Proof: (x + b/k) * a + (y - a/k) * b
     *         = x * a + ab/k + y * b - ab/k
     *         = x * a + y * b
     * Note that you can subtract from x instead of adding,
     *  but you'll have to add to y. If you want to do so,
     *  swapping the arguments a and b will do the job.
     * Note that to have an integer x and y, the smallest
     *  k = GCD(a, b).
     * If you want multiple answers, you can just keep adding
     *  or subtracting multiples of the GCD.
     */
    auto answer = extended_GCD(a, b);

    auto n = range.size();

    BezoutsIdentityResult<T> result {
            answer.GCD,
            std::vector<T>(n),
            std::vector<T>(n)
    };

    const T dx = b / answer.GCD;
    const T dy = a / answer.GCD;

    auto k = range.start;
    for (int i = 0; i < n; i++) {
        result.xs[i] = answer.x + dx * k;
        result.ys[i] = answer.y - dy * k;
    }

    return result;
}

template <typename T>
BezoutsIdentityResult<T> bezouts_identity(const T& a, const T& b, size_t answers_count)
{
    return bezouts_identity(a, b, {0, answers_count - 1});
}

void test(int a, int b, const Range& range)
{
    auto answer = bezouts_identity(a, b, range);

    if (a % answer.GCD != 0 || b % answer.GCD != 0)
        std::cout << "GCD is not correct" << std::endl;

    int n = range.size();
    for (int i = 0; i < n; i++)
        if (answer.xs[i] * a + answer.ys[i] * b != answer.GCD)
            std::cout << "x and y are not correct" << std::endl;
}

int main()
{
    int n = 500;
    for (int i = 0; i < n; i++)
        for (int j = 1; j < n; j++) {
            test(i, j, {-5, 5});
            test(i, -j, {-5, 5});
            test(-i, j, {-5, 5});
            test(-i, -j, {-5, 5});
        }
}