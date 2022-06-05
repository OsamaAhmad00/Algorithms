#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

template <typename T>
std::vector<std::vector<T>> get_divisors_of_range(size_t upperbound)
{
    // The returned factors are sorted.
    std::vector<std::vector<T>> divisors(upperbound + 1);

    for (size_t i = 1; i <= upperbound; i++)
        for (size_t j = i; j <= upperbound; j+=i)
            divisors[j].push_back(i);

    return divisors;
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
        result.push_back(compute_power(number, (T)2));

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

template <typename T>
void validate(size_t number)
{
    auto to_validate = get_divisors_of_range<T>(number);

    for (int i = 2; i < to_validate.size(); i++)
    {
        auto divisors = GetAllDivisors<T>(i).compute();
        std::sort(divisors.begin(), divisors.end());

        if (to_validate[i].size() != divisors.size())
            std::cout << "Number of divisors is different for the number " << i << "." << std::endl;

        for (int j = 0; j < divisors.size(); j++)
            if (to_validate[i][j] != divisors[j])
                std::cout << "The divisors of the number " << i << " are not correct." << std::endl;
    }
}

void test(int upperbound)
{
    validate<int>(upperbound);
    auto result = get_divisors_of_range<int>(upperbound);
    std::cout << "Divisors of the numbers from 2 to " << upperbound << " are:" << std::endl;
    for (int num = 2; num < result.size(); num++) {
        std::cout << num << ": ";
        for (int factor : result[num])
            std::cout << factor << ' ';
        std::cout << std::endl;
    }
}

int main()
{
    test(100);
}