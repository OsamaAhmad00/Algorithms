#include <iostream>
#include <tuple>

template <typename T>
struct ExtendedGCDResult
{
    T GCD, x, y;
    // GCD = GCD(a, b) = x * a + y * b
};

template <typename T>
struct LinearDiophantineResult
{
    // This is set to false when there are no solutions.
    bool has_solutions = true;

    // for an input a, b, and c,
    //  x * a + y * b = c
    // c has to be divisible by GCD(a, b).
    T x, y;
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
LinearDiophantineResult<T> linear_diophantine_equation(const T& a, const T& b, const T& c)
{
    /*
     * Given a, b, and c, this function returns {x, y}
     *  such that x * a + y * b = c.
     * This is based on a very simple observation:
     *  The extended Euclidean algorithm already figures
     *  out {x, y} for the equation x * a + y * b = GCD(a, b).
     *  if c is divisible by GCD(a, b), then we can just
     *  multiply {x, y} by c/GCD(a, b), and get a correct solution.
     * Note that if c is not divisible by GCD(a, b), then there
     *  are no answers.
     */
    auto answer = extended_GCD(a, b);

    if (c % answer.GCD != 0)
        return {false};

    auto q = c / answer.GCD;

    return {true, answer.x * q, answer.y * q};
}

void test(int a, int b, int c)
{
    auto result = linear_diophantine_equation(a, b, c);
    if (!result.has_solutions) {
//        std::cout << "There are no solutions for a=" << a;
//        std::cout << " and b=" << b << " and c=" << c << std::endl;
    } else if (result.x * a + result.y * b != c) {
        std::cout << "x and y are not correct" << std::endl;
    }
}

int main()
{
    int n = 100;
    for (int i = 0; i < n; i++)
        for (int j = 1; j < n; j++)
            for (int k = 0; k < n; k++)
                test(i, j, k), test(i, -j, k), test(-i, j, k), test(-i, -j, k);
}