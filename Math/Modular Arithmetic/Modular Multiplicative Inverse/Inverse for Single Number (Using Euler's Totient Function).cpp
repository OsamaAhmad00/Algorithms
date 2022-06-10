#include <iostream>

template <typename T>
T GCD(T a, T b)
{
    while (a != 0) {
        b %= a;
        std::swap(a, b);
    }

    return b;
}

template <typename T, typename U, typename V>
T fast_mod_power(T number, U power, V mod)
{
    T result = 1;
    while (power > 0)
    {
        if (power % 2 != 0)
            result = (result * number) % mod;
        number = (number * number) % mod;
        power /= 2;
    }

    // Returns 1 for 0^0 % 1 which is not correct.
    //  You can take the mode when returning the
    //  answer, but it's not worth it since you'll
    //  never use this function to compute 0^0 % 1.
    return result;
}

template <typename T>
T phi_prime_in_number(T& number, const T& prime)
{
    T power_of_prime = 1;
    while (number % prime == 0) {
        power_of_prime *= prime;
        number /= prime;
    }

    return (power_of_prime / prime) * (prime - 1);
}

template <typename T>
T phi(T number)
{
    T result = (number > 1);

    if (number % 2 == 0)
        result *= phi_prime_in_number(number, 2);

    for (T i = 3; i * i <= number; i+=2)
        if (number % i == 0)
            result *= phi_prime_in_number(number, i);

    if (number != 1)
        result *= phi_prime_in_number(number, (T)number);

    return result;
}

template <typename T>
T mod_multiplicative_inverse(const T& number, const T& mod)
{
    /*
     * Returns 0 if there exist no multiplicative inverse
     *  Otherwise, Returns a positive modular multiplicative
     *  inverse for the number.
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
     * Euler's Theorem:
     *  If GCD(a, n) = 1, then a^Phi(n) ≡ 1 (mod n) where
     *  Phi is Euler's Totient Function.
     *  This means that for ax ≡ 1, x = a^(Phi(n) - 1).
     * We can compute the inverse using this theorem.
     *
     * Notice that Phi(p) = p - 1 if p is a prime number.
     *  This means that if the mod is a prime number, the
     *  inverse = a^(p - 2).
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

    if (std::abs(GCD(number, mod)) != 1)
        return 0;

    return fast_mod_power(number, phi(mod) - 1, mod);
}


void test(int number, int mod)
{
    int inverse = mod_multiplicative_inverse(number, mod);

    if (inverse == 0) {
        if (std::abs(GCD(number, mod)) == 1)
            std::cout << "Inverse does actually exist" << std::endl;
        return;
    }

    if (((number * inverse) % mod + mod) % mod != 1 && number != 0)
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