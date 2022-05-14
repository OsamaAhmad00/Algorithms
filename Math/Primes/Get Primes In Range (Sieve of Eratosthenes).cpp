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

void test(size_t upperbound, size_t lowerbound = 2)
{
    auto primes = get_primes_in_range(upperbound, lowerbound);
    std::cout << "Primes in the range (" << lowerbound << ", " << upperbound << ") are:" << std::endl;
    for (size_t i : primes)
        std::cout << i << ' ';
    std::cout << std::endl;
}

int main()
{
    test(1000);
    test(2000, 1980);
    test(10);
}