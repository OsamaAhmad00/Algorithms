#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>

template <typename T>
struct ExtendedGCDResult
{
    T GCD, x, y;
    // GCD = GCD(a, b) = x * a + y * b
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

// Returns all solutions to the equation: ax ≡ b (mod n)
template <typename T>
std::vector<T> linear_modular_equation(const T& a, const T& b, const T& n)
{
    /*
     * Linear Diophantine Equation is an equation in the form:
     *   ax + by = c (where c is divisible by GCD(a, b)).
     *  We already know how to get solutions for the linear Diophantine equation.
     * Linear Modular Equation is an equation in the form:
     *   ax ≡ b (mod n)
     *  For now, we're going to use different symbols for demonstration purpose.
     *   ax ≡ c (mod b)
     *  Which can be written as:
     *     ax = c - by (for some integer y)
     *   = ax + by = c
     * We can see that it has the exact same structure
     *  as the linear Diophantine equation. We can make
     *  use of this observation, and turn the linear
     *  modular equation to a linear Diophantine equation,
     *  solve it, and turn the only return the xs part of the
     *  solution (ignoring the ys).
     * Note that a linear Diophantine equation can have no,
     *  or infinite solutions, but for a modular equation (mod n),
     *  a solution x must be: -n < x < n.
     *  This means that we have a limited number of solutions,
     *  specifically, GCD(a, n) number of solutions.
     * You might think that if b = 0, the only solution is x = 0, but
     *  Notice that multiplying a with a multiple of n / GCD(a, n)
     *  will set it to 0 (mod n).
     */

    // xa - yn = b
    auto answer = extended_GCD(a, n);

    if (b % answer.GCD != 0)
        return {};

    T base = answer.x * (b / answer.GCD);
    base = (base % n + n) % n;

    auto solutions = std::abs(answer.GCD);
    std::vector<T> result(solutions);

    T step = n / answer.GCD;
    for (int i = 0; i < solutions; i++)
        result[i] = (base + i * step) % n;

    return result;
}

void test(int a, int b, int n)
{
    auto result = linear_modular_equation(a, b, n);

    if (result.empty()) {
//        std::cout << "There are no solutions for a=" << a;
//        std::cout << ", b=" << b << ", n=" << n << std::endl;
        return;
    }

    std::sort(result.begin(), result.end());
    for (int i = 1; i < result.size(); i++) {
        if (result[i - 1] == result[i]) {
            std::cout << "The xs are not unique" << std::endl;
            break;
        }
    }

    for (int x : result) {
        if (((a * x) % n + n) % n != (b + n) % n) {
            std::cout << "The xs are not correct" << std::endl;
            break;
        }
    }
}

int main()
{
    int count = 100;
    for (int n = 1; n <= count; n++)
        for (int a = 0; a < n; a++)
            for (int b = 0; b < n; b++) {
                test(a, b, n);
                test(-a, b, n);
                test(a, -b, n);
                test(-a, -b, n);
            }
}