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
    // To get the number of the trailing zeros in n!, we need to get
    //  the biggest power of 10 (2 * 5) in the factorial.
    // Notice that the factorial gets multiplied by 2's every 2 numbers,
    //  while it gets multiplied by 5's every 5 numbers.
    // We can conclude from this that the biggest power of 5 in a factorial
    //  will always be less than the biggest power of 2 in it, thus, we
    //  can compute only the biggest power of 5.
    // Note that this is not doable for every base. This will work only for
    //  bases in which primes in its prime factorization are not repeated.
    //  In this case, you can compute only the biggest power of the biggest
    //  prime factor of the base.
    // Examples:
    //  You can't do this for the hexadecimal base since the prime
    //  factors of 16 are 2^4.
    //  You can do it for base 34 since the prime factors of 34 = 2^1 * 17^1
    //   in which case, we would compute only the biggest power of 17 in the
    //   factorial.
    return power_of_prime_in_factorial(n, 5ULL);
}

void test(int n)
{
    std::cout << "Number of trailing zeros in " << n << "! is ";
    std::cout << number_of_trailing_zeros_in_factorial(n) << std::endl;
}

int main()
{
    test(10);
    test(1000);
    test(12);
    test(23434);
}