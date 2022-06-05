#include <iostream>
#include <vector>
#include <tuple>

template <typename T>
struct ExtendedGCDResult
{
    T x, y;
    // GCD(a, b) = x * a + y * b
};

template <typename T>
struct MultiExtendedGCDResult
{
    T GCD;
    std::vector<T> coefficients;
    // For input x1, x2, ..., xn
    // GCD = GCD(x1, x2, ... xn)
    //  = coefficients[0] * x1 + coefficients[1] * x2 + ... + coefficients[n-1] * xn
};

template <typename T>
T GCD(T a, T b)
{
    while (a != 0) {
        b %= a;
        std::swap(a, b);
    }

    return b;
}

template <typename T>
ExtendedGCDResult<T> extended_GCD(T a, T b)
{
    T x = 1, prev_x = 0;
    T y = 0, prev_y = 1;

    while (b != 0)
    {
        T q = a / b;
        std::tie(x, prev_x) = std::make_tuple(prev_x, x - q * prev_x);
        std::tie(y, prev_y) = std::make_tuple(prev_y, y - q * prev_y);
        std::tie(a, b) = std::make_tuple(b, a - q * b);
    }
    return {x, y};
}

template <typename T>
MultiExtendedGCDResult<T> extended_GCD_multi_numbers(const std::vector<T>& numbers)
{
    // This is a very simple extension of the extended Euclidean algorithm.
    // Note that extended_GCD(a, b, c, d) = extended_GCD(a, GCD(b, c, d))
    // Base case: numbers.size() = 2:
    //  Return extended_GCD(a, b)
    // Otherwise:
    //  let x, y = extended_GCD(a, GCD(b, c, ...))
    //  coefficients[0] = x
    //  rest of coefficients = y * extended_GCD(b, c, ...)

    // Note that most of the time, most coefficients will be 0.

    // GCDs[i] = GCD(numbers[n-1], numbers[n-2], ... numbers[i])
    std::vector<T> GCDs(numbers.size());

    GCDs.back() = numbers.back();
    for (int i = numbers.size() - 2; i >= 0; i--)
        GCDs[i] = GCD(GCDs[i+1], numbers[i]);

    MultiExtendedGCDResult<T> result {GCDs.front(), std::vector<T>(numbers.size())};

    T multiplier = 1;
    for (int i = 0; i < numbers.size() - 1; i++) {
        auto x = extended_GCD(numbers[i], GCDs[i+1]);
        result.coefficients[i] = x.x * multiplier;
        multiplier *= x.y;
    }

    result.coefficients.back() = multiplier;

    return result;
}

void test(const std::vector<int>& numbers)
{
    auto result = extended_GCD_multi_numbers(numbers);
    int sum = 0;
    for (int i = 0; i < numbers.size(); i++)
        sum += result.coefficients[i] * numbers[i];
    if (sum != result.GCD)
        std::cout << "The result is not correct" << std::endl;

    for (int i = 0; i < numbers.size(); i++)
        std::cout << " + " << result.coefficients[i] << "*" << numbers[i];
    std::cout << " = " << result.GCD << std::endl;
}

int main()
{
    test({455, 345, 25, 5, 225, 465, 85});
    test({453, 345, 23, 6, 223, 456, 89});
    test({3, 12, 36, 120, 1400});
    test({4, 12, 36, 120, 1400});
    test({-4, 12, -36, 120, -1400});
}