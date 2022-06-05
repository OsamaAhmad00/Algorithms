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
unsigned long long all_divisors_count(const T& number)
{
    unsigned long long result = 1;
    for (auto& x : prime_factorization(number))
        result *= (x.power + 1);
    return result;
}

template <typename T>
unsigned long long all_divisors_count_of_power(const T& number, unsigned int power)
{
    unsigned long long result = 1;
    for (auto& x : prime_factorization(number))
        result *= (x.power * power + 1);
    return result;
}

void test(int number)
{
    std::cout << "The number " << number << " has "
              << all_divisors_count(number) << " divisors." << std::endl;
}

void test_divisors_of_power(int number, int power)
{
    std::cout << "The number " << number << "^" << power << " has "
        << all_divisors_count_of_power(number, power) << " divisors." << std::endl;
}

int main()
{
    test(10);
    test(1230);
    test(9930);

    test_divisors_of_power(10, 5);
    test_divisors_of_power(1230, 3);
    test_divisors_of_power(9930, 3);
}