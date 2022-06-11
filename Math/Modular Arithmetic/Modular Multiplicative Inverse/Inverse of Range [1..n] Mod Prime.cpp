#include <iostream>
#include <vector>
#include <cmath>
#include <tuple>

template <typename T>
struct ExtendedGCDResult
{
    T GCD, x, y;
    // GCD = GCD(a, b) = x * a + y * b
};

template <typename T>
std::vector<T> range_prime_mod_inverse(const T& mod, size_t upperbound)
{
    // Computes all inverses for numbers in range [1..upperbound] (mod mod)

    // UPPERBOUND MUST BE < MOD.
    // MOD MUST BE PRIME. This is to ensure that every number has an inverse
    //  and to ensure that the calculations are correct.

    // inverses[x] = inverse of x (mod mod)
    std::vector<T> inverses(upperbound + 1);

    // Base case.
    if (upperbound >= 1)
        inverses[1] = 1;

    /*
     * This comes from a simple observation:
     *
     *  For a number p = mod, number i < mod:
     *   p % i = p - p/i * i
     *
     *  Taking % p for both sides (Note that (p % i) % p = p % i):
     *   p % i = (p - p/i * i) % p
     *   p % i = 0 - (p/i * i) % p
     *
     *  Dividing by (i * p%i):
     *   1/i = (-p/i * 1/(p%i)) % p
     *   inverse[i] = (-p/i * inverse[p%i]) % p
     *
     *  Turning the result to a positive number (adding p, and taking % p):
     *   inverse[i] = ((-p/i * inverse[p%i]) % p + p) % p
     *
     *  Base case: inverse(1) = 1.
     *  Otherwise, compute the above recurrence.
     *
     *  We can use dynamic programming here. Create an array called
     *  "inverses", and keep on building starting from i = 2.
     *
     *  Notice that the expression "p % i" would always be < i, thus,
     *   you're only relying on the values you've already computed.
     *   Also, notice that "p % i" will never be 0 since p is prime,
     *   except for the base case, which we have set by hand.
     *
     *  Notice that since the mod is prime, we can be sure that for
     *   any number x < mod, 1/x = the modular multiplicative inverse
     *   of x, in other words, we can divide by any number < the mod
     *   and the computations will remain correct.
     *
     * Another way to look at it:
     *
     *  Remember that we can use the extended Euclidean algorithm
     *  which solves the equation "ax + by = GCD(a, b)" to compute
     *  the modular multiplicative inverse. We can pass a=i, and b=mod,
     *  and the value of x would be the inverse.
     *
     *  A quick refresher on the recursive extended Euclidean algorithm:
     *   Base case:
     *    if a == 0, return x = 0, y = 1.
     *   Otherwise:
     *    prev = extended_GCD(b % a, a)
     *    Return {
     *       x = prev.y - (b / a) * prev.x
     *       y = prev.x
     *    }
     *
     *  Here, we only care about the x value, we don't care about the y value.
     *  We can use dynamic programming here. Create an array called "inverses",
     *   set inverses[1] = 1 (since extended_GCD(1, mod).x = 1). All that is
     *   left to do is to compute the recurrence "prev.y - (b / a) * prev.x".
     *  Notice that in the recurrence above:
     *   - a corresponds to the number you're computing the inverse for.
     *   - b corresponds to the mod
     *   - prev.x corresponds to inverses[mod % i]
     *   - prev.y = b * prev.x = mod * inverses[mod % i]
     *      TODO: why prev.y = b * prev.x?
     *   Thus, all you have to do is, for each number i, set inverses[i]
     *   = (mod * inverses[mod % i] - mod/i * inverses[mod % i]) % mod
     *   = ((mod - mod/i) * inverses[mod % i]) % mod.
     */

    for (size_t i = 2; i <= upperbound; i++)
    {
        // Notice that since i is an unsigned type, the expression number 1
        //  is correct, and the expression 2 is not correct.
        // 1 - inverses[i] = (-(mod/i * inverses[mod % i] % mod) + mod) % mod;
        // 2 - inverses[i] = (- mod/i * inverses[mod % i] % mod  + mod) % mod;

        // A better way:
        inverses[i] = ((mod - mod/i) * inverses[mod % i]) % mod;
    }

    return inverses;
}

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
    auto result = extended_GCD((number + mod) % mod, mod);
    return ((result.x + mod) % mod) * (std::abs(result.GCD) == 1);
}

template <typename T>
bool is_prime(const T& number)
{
    if (number != 2 && number % 2 == 0)
        return false;

    T s = std::sqrt(std::abs(number));
    for (T i = 3; i <= s; i+=2)
        if (number % i == 0)
            return false;
    return true;
}

void test(int mod, size_t upperbound)
{
    auto inverses = range_prime_mod_inverse(mod, upperbound);
    for (int i = 1; i <= upperbound; i++) {
        auto inverse = mod_multiplicative_inverse(i, mod);
        if (inverses[i] != inverse)
            std::cout << "The result is not correct" << std::endl;
    }
}

int main()
{
    int n = 10000;
    for (int mod = 1; mod <= n; mod++)
        if (is_prime(mod))
            test(mod, mod - 1);
}