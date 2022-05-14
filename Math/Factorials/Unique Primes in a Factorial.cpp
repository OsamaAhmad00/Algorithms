#include <iostream>
#include <cmath>
#include <vector>

void add_all_multiples_of(size_t num, std::vector<bool>& is_prime)
{
    // is_prime[num] = true, is_prime[num * x] = false for any x > 1.
    for (size_t i = num * 2; i < is_prime.size(); i+=num)
        is_prime[i] = false;
}

std::vector<bool> get_primes_in_range_mask(size_t upperbound)
{
    // The number of primes can be approximated as upperbound/ln(upperbound)
    std::vector<bool> is_prime(upperbound + 1, true);

    add_all_multiples_of(2, is_prime);

    auto s = (size_t)std::sqrt(upperbound);
    for (size_t i = 3; i <= s; i+=2)
        if (is_prime[i])
            add_all_multiples_of(i, is_prime);

    return is_prime;
}

std::vector<size_t> get_primes_in_range(size_t upperbound, size_t lowerbound = 2)
{
    auto mask = get_primes_in_range_mask(upperbound);
    std::vector<size_t> result;
    for (size_t i = lowerbound; i < mask.size(); i++)
        if (mask[i])
            result.push_back(i);
    return result;
}

std::vector<size_t> get_unique_primes_of_factorial(size_t n)
{
    return get_primes_in_range(n);
}

void test(size_t n)
{
    auto result = get_unique_primes_of_factorial(n);
    std::cout << "The unique primes of " << n << "! are:" << std::endl;
    for (size_t x : result)
        std::cout << x << ' ';
    std::cout << std::endl << std::endl;
}

int main()
{
    test(210);
    test(300);
}