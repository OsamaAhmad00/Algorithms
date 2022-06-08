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
     * Reason of having only GCD(a, n) solutions:
     *  After turing "ax ≡ b (mod n)" to "ax - ny = b", and solving
     *  it as a linear Diophantine equation, using Bezout's identity,
     *  we can see that the values of x will be in the form:
     *   s + (n / GCD(a, n)) * i, for some solution s, and for i
     *   in range [-infinity, infinity].
     *  Since each step is (n / GCD(a, n)), we know that after GCD(a, n)
     *  steps, we would've added (or subtracted) n, and since we're taking
     *  the mod, after adding n, the next value will already be in our
     *  solution set. Thus, we can only have GCD(a, n) unique solutions.
     */

    // xa - yn = b
    auto answer = extended_GCD(a, n);

    if (b % answer.GCD != 0)
        return {};

    // xa - yn = b. Here, since b can be a multiple of
    //  GCD(a, n), and since answer.x is a solution for
    //  xa - yn = GCD(a, n), we should multiply it  by
    //  (b / GCD(a, n)) to be a solution for xa - yn = b.
    T base = answer.x * (b / answer.GCD);
    // Making sure it's positive, and less than n.
    base = (base % n + n) % n;

    // Notice that unlike the base solution, the
    //  step is not multiplied by (b / GCD(a, n)),
    //  we keep on adding (n / GCD(a, n)) only.
    //  Think of the example where a=5, b=10, n=100:
    //  Solutions for 5x - 100y = 5 (5 is GCD(a, n)) are:
    //   x = {1, 21, 41, 61, 81}, y = {0, 1, 2, 3, 4}
    //  Solutions for 5x - 100y = 10 (10 is b) are:
    //   x = {2, 22, 42, 62, 82}, y = {0, 1, 2, 3, 4}
    //  Notice that the base got multiplied by 2, which is
    //   (b / GCD(a, n)), but the step remained the same.
    //  The reason is that for every step in x (20 in the
    //   example above), we have a step in y (1 in the example
    //   above) that can cancel it, and it's not affected by
    //   the multiplication.
    T step = std::abs(n / answer.GCD);

    // We take the mod to get the smallest
    //  solution, then keep on adding a step
    //  at a time. This has two advantages:
    //   1 - We don't have to take the mod on each
    //       iteration when computing the solutions
    //       since we know that we're starting from
    //       the smallest number, and that the biggest
    //       solution won't exceed n, as opposed to
    //       the case where the base solution is any
    //       of the solutions.
    //   2 - The returned result will be sorted
    //  Intuitively, there is a base solution
    //  that is either 0 or some number that is
    //  not divisible by "step", that we'll keep
    //  adding step to. Note that we're taking
    //  the absolute value to ensure that the
    //  solutions are increasing (if step is
    //  negative, it can be decreasing).
    base %= step;

    auto solutions = std::abs(answer.GCD);
    std::vector<T> result(solutions);

    result[0] = base;
    for (int i = 1; i < solutions; i++) {
        // We don't have to take % n here.
        result[i] = result[i - 1] + step;
    }

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

    auto sorted = result;
    std::sort(sorted.begin(), sorted.end());
    if (result != sorted)
        std::cout << "The result is not sorted" << std::endl;

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