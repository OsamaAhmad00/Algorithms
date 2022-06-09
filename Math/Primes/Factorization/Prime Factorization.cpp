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
void compute_power_push_back(T& number, const T& base, std::vector<T>& vector)
{
    while (number % base == 0) {
        vector.push_back(base);
        number /= base;
    }
}

template <typename T>
std::vector<T> prime_factorization_list(T number)
{
    // This is not designed to work with negative numbers.
    // The returned prime factors are sorted.

    std::vector<T> result;

    if (number % 2 == 0)
        compute_power_push_back(number, 2, result);

    for (T i = 3; i * i <= number; i+=2)
        if (number % i == 0)
            compute_power_push_back(number, i, result);

    // A number can have at most 1 prime number bigger than
    //  its square root. This is checking for this case.
    if (number != 1)
        result.push_back(number);

    return result;
}

void test(int number)
{
    auto result1 = prime_factorization(number);
    auto result2 = prime_factorization_list(number);

    std::cout << "The prime factorization of the number " << number << " is:" << std::endl;

    std::cout << "result1: ";
    bool not_first = false;
    for (auto& x : result1) {
        if (not_first) std::cout << " * ";
        std::cout << x.number << "^" << x.power;
        not_first = true;
    }

    std::cout << std::endl;

    std::cout << "result2: ";
    not_first = false;
    for (int x : result2) {
        if (not_first) std::cout << " * ";
        std::cout << x;
        not_first = true;
    }

    std::cout << std::endl << std::endl;
}

int main()
{
    test(100);
    test(13);
    test(1);
    test(2);
    test(25);
    test(-36);
    test(-900);
    test(1024);
}