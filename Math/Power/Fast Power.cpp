#include <iostream>
#include <cmath>

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

void test(long long n, unsigned int power)
{
    long long result = fast_power(n, power);
    long long correct = std::pow(n, power);

    if (result != correct)
        std::cout << "The result is not correct for " << n << "^" << power << std::endl;

    std::cout << n << "^" << power << " = " << result << std::endl;
}

int main()
{
    test(2, 10);
    test(30, 4);
    test(10, 11);
    test(5, 11);
}