#include <iostream>
#include <cmath>
#include <sstream>

double log10_sum(unsigned long long n)
{
    double result = 0;
    for (unsigned long long i = 2; i <= n; i++)
        result += std::log10(i);
    return result;
}

unsigned long long number_of_digits_in_factorial(unsigned long long n)
{
    // Returns the number of digits in a factorial REPRESENTED IN DECIMAL.
    // Warning: This won't work for n <= 1.
    // Number of digits in a number = ceil(log(number))
    // Since log(a * b) = log(a) + log(b), we can compute
    //  log(factorial(n)) by summing log(x) for every x <= n.
    // There is an exception for the powers of the base. Example:
    //  10,000 has 5 digits, but log10(10,000) = 4.
    //  This is the only exception, but since a factorial is never
    //  a power of a base, we don't have to worry about this case.
    return std::ceil(log10_sum(n));
}

unsigned long long number_of_digits_in_factorial_base(unsigned long long n, unsigned long long base)
{
    // We can change the base of the log by dividing by the log of the new base.
    return std::ceil(log10_sum(n) / std::log10(base));
}

unsigned long long number_of_bits_in_factorial(unsigned long long n)
{
    return number_of_digits_in_factorial_base(n, 2);
}

std::string get_hex(unsigned long long num)
{
    std::stringstream stream;
    stream << std::hex << num;
    return stream.str();
}

void test(int upperbound)
{
    // Maximum value of representable factorial in an unsigned long long is factorial(20).
    for (unsigned long long n = 2, factorial = 2; n <= upperbound; n++, factorial *= n) {
        auto digits_count = number_of_digits_in_factorial(n);
        auto string_size = std::to_string(factorial).size();
        if (string_size != digits_count)
            std::cout << "The result is wrong for n = " << n << std::endl;
        std::cout << "Factorial of " << n << " has " << digits_count << " digits." << std::endl;

        // Test on hexadecimal numbers
        auto hex = get_hex(factorial);
        auto digits_count_hex = number_of_digits_in_factorial_base(n, 16);
        if (hex.size() != digits_count_hex)
            std::cout << "The result is wrong for n = " << n << " in hexadecimal." << std::endl;
    }
}

int main()
{
    test(20);
}