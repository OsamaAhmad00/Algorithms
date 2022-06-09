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
std::vector<Power<T>> prime_factorization_of_power(const T& number, const T& power)
{
    auto result = prime_factorization(number);
    for (auto& x : result)
        x.power *= power;
    return result;
}

void test(int number, int power)
{
    auto result = prime_factorization_of_power(number, power);

    std::cout << "The prime factorization of the number " << number << "^" << power << " is:" << std::endl;

    bool not_first = false;
    for (auto& x : result) {
        if (not_first) std::cout << " * ";
        std::cout << x.number << "^" << x.power;
        not_first = true;
    }
    std::cout << std::endl << std::endl;
}

int main()
{
    test(10, 101010);
    test(2, 1000);
    test(12452, 112);
}