#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

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
    std::vector<T> result(upperbound + 1, 1);
    std::vector<bool> is_prime(upperbound + 1, true);

    process_all_multiples_of_prime(2, result, is_prime);

    for (size_t i = 3; i <= upperbound; i+=2)
        if (is_prime[i])
            process_all_multiples_of_prime(i, result, is_prime);

    return result;
}

template <typename T>
std::vector<T> get_indices(std::vector<T> numbers)
{
    // Using Mobius function and inclusion-exclusion.

    T max_value = *std::max_element(numbers.begin(),  numbers.end());
    auto mu = mu_range<T>(max_value);

    // Just an alias
    auto& result = numbers;

    for (int i = 0; i < numbers.size(); i++) {
        T number = numbers[i];
        // Notice that we're iterating only up to sqrt(number).
        for (int x = 2; x * x <= number; x++)
            result[i] += mu[x] * (number / (x * x));
    }

    return result;
}

void test(size_t upperbound)
{
    auto mu = mu_range<size_t>(upperbound);

    // Sorted List of square free numbers.
    std::vector<size_t> square_free;
    for (size_t i = 1; i <= upperbound; i++)
        if (mu[i] != 0)
            square_free.push_back(i);

    auto result = get_indices(square_free);
    for (size_t i = 0; i < result.size(); i++)
        if (result[i] != i + 1)
            std::cout << "Result is not correct" << std::endl;
}

int main()
{
    test(1000000);
}