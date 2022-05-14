#include <iostream>

template <typename T>
T power_of_prime_in_factorial(const T& n, const T& prime)
{
    T power = 0;
    for (T i = prime; i <= n; i*=prime)
        power += n / i;
    return power;
}

unsigned long long number_of_trailing_zeros_in_factorial(unsigned long long n)
{
    return power_of_prime_in_factorial(n, 5ULL);
}

unsigned int last_digit_in_factorial_before_trailing_zeros(unsigned long long n)
{
    const unsigned int mod = 10;
    unsigned int factorial = 1;

    unsigned long long twos, fives;
    twos = fives = number_of_trailing_zeros_in_factorial(n);

    for (unsigned long long i = 2; i <= n; i++)
    {
        unsigned long long x = i;
        while (x % 2 == 0 && twos) x /= 2, twos--;
        while (x % 5 == 0 && fives) x /= 5, fives--;
        factorial = (factorial * (x % mod)) % mod;
    }

    return factorial;
}

void test(int n)
{
    std::cout << "Last digit of " << n << "! before the zeros is ";
    std::cout << last_digit_in_factorial_before_trailing_zeros(n) << std::endl;
}

int main()
{
    test(10);
    test(1000);
    test(12);
    test(23434);
}