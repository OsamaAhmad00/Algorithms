#include <iostream>
#include <cmath>
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
void process_all_multiples_of_prime(size_t prime, std::vector<T>& result)
{
    result[prime] = prime - 1;
    for (size_t i = prime * 2; i < result.size(); i += prime)
    {
        auto number = i;
        T prime_to_power = 1;

        while (number % prime == 0)
            number /= prime, prime_to_power *= prime;

        // p^(k - 1) * (p - 1)
        result[i] *= (prime_to_power / prime) * (prime - 1);
    }
}

template <typename T>
std::vector<T> phi_range(size_t upperbound)
{
    // This is based on Sieve of Eratosthenes.
    // Take a look at the code for Phi of a
    //  single number for better understanding.

    std::vector<T> result(upperbound + 1, 1);
    result[0] = result[1] = 0;

    process_all_multiples_of_prime(2, result);

    // Notice that unlike the algorithm for prime factorization,
    //  we iterate up to the upperbound, not up to sqrt(upperbound).
    for (size_t i = 3; i <= upperbound; i+=2)
        if (result[i] == 1) // if not processed yet (if prime).
            process_all_multiples_of_prime(i, result);

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

void test(size_t upperbound)
{
    auto result = phi_range<int>(upperbound);
    for (int i = 1; i <= upperbound; i++) {
        if (result[i] != phi_slow(i))
            std::cout << "The result is not correct" << std::endl;
    }
}

int main()
{
    test(10000);
}