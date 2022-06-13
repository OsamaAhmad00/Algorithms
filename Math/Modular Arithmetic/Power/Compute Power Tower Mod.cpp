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

template <typename T, typename U = size_t>
class PowerTower
{
    const std::vector<T>& powers;

    std::pair<T, T> power_of_prime_in_number(const T& prime, const T& number)
    {
        T divisor = prime;
        T power = 1;
        while (number % divisor == 0)
            divisor *= prime, power++;
        return {power - 1, divisor / prime};
    }

    T compute_prime_power(const T& prime, const U& index, const T& mod)
    {
        // Revise the function for reducing powers where the number
        //  and the mod are coprimes for better understanding.

        T biggest_power, biggest_divisor;
        std::tie(biggest_power, biggest_divisor) = power_of_prime_in_number(prime, mod);

        T t = mod / biggest_divisor;
        T p = phi(t);

        // Calling the reduction function again, this
        //  time with the mod = Phi(t).
        T power = reduced_fast_mod_power(index + 1, p);
        // Adding p to ensure that the result is positive.
        T reduced_power = (p + power - biggest_power) % p;

        T result = fast_mod_power(prime, reduced_power, mod);
        result = (result * biggest_divisor) % mod;

        return result;
    }

    T reduced_fast_mod_power(const U& index, const T& mod)
    {
        /*
         * This is a generalization of the function for reducing
         *  large powers in which the number and the mod are coprime.
         *  Revise the mentioned function for better understanding.
         *
         * Idea:
         *  This is a recursive function. Given x1^x2^x3^...^xn, treat
         *   it as just x1^y where y = x2^x3^...^xn.
         *   To compute x1^y, you need to evaluate y first. To compute
         *   y, call the function recursively, evaluating x2^x3^...^xn.
         *   The recursive call will treat the given exponent as x2^z
         *   where z = x3^...^xn, and so on.
         *
         *  There is one more detail here. When calling the function
         *   recursively, we can't just pass the same mod every time.
         *   This is because we can't take the mod for exponents, in
         *   other words, p^x % n != p^(x % n) % n.
         *  But, with that being said, we know that p^x % n
         *   = p^(x % Phi(n)) % n if GCD(p, n) = 1. We can use this fact
         *   to keep reducing the power, essentially taking the mod each time.
         *
         *  There is a problem with this approach, which is that to be
         *   able to take the mod for the power, GCD(number, mod) must
         *   be equal to 1, but this won't always be the case.
         *   To know how this problem is solved, revise the code and the
         *   explanation of the function for reducing large powers in
         *   which the number and the mod are coprime.
         *
         * Potentially useful links:
         *  https://stackoverflow.com/q/21367824/9140652
         *  https://stackoverflow.com/q/4223313/9140652
         *  https://math.stackexchange.com/q/653682/698734
         */

        const T& number = powers[index];

        if (index == powers.size() - 1)
            return number % mod;

        T gcd = std::abs(GCD(number, mod));
        T power = reduced_fast_mod_power(index + 1, phi(mod));
        T result = fast_mod_power(number / gcd, power, mod);

        // If number == 0, GCD(number, mod) = mod, but
        //  in this case, we don't want to enter here.
        if (number != 0 && gcd != 1) {
            auto primes = prime_factorization(gcd);
            for (auto& prime : primes) {
                T x = compute_prime_power(prime.number, index, mod);
                result *= fast_mod_power(x, prime.power, mod);
                result %= mod;
            }
        }

        return result % mod;
    }

public:

    explicit PowerTower(std::vector<T>& powers) : powers(powers) {}

    T compute(const T& mod) {
        return reduced_fast_mod_power(0, mod);
    }
};

void test(std::vector<int> powers, int mod, bool print_equation = true)
{
    PowerTower<int> pt{powers};
    if (print_equation) {
        bool first = true;
        for (int i: powers) {
            if (!first)
                std::cout << "^";
            std::cout << i;
            first = false;
        }
        std::cout << " % " << mod << " = ";
    }
    std::cout << pt.compute(mod) << std::endl;
}

int main()
{
    test({15}, 10); // 5
    test({0, 0, 0, 0}, 12); // 1
    test({0, 1, 0, 0}, 12); // 0
    test({2, 3, 4}, 12); // 8
    test({5, 4, 3, 2, 1}, 100); // 25
    test({100, 9, 9}, 122); // 102
    test({2, 2, 2, 2, 2}, 1000); // 736
    test({12, 34, 56, 78}, 90); // 36
    test({2323, 24323, 76574756, 76456, 543456}, 1e9 + 7); // 2323, Not verified.

    int n = 1000;
    std::vector<int> v(n);
    for (int i = 0; i < n; i++)
        v[i] = n - i;

    test(v, 1e9 + 7, false);
    test(v, 1e9 + 9, false);
    test(v, 109, false);
}