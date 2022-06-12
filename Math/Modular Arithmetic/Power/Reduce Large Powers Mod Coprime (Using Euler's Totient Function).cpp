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
    // Phi(1) is defined to be 1.
    T result = (number >= 1);

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

template <typename T>
T reduced_fast_mod_power(const T& number, const T& power, const T& mod)
{
    /*
     * Returns 0 if GCD(number, mod) != 1.
     *
     * Let a = number, p = power, n = mod.
     *
     * Euler's Theorem:
     *  If GCD(a, n) = 1, then a^Phi(n) ≡ 1 (mod n) where
     *  Phi is Euler's Totient Function.
     *
     * Notice that Phi(n) = n - 1 if n is a prime number.
     *  This means that if the mod is a prime number,
     *  a^(n - 1) ≡ 1 (mod n).
     *
     * Let x = Phi(power).
     * Claim: We can reduce a^p to a^(p % x).
     * Proof:
     *  We can represent a^p % n as:
     *    a^((p/x * x) + p%x)
     *  = a^(p/x * x) * a^(p % x)
     *  = 1 * a^(p % x) = a^(p % x).
     *
     * NOTICE THAT PHI(0) = PHI(1) = 0, THUS, THE MODS
     *  MUST BE >= 2 TO AVOID DIVISION BY 0 EXCEPTION.
     */

    if (std::abs(GCD(number, mod)) != 1)
        return 0;

    return fast_mod_power(number, power % phi(mod), mod);
}

template <typename T>
T reduced_fast_mod_power_prime(const T& number, const T& power, const T& mod)
{
    /*
     * Returns 0 if GCD(number, mod) != 1.
     *
     * This function assumes that the mod is a prime number.
     *
     * Let a = number, p = power, n = mod.
     *
     * Euler's Theorem:
     *  If GCD(a, n) = 1, then a^Phi(n) ≡ 1 (mod n) where
     *  Phi is Euler's Totient Function.
     *
     * Notice that Phi(n) = n - 1 if n is a prime number.
     *  This means that if the mod is a prime number,
     *  a^(n - 1) ≡ 1 (mod n).
     *
     * Let x = Phi(power).
     * Claim: We can reduce a^p to a^(p % x).
     * Proof:
     *  We can represent a^p % n as:
     *    a^((p/x * x) + p%x)
     *  = a^(p/x * x) * a^(p % x)
     *  = 1 * a^(p % x) = a^(p % x).
     *
     * Since we know that the mod is prime, we don't need to call the
     *  Phi function. The result is equal to a^(p % (n - 1)) % n.
     *
     * NOTICE THAT PHI(0) = PHI(1) = 0, ALSO, NOTICE THAT WE TAKE % (MOD - 1)
     *  THUS, THE MODS MUST BE >= 2 TO AVOID DIVISION BY 0 EXCEPTION.
     */

    if (std::abs(GCD(number, mod)) != 1)
        return 0;

    return fast_mod_power(number, power % (mod - 1), mod);
}

void test(int number, int power, int mod)
{
    int result1 = reduced_fast_mod_power(number, power, mod);

    if (std::abs(GCD(number, mod)) != 1) {
        if (result1 != 0)
            std::cout << "Result should be 0" << std::endl;
        return;
    }

    int result2 = fast_mod_power(number, power, mod);

    if (number != 0 && result1 != result2)
        std::cout << "The result is not correct" << std::endl;

    if (is_prime(mod)) {
        int result3 = reduced_fast_mod_power_prime(number, power, mod);
        if (result3 != result1)
            std::cout << "The version for the prime mod is not correct" << std::endl;
    }
}

int main()
{
    int n = 200;
    for (int mod = 2; mod <= n; mod++) {
        for (int number = 0; number < mod; number++) {
            for (int power = 0; power <= n; power++) {
                test(number, power, mod);
                test(-number, power, mod);
            }
        }
    }
}