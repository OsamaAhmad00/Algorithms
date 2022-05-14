#include <iostream>
#include <vector>
#include <algorithm>

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
class GetAllDivisors
{
    std::vector<Power<T>> prime_factors;
    std::vector<T> divisors;

    void add_divisor(int index, const T& current)
    {
        if (index == prime_factors.size()) {
            divisors.push_back(current);
            return;
        }

        T power = 1;
        for (int i = 0; i <= prime_factors[index].power; i++) {
            add_divisor(index+1, current * power);
            power *= prime_factors[index].number;
        }
    }

    unsigned long long all_divisors_count()
    {
        unsigned long long result = 1;
        for (auto& x : prime_factors)
            result *= (x.power + 1);
        return result;
    }

public:

    GetAllDivisors(const T& number) : prime_factors(prime_factorization(number)) {
        divisors.reserve(all_divisors_count());
    }

    std::vector<T> compute() {
        // This should be called only once.
        add_divisor(0, 1);
        return divisors;
    }
};

void test(int number)
{
    auto result = GetAllDivisors<int>{number}.compute();
    std::sort(result.begin(), result.end());
    std::cout << "All divisors of the number " << number << " are:" << std::endl;
    for (int divisor : result) {
        std::cout << divisor << ' ';
    }
    std::cout << std::endl << std::endl;
}

int main()
{
    test(10);
    test(1230);
    test(9930);
}