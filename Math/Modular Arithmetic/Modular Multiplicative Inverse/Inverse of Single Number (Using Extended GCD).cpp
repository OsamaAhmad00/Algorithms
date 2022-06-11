#include <iostream>
#include <tuple>

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

template <typename T>
T mod_multiplicative_inverse(const T& number, const T& mod)
{
    /*
     * Returns 0 if there exist no multiplicative inverse
     *  or if number = 0 and mod = 1. Otherwise, Returns a
     *  positive modular multiplicative inverse for the number.
     *
     * Let a = number, n = mod. ax ≡ 1 (mod n) is equivalent
     *  to ax - qn = 1 for some integers x and q.
     *
     * Fact: If abs(GCD(number, mod)) != 1, there exists no inverse.
     *  Proof:
     *   let g = GCD(a, n), a = gA, n = gN where A = a/g, N = n/g.
     *   ax - qn = 1 is equivalent to gAx - gqN = 1 = g(Ax - qN).
     *   This implies that g must divide 1, and the only integers
     *   that divide 1 are 1 and -1.
     *
     * We're looking for a solution for the equation "ax - qn = 1"
     *  which can be written as "ax - qn = GCD(a, n)". Notice that
     *  the extended Euclidean algorithm is perfect here.
     *
     * In modular arithmetic, you can't just divide by a number.
     *  If you want to divide by a number x, you have to multiply
     *  by the modular multiplicative inverse of x. This is because
     *  not every number has an inverse, thus, you might be dividing
     *  by a number that has no inverse. BUT, if you know that x has
     *  an inverse under the mod, you can divide by x.
     * Remember that a number x has an inverse if GCD(x, mod) = 1. This
     *  means that if mod is a prime number, then you can divide by any
     *  number less than the mod.
     * Take a look at https://qr.ae/pvHGez.
     */

    auto result = extended_GCD((number + mod) % mod, mod);
    // result.x and result.GCD can be negative.
    return ((result.x + mod) % mod) * (std::abs(result.GCD) == 1);
}

template <typename T>
T GCD(T a, T b)
{
    while (a != 0) {
        b %= a;
        std::swap(a, b);
    }

    return b;
}

void test(int number, int mod)
{
    int inverse = mod_multiplicative_inverse(number, mod);

    if (inverse == 0) {
        if (std::abs(GCD(number, mod)) == 1 && mod != 1)
            std::cout << "Inverse does actually exist" << std::endl;
        return;
    }

    if (((number * inverse) % mod + mod) % mod != 1)
        std::cout << "The inverse is incorrect" << std::endl;
}

int main()
{
    int n = 1000;
    for (int mod = 1; mod <= n; mod++) {
        for (int number = 0; number < mod; number++) {
            test(number, mod);
            test(-number, mod);
        }
    }
}