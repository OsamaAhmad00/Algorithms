#include <iostream>
#include <cmath>
#include <vector>

template <typename T>
void add_all_multiples_of(size_t num, std::vector<std::vector<T>>& prime_factors)
{
    for (size_t i = num * 2; i < prime_factors.size(); i+=num)
        prime_factors[i].push_back(num);
}

template <typename T>
std::vector<std::vector<T>> get_unique_prime_factors_of_range(size_t upperbound)
{
    // The result won't include prime factors for the prime numbers.
    // This is to distinguish numbers that are prime from the numbers
    //  that are a power of a prime.
    // The returned factors are sorted.
    std::vector<std::vector<T>> prime_factors(upperbound + 1);

    add_all_multiples_of(2, prime_factors);

    // We can't just iterate up to sqrt(upperbound) here.
    // Even though a number can have at most 1 prime number
    //  that is bigger than its square root, it can have other
    //  multiple factors that are less than its square root.
    // If you were to iterate here only up to the square root,
    //  you'll have to go through every number afterwards and
    //  divide by all of its factors until you're left with one
    //  number, which is not very efficient.
    // Example: the number 1356 = 2 * 2 * 3 * 113
    //  In this example, you'll have to keep dividing by 2 until
    //  it's not divisible by 2, then do the same for 3, then check
    //  if the remaining number is a prime or not.
    for (size_t i = 3; i <= upperbound; i+=2)
        if (prime_factors[i].empty()) // is a prime
            add_all_multiples_of(i, prime_factors);

    return prime_factors;
}
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
        result.push_back(compute_power(number, (T)2));

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
void validate(size_t number)
{
    auto to_validate = get_unique_prime_factors_of_range<T>(number);

    for (int i = 2; i < to_validate.size(); i++)
        if (to_validate[i].empty())
            to_validate[i].push_back(i);

    for (int i = 2; i < to_validate.size(); i++)
    {
        auto valid_factors = prime_factorization(i);
        if (to_validate[i].size() != valid_factors.size())
            std::cout << "Number of prime factors is different for the number " << i << "." << std::endl;

        for (int j = 0; j < valid_factors.size(); j++)
            if (to_validate[i][j] != valid_factors[j].number)
                std::cout << "The prime factors of the number " << i << " are not correct." << std::endl;
    }
}

void test(int upperbound)
{
    validate<int>(upperbound);
    auto result = get_unique_prime_factors_of_range<int>(upperbound);
    std::cout << "Prime factors of the number from 2 to " << upperbound << " are:" << std::endl;
    for (int num = 2; num < result.size(); num++) {
        std::cout << num << ": ";
        for (int factor : result[num])
            std::cout << factor << ' ';
        std::cout << std::endl;
    }
}

int main()
{
    test(100);
}