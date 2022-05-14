#include <iostream>

template <typename T>
T power_of_prime_in_factorial(const T& n, const T& prime)
{
    // Returns the power of the given prime in factorial(n).
    // Let's take an example where n = 17, prime = 2
    // 17! = 1 * 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10 * 11 * 12 * 13 * 14 * 15 * 16 * 17
    //
    //                                                                                    (2)
    //                                                                                     *
    //                                         (2)                   3                    (2)
    //                                          *                    *                     *
    //            1        (2)        3        (2)        5         (2)         7         (2)
    //            *         *         *         *         *          *          *          *
    // 17! = 1 * (2) * 3 * (2) * 5 * (2) * 7 * (2) * 9 * (2) * 11 * (2) * 13 * (2) * 15 * (2) * 17
    // When we divide 17 by 2, we get the number of 2's in the first layer, which is 8.
    // When we divide 17 by 4, we get the number of 2's in the second layer, which is 4.
    // And so on, until we reach the biggest power of 2 <= 17 which is 16, we get the number
    //  of 2's in the last layer, which is always 1.
    // Note that the number of occurrences of the prime number in each level doesn't have
    //  to be a power of the prime. In other words, in the example here, the last layer has
    //  2^0 2's, and the layer below it has 2^1 2's, and the layer below it has 2^2 2's.
    //  This is not always the case.
    //  Consider the example where n = 12 and prime = 5. We have only one 5 in the first layer,
    //  and one 5 in the layer above it.

    T power = 0;
    for (T i = prime; i <= n; i*=prime)
        power += n / i;
    return power;
}

void test(int n, int prime)
{
    std::cout << "The power of the prime " << prime << " in the factorial of " << n << " is ";
    std::cout << power_of_prime_in_factorial<unsigned long long>(n, prime);
    std::cout << std::endl;
}

int main()
{
    test(17, 2);
    test(16, 2);
    test(12, 2);
    test(12, 5);
    test(12, 13);
    test(20, 19);
    test(1000, 13);
}