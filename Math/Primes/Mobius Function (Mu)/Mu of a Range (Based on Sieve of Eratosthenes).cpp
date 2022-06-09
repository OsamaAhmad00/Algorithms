#include <iostream>
#include <cmath>
#include <vector>

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
void process_all_multiples_of_prime(size_t prime, std::vector<T>& result, std::vector<bool>& is_prime)
{
    result[prime] = -1;
    size_t square = prime * prime;
    for (size_t i = prime * 2; i < result.size(); i += prime) {
        result[i] *= (i % square != 0) * -1;
        is_prime[i] = false;
    }
}

template <typename T>
std::vector<T> mu_range(size_t upperbound)
{
    // This is based on Sieve of Eratosthenes.
    // Take a look at the code for Mu of a
    //  single number for better understanding.

    std::vector<T> result(upperbound + 1, 1);
    std::vector<bool> is_prime(upperbound + 1, true);

    if (upperbound >= 2)
        process_all_multiples_of_prime(2, result, is_prime);

    // Notice that unlike the algorithm for prime factorization,
    //  we iterate up to the upperbound, not up to sqrt(upperbound).
    for (size_t i = 3; i <= upperbound; i+=2)
        if (is_prime[i])
            process_all_multiples_of_prime(i, result, is_prime);

    return result;
}

template <typename T>
T mu_slow(const T& number)
{
    int result = 1;
    auto factorization = prime_factorization(number);
    for (auto& p : factorization) {
        if (p.power > 1)
            return 0;
        result = -result;
    }
    return result;
}

void test(size_t upperbound)
{
    auto result = mu_range<int>(upperbound);
    for (int i = 1; i <= upperbound; i++) {
        if (result[i] != mu_slow(i))
            std::cout << "The result is not correct" << std::endl;
    }
}

int main()
{
    test(1000000);
}