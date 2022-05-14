#include <iostream>
#include <cmath>

template <typename T, typename U>
T fast_power(const T& number, const U& power)
{
    if (power <= 0) return 1;
    T result = fast_power(number, power / 2);
    result *= result;
    if (power % 2 != 0)
        result *= number;
    return result;
}

void test(long long n, unsigned int power)
{
    long long result = fast_power(n, power);
    long long correct = std::pow(n, power);

    if (result != correct)
        std::cout << "The result is not correct for n = " << n << std::endl;

    std::cout << n << "^" << power << " = " << result << std::endl;
}

int main()
{
    test(2, 10);
    test(30, 4);
    test(10, 11);
    test(5, 11);
}