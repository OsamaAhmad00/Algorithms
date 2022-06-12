#include <iostream>
#include <vector>
#include <tuple>

template <typename T>
struct Power
{
    T number;
    unsigned int power;
};

template <typename T>
Power<T> compute_power(T& number, const T& base)
{
    Power<T> result = {base, 0};
    while (number % base == 0) {
        result.power++;
        number /= base;
    }
    return result;
}

template <typename T>
std::vector<Power<T>> prime_factorization(T number)
{
    // This is not designed to work with negative numbers.
    // The returned prime factors are sorted.

    std::vector<Power<T>> result;

    if (number % 2 == 0)
        result.push_back(compute_power(number, 2));

    for (T i = 3; i * i <= number; i+=2)
        if (number % i == 0)
            result.push_back(compute_power(number, i));

    // A number can have at most 1 prime number bigger than
    //  its square root. This is checking for this case.
    if (number != 1)
        result.push_back({number, 1});

    return result;
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

template <typename T, typename U>
T fast_power(T number, U power)
{
    T result = 1;
    while (power > 0)
    {
        if (power % 2 != 0)
            result *= number;
        number = number * number;
        power /= 2;
    }
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
std::pair<T, T> power_of_prime_in_number(const T& prime, const T& number)
{
    T divisor = prime;
    T power = 1;
    while (number % divisor == 0)
        divisor *= prime, power++;
    return {power - 1, divisor / prime};
}

template <typename T>
T compute_prime_power(const T& prime, const T& power, const T& mod)
{
    T biggest_power, biggest_divisor;
    std::tie(biggest_power, biggest_divisor) = power_of_prime_in_number(prime, mod);

    // Since mod = prime^biggest_power * x for some
    //  integer x, we know that prime^biggest_power
    //  <= mod, thus, we don't need to call the
    //  fast_mod_power function when computing the power
    //  since it won't exceed the mod anyways. But, we
    //  need to take the mod of the result since the
    //  result can be equal to the mod.
    if (power <= biggest_power)
        return fast_power(prime, power) % mod;

    T t = mod / biggest_divisor;
    T reduced_power = (power - biggest_power) % phi(t);
    T result = fast_mod_power(prime, reduced_power, mod);
    result = (result * biggest_divisor) % mod;

    return result;
}

template <typename T>
T reduced_fast_mod_power(const T& number, const T& power, const T& mod)
{
    /*
     * This function doesn't assume that GCD(number, mod) = 1.
     *
     * Notice that if number = x1 * x2 * ... * xn, then number^p
     *  = x1^p * x2^p * ... * xn^p. Note that the xs doesn't have
     *  to be the prime factors of number, simply any factorization.
     *
     * The idea here is simple. let g = GCD(number, mod).
     *  - If g  = 1, do the same as the coprime version of this function.
     *  - If g != 1, we know that number^power = (number / g)^power * g^power,
     *     and we also know that (number / g) and mod are coprimes, thus,
     *     do the same as the coprime version of this function for (number / g)^power,
     *     and multiply the result by g^power. Since GCD(mod, g) = g != 1, we
     *     can't use the already known method for reducing the power.
     *  To compute g^power, we factorize g into its prime factors first.
     *   Let p1^x1, p2^x2, ..., pn^xn = the prime factors of g, raised to their
     *   powers in g.
     *   For every prime pi in g, do the following:
     *    Compute pi^yi where yi is the power of pi in the mod.
     *    Let ti = mod / pi^yi, in other words, ti = mod without the prime pi in it.
     *    We know that GCD(pi, ti) = 1, and thus, we can use the known method for
     *     power reduction if the mod is ti.
     *    Observe the following fact:
     *     Since GCD(pi, ti) = 1,
     *     pi^z % ti = pi^(z % Phi(ti)) % ti for some power z.
     *     Multiplying both sides by pi^yi:
     *     pi^yi * pi^z = pi^yi * pi^(z % Phi(ti)) % (ti * pi^yi)
     *                  = pi^yi * pi^(z % Phi(ti)) % mod
     *     Notice how the mod has returned back to the original mod.
     *    Conclusion: For a prime pi, pi^power % mod where GCD(pi, mod) != 1
     *     is equal to (pi^yi * pi^((power - yi) % Phi(ti))) % mod where yi
     *     and ti are the same as above.
     *
     *   Returning back to the computation of g^power:
     *    For each prime factor of g, pi^xi, compute pi^power, then raise it to xi.
     *   How to compute pi^power?
     *    - if power <= yi, the power is not very large, and there is
     *      no need to reduce the power
     *    - if power > yi, then pi^power = pi^yi * pi^(power - yi)
     *      = (pi^yi * pi^((power - yi) % Phi(ti))) % mod
     *   At the end, multiply the result by the result of
     *    (pi^power)^xi for every prime pi.
     */

    if (power == 0) return 1;

    T gcd = std::abs(GCD(number, mod));
    T result = fast_mod_power(number / gcd, power % phi(mod), mod);

    if (gcd != 1) {
        auto primes = prime_factorization(gcd);
        for (auto& prime : primes) {
            T x = compute_prime_power(prime.number, power, mod);
            result *= fast_mod_power(x, prime.power, mod);
            result %= mod;
        }
    }

    return result % mod;
}

void test(int number, int power, int mod)
{
    int result1 = reduced_fast_mod_power(number, power, mod);
    int result2 = fast_mod_power(number, power, mod);

    if (result1 != result2)
        std::cout << "The result is not correct" << std::endl;
}

int main()
{
    int n = 250;
    for (int mod = 2; mod <= n; mod++) {
        for (int number = 0; number < mod; number++) {
            for (int power = n * 10000; power <= n * 10001; power++) {
                test(number, power, mod);
                test(-number, power, mod);
            }
        }
    }
}