#include <iostream>
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
T mu(T number)
{
    /*
     * This is not designed to work with numbers < 1.
     *
     * Returns 0 if the number is not square-free.
     * Returns +1 if the number of prime factors of the number is even.
     * Returns -1 if the number of prime factors of the number is odd.
     *
     * Square-free numbers are the numbers that are not divisible by
     *  perfect squares like 4, 9, 16..., in other words, every prime
     *  factor appears only once in its prime factorization.
     *
     * This function can be useful in cases where inclusion-exclusion
     *  is being used.
     */

    T result = 1;

    if (number % 2 == 0) {
        if (number % 4 == 0) // Not square free.
            return 0;
        result = -result, number /= 2;
    }

    for (T i = 3; i * i <= number; i+=2) {
        if (number % i == 0) {
            if (number % (i * i) == 0) // Not square free.
                return 0;
            result = -result, number /= i;
        }
    }

    // A number can have at most 1 prime number bigger than
    //  its square root. This is checking for this case.
    if (number != 1)
        result = -result;

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

void test(int number)
{
    auto result = mu(number);
    if (result != mu_slow(number))
        std::cout << "The result is not correct" << std::endl;
//    std::cout << "Mu(" << number << ") = " << result << std::endl;
}

int main()
{
    int n = 1000000;
    for (int i = 1; i <= n; i++)
        test(i);
}