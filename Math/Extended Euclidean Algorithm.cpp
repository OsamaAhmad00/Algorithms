#include <iostream>
#include <tuple>

template <typename T>
struct ExtendedGCDResult
{
    T g, x, y;

    // g = x * a + y * b
    //  where g = GCD(a, b)
};

template <typename T>
ExtendedGCDResult<T> extended_GCD_recursive(const T& a, const T& b)
{
    // Base case:
    //  if b is 0, then gcd = 1 * a + 0 * b = a.
    if (b == 0)
        return {a, 1, 0};

    /*
     * Example:
     *  a = 32, b = 219
     *  Note that in this example, b is bigger than a.
     *   I set it in this way on purpose to show that
     *   it doesn't matter which one is bigger than the other.
     *
     *  let's agree to represent b % a as b - floor(b / a) * a
     *  #)  x   *   a   -  y   *   b   =  Mod
     *  1)  1   *   32  -  0   *   219 =  32
     *  2)  1   *   219 -  6   *   32  =  27
     *  3)  1   *   32  -  1   *   27  =  5
     *  4)  1   *   27  -  5   *   5   =  2
     *  5)  1   *   5   -  2   *   2   =  1
     *  6)  1   *   2   -  2   *   1   =  0    # Note that this equation is
     *                                            not being used. It just indicates
     *                                            that prev Mod = GCD.
     *
     *  Observations up until now:
     *   1 - x is always 1
     *   2 - In each step, a = prev b
     *   4 - In each step, y = floor(a / b)
     *   3 - In each step, b = prev Mod
     *   5 - The GCD is the Mod of the equation right
     *        before the last one (equation number 5)
     *
     *  Here, we ended up with GCD (1) = 1 * 5 - 2 * 2. This is good, but
     *   we need to represent the GCD in terms of a and b.
     *  We can see that in each step, b = prev Mod. We can use this fact
     *   to substitute each equation in the equation below it, one by one,
     *   starting from the bottom equation (in which Mod = GCD), until we
     *   reach the first equation, which is represented in terms of a and b.
     *
     *  Continuing the example:                           This is the prev mod, which is the
     *                                                     result of the previous equation.
     *                                                                        v
     *  current equation = Initial equation = The equation
     *   of the GCD (equation 5) = 1 * 5 - 2 * 2 = 1.
     *
     *  Substituting the previous equation (number 4) in place of b:
     *   1 * 5 - 2 * (1 * 27 - 5 * 5) = 1
     *   = 1 * 5 + 10 * 5 - 2 * 1 * 27 = 1
     *   = (1 + 10) * 5 - 2 * 27 = 1
     *   = 11 * 5 - 2 * 27 = 1
     *  Rearranging the terms such that (x, a, y, b) = (y, b, x, a):
     *   current equation = -2 * 27 + 11 * 5 = 1
     *
     *  Note that we swapped x and a with y and b to let the
     *   algorithm do the same steps each time.
     *
     *  Substituting the previous equation (number 3) in place of b:
     *   -2 * 27 + 11 * (1 * 32 - 1 * 27) = 1
     *   = -13 * 27 + 11 * 32 = 1
     *  Rearranging the terms such that (x, a, y, b) = (y, b, x, a):
     *   current equation = 11 * 32 - 13 * 27 = 1
     *
     *  Substituting the previous equation (number 2) in place of b:
     *   11 * 32 - 13 * (1 * 219 - 6 * 32) = 1
     *   = 89 * 32 - 13 * 219 = 1
     *  Rearranging the terms such that (x, a, y, b) = (y, b, x, a):
     *   current equation = -13 * 219 + 89 * 32 = 1
     *
     *  Substituting the previous equation (number 2) in place of b:
     *   -13 * 219 + 89 * (1 * 32 - 0 * 219) = 1
     *   = -13 * 219 + 89 * 32
     *  Rearranging the terms such that (x, a, y, b) = (y, b, x, a):
     *   current equation = 89 * 32 - 13 * 219 = 1
     *
     *  Final result: x = 89, a = 32, y = -13, b = 219.
     *
     *  Note here that we didn't need to keep track of the
     *   intermediate a and b except for computing floor(a / b)
     *
     *  How to compute this algorithmically?
     *   We can see that when substituting the equations back,
     *   the following happens:
     *    1 - x = y from the equation below
     *    2 - y = x from the equation below - (a / b) * y from the equation below
     *   This is a good place to use recursion.
     */

    auto result = extended_GCD_recursive(b, a % b);

    // C++ way of saying
    // (result.x, result.y) = (result.y, result.x - (a / b) * result.y)
    std::tie(result.x, result.y) = std::make_tuple(
        result.y,
        result.x - (a / b) * result.y
    );

    return result;
}

template <typename T>
ExtendedGCDResult<T> extended_GCD_iterative(T a, T b)
{
    T x = 1, prev_x = 0;
    T y = 0, prev_y = 1;

    while (b != 0)
    {
        T q = a / b; // q = floor(a / b)

        // Note that the next two lines are identical.
        std::tie(x, prev_x) = std::make_tuple(prev_x, x - q * prev_x);
        std::tie(y, prev_y) = std::make_tuple(prev_y, y - q * prev_y);

        // Note that here, the intermediate values of a and b
        //  are the same for the values of the normal iterative
        //  Euclidean algorithm for computing the GCD, so, we
        //  know that at least this algorithm computes a correct GCD.
        std::tie(a, b) = std::make_tuple(b, a - q * b);
    }

    /*
     * This might not be very intuitive. To see that this algorithm
     *  computes the correct coefficients, you can confirm that the
     *  following invariants hold at any time, before the loop, in
     *  the loop, and after the loop:
     *   Let A = the original value of a that is passed as an argument.
     *   Let B = the original value of b that is passed as an argument.
     *   1 - x      * A + y      * B = a
     *   2 - prev_x * A + prev_y * B = b
     *  Since after the loop, a will hold the GCD, and since the invariant
     *   number 1 holds, then we know that x * A + y * B = GCD, which means
     *   that x and y are correct coefficients.
     */

    return {a, x, y};
}

void test(int a, int b)
{
    auto result1 = extended_GCD_recursive(a, b);
    auto result2 = extended_GCD_iterative(a, b);
    if (result1.g != result2.g || result1.x != result2.x || result1.y != result2.y)
        std::cout << "The recursive and the iterative versions don't give the same answers" << std::endl;
    if (a % result1.g != 0 || b % result1.g != 0)
        std::cout << "GCD is not correct" << std::endl;
    if (result1.x * a + result1.y * b != result1.g)
        std::cout << "x and y are not correct" << std::endl;
}

int main()
{
    int n = 1000;
    for (int i = 0; i < n; i++)
        for (int j = 1; j < n; j++)
            test(i, j);
}