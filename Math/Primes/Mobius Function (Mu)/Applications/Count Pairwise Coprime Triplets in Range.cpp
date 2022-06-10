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

    if (upperbound >= 2)
        process_all_multiples_of_prime(2, result, is_prime);

    for (size_t i = 3; i <= upperbound; i+=2)
        if (is_prime[i])
            process_all_multiples_of_prime(i, result, is_prime);

    return result;
}

size_t count_coprime_triplets(size_t n)
{
    // This is a simplified case of this problem:
    //  https://www.codechef.com/LTIME13/problems/COPRIME3
    // Explanation here:
    //  https://discuss.codechef.com/t/coprime3-editorial/6051

    // Using Mobius function and inclusion-exclusion.

    auto mu = mu_range<int>(n);

    size_t result = 0;

    for (int i = 1; i <= n; i++) {
        // This variable has to be signed because
        //  the computation below can be negative.
        // This is the count of numbers that i
        //  divides in the range [0, n].
        long long multiples = n / i;
        // result += multiples choose 3.
        result += mu[i] * multiples * (multiples - 1) * (multiples - 2) / 6;
    }

    return result;
}

void test(size_t upperbound)
{
    size_t result1 = count_coprime_triplets(upperbound);
    size_t result2 = 0;
    for (size_t i = 1; i <= upperbound; i++)
        for (size_t j = i + 1; j <= upperbound; j++)
            for (size_t k = j + 1; k <= upperbound; k++)
                result2 += (GCD(i, GCD(j, k)) == 1);

    if (result1 != result2)
        std::cout << "Result is not correct" << std::endl;
}

int main()
{
    int n = 200;
    for (int i = 1; i < n; i++)
        test(i);
}