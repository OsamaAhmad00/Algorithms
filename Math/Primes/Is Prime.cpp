#include <iostream>
#include <cmath>

template <typename T>
bool is_prime(const T& number)
{
    if (number != 2 && number % 2 == 0)
        return false;

    T s = std::sqrt(std::abs(number));
    for (T i = 3; i <= s; i+=2)
        if (number % i == 0)
            return false;
    return true;
}

int main()
{
    std::cout << std::boolalpha;
    std::cout << is_prime(1) << std::endl;
    std::cout << is_prime(-11) << std::endl;
    std::cout << is_prime(132) << std::endl;
    std::cout << is_prime(13) << std::endl;
}