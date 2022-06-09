#include <iostream>
#include <vector>

template <typename T>
T GCD(T a, T b)
{
    while (a != 0) {
        b %= a;
        std::swap(a, b);
    }

    return b;
}

template <typename T>
T phi_prime_in_number(T& number, const T& prime)
{
    T power_of_prime = 1;
    while (number % prime == 0) {
        power_of_prime *= prime;
        number /= prime;
    }

    // p^(k - 1) * (p - 1)
    return (power_of_prime / prime) * (prime - 1);
}

template <typename T>
T phi(T number)
{
    /*
     * This is not designed to work with numbers < 1.
     *
     * Returns the count of integers < given number
     *  that are relatively prime to the given number.
     *
     * Phi(10) = 4 (1, 3, 7, 9)
     * Phi(5 ) = 4 (1, 2, 3, 4)
     *  Note that since 5 is a prime number, all numbers
     *  less than it are coprimes to it.
     * Generalizing, Phi(p) = p - 1 for any prime number p.
     *
     * Fact: if numbers x1, x2, ..., xn are pairwise coprimes,
     *  then Phi(x1 * x2 * ... * xn) = Phi(x1) * Phi(x2) * ...
     *  * Phi(xn).
     * Fact: for k >= 1, and a prime p, Phi(p^k)
     *  = p^(k - 1) * (p - 1)
     *  = p^k - p^(k - 1)
     *  = p^k * (1 - 1/p)
     * We can use these two facts to compute Phi of any number
     *  by factorizing the number, and computing Phi of the prime
     *  factorization.
     *
     * Some facts about Phi:
     *  - Phi(n) is even for any n > 2.
     *  - sqrt(n) <= Phi(n) <= n - sqrt(n) except for 2 and 6.
     *  - Phi(n^k) = n^(k - 1) * Phi(n)
     *  - For a number n with divisors [d_1, d_2, ..., d_n],
     *     sum of Phi(d_i) for i in range [1, n] = n.
     *  - Menon's Identity: for k in range [1, n], and
     *     GCD(k, n) = 1, sum of GCD(k - 1, n) = Phi(n) * D(n)
     *     where D(n) = number of divisors of n.
     */

    T result = (number > 1);

    if (number % 2 == 0)
        result *= phi_prime_in_number(number, 2);

    for (T i = 3; i * i <= number; i+=2)
        if (number % i == 0)
            result *= phi_prime_in_number(number, i);

    // A number can have at most 1 prime number bigger than
    //  its square root. This is checking for this case.
    if (number != 1)
        result *= phi_prime_in_number(number, (T)number);
    //                                         ^
    //                   Note that we shouldn't pass the same instance
    //                   of number twice since it's taken by reference,
    //                   and the first reference is getting modified.

    return result;
}

template <typename T>
T phi_slow(const T& number)
{
    T result = 0;
    for (T i = 1; i < number; i++)
        result += (GCD(number, i) == 1);
    return result;
}

void test(int number)
{
    auto result = phi(number);
    if (result != phi_slow(number))
        std::cout << "The result is not correct" << std::endl;
//    std::cout << "Phi(" << number << ") = " << result << std::endl;
}

int main()
{
    int n = 10000;
    for (int i = 1; i <= n; i++)
        test(i);
}