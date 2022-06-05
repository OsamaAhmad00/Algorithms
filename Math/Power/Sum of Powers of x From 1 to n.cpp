#include <iostream>

template <typename T, typename U>
T power_sum(const T& base, const U& n)
{
    /*
     * Example:
     *  base = a, n = 6
     *  a^1 + a^2 + a^3 + a^4 + a^5 + a^6
     *  = (a^1 + a^2 + a^3) + ((a^1 * a^3) + (a^2 * a^3) + (a^3 * a^3))
     *  = (a^1 + a^2 + a^3) + (a^1 + a^2 + a^3) * a^3
     *  = (a^1 + a^2 + a^3) * (1 + a^3)
     *  = power_sum(a, n/2 = 3) * (1 + a^3)
     *  Here, we need to calculate only up until the half (n / 2 = 6 / 2 = 3).
     *  The problem here is that we need to compute a^3 alone.
     *  We can see that a^3 = (power_sum(a, n/2 = 3) / a) - 1.
     */

    if (n == 0)
        return 0;

    if (n % 2 != 0)
        return base * (1 + power_sum(base, n - 1));

    T first_half = power_sum(base, n / 2);
    T last_term = first_half - ((first_half / base) - 1);

    return first_half * (1 + last_term);
}

void test(long long base, int n)
{
    long long num = 1;
    long long sum = 0;
    for (int i = 0; i < n; i++)
        num *= base, sum += num;

    long long result = power_sum(base, n);

    if (result != sum)
        std::cout << "The result is wrong..." << std::endl;

    std::cout << "Sum of " << base << "^1 + " << base << "^2 + ... + ";
    std::cout << base << "^" << n << " = " << result << std::endl;
}

int main()
{
    test(2, 15);
    test(3, 4);
    test(20, 3);
    test(2, 60);
}