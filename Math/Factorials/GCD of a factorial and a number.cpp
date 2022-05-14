#include <iostream>
#include <vector>

template <typename T, typename U>
T fast_power(T number, U power)
{
    T result = 1;
    while (power > 0)
    {
        if (power % 2 != 0)
            result *= number;
        number = number * number;
        power /= 2;
    }
    return result;
}

template <typename T>
T power_of_prime_in_factorial(const T& n, const T& prime)
{
    T power = 0;
    for (T i = prime; i <= n; i*=prime)
        power += n / i;
    return power;
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
unsigned long long GCD_of_number_and_factorial_of_n(const T& n, const T& number)
{
    unsigned long long result = 1;

    for (auto& factor : prime_factorization(number))
    {
        unsigned long long power_in_factorial = power_of_prime_in_factorial(n, factor.number);
        unsigned long long power_in_number = factor.power;
        unsigned long long max_power_for_this_prime = std::min(power_in_factorial, power_in_number);
        result *= fast_power(factor.number, max_power_for_this_prime);
    }
    return result;
}

void test(int n, int number)
{
    auto result = GCD_of_number_and_factorial_of_n(n, number);
    std::cout << "GCD of " << n << "! and " << number << " is " << result << std::endl;
}

int main()
{
    test(10, 625);
    test(200, 340);
    test(1200, 1234);
    test(5, 7);
    test(20, 5);
}