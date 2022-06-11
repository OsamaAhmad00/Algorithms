#include <iostream>
#include <cmath>

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

    return (result + mod) % mod;
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
    if (std::abs(GCD(number, mod)) != 1)
        return 0;

    return fast_mod_power(number, phi(mod) - 1, mod);
}

template <typename T>
T prime_mod_inverse_large_power(const T& number, const T& power, const T& mod)
{
    /*
     * THIS ASSUMES THAT THE MOD IS A PRIME NUMBER.
     *
     * Revise the code for reducing large powers % mod, and
     * the code for Euler's totient function for better understanding.
     *
     * This is based on a very simple observation:
     *  (1 / x^p) % n = (1 /x % n)^p % n = inverse(x)^p % n.
     *  if GCD(x, n) = 1, then x^Phi(n) = 1, thus, inverse(x)
     *  = x^(Phi(n) - 1) (using Euler's theorem).
     *  if n is prime, then Phi(n) = n - 1.
     *  Combining all of that, (1 / x^p) % n = x^(p * (n - 2)) % n.
     * Since x^Phi(n) = 1, x^m = x^(m % Phi(n)) for some power m.
     *  Thus, the result is equal to x^((p * (n - 2)) % (n - 1))
     *  = x^((p % (n - 1)) * ((n - 2) % (n - 1))).
     *  And since (n - 2) % (n - 1) = -1 (mod (n - 1)), the result
     *  = x^((p % (n - 1)) * -1) = x^(-p % (n - 1)).
     *  The power now is negative, and we want to make is positive.
     *  Since (-p % (n - 1)) = (n - 1) + (-p % (n - 1)), the result
     *  = x^(n - 1 - (p % (n - 1))) % n
     *  Notice that we don't need to take the mod at the end of the
     *  power since we know that p % (n - 1) is positive, and < (n - 1).
     */

    return fast_mod_power(number, mod - 1 - (power % (mod - 1)), mod);
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

void test(int number, int power, int mod)
{
    int result1 = fast_mod_power(number, power, mod);
    result1 = mod_multiplicative_inverse(result1, mod);

    int result2 = prime_mod_inverse_large_power(number, power, mod);

    if (result1 != result2)
        std::cout << "Result is not correct" << std::endl;
}

int main()
{
    int n = 100;
    for (int mod = 2; mod <= n; mod++) {
        if (is_prime(mod)) {
            for (int power = n * 10; power <= n * 20; power++)
                for (int number = 0; number <= n; number++)
                    test(number, power, mod);
        }
    }
}