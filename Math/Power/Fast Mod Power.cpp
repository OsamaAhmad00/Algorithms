#include <iostream>
#include <cmath>

template <typename T, typename U, typename V>
T fast_mod_power(T number, U power, V mod)
{
    T result = 1;
    while (power > 0)
    {
        if (power % 2 != 0)
            result = (result * number) % mod;
        number = (number * number) % mod;
        power /= 2;
    }

    return (result + mod) % mod;
}

void test(long long n, unsigned int power, long long mod)
{
    long long result = fast_mod_power(n, power, mod);
    long long correct = ((long long)std::pow(n, power) % mod + mod) % mod;

    if (result != correct) {
        std::cout << "The result is not correct for " << n;
        std::cout << "^" << power << " mod " << mod << std::endl;
    }

//    std::cout << n << "^" << power << " = " << result << std::endl;
}

int main()
{
    int n = 10;
    for (int mod = 1; mod <= n; mod++)
        for (int i = 0; i < mod; i++)
            for (int j = 0; j < mod; j++)
                test(i, j, mod), test(-i, j, mod);
}