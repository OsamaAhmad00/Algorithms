#include <iostream>
#include <vector>
#include <tuple>

template <typename T>
struct ExtendedGCDResult
{
    T GCD, x, y;
    // GCD = GCD(a, b) = x * a + y * b
};

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

    return {a, x, y};
}

// This will need a floating point data type.
template <typename T>
ExtendedGCDResult<T> force_x_value_exact(const T& a, const T& b, const T& desired_x)
{
    /*
     * This is just for the purpose of demonstration.
     *  if you're fine with floating point numbers, you can
     *  simply solve the equation "desired_x * a + y * b = GCD(a, b)".
     *  Note that desired_x, a, b, and GCD are known, thus, this is
     *  just an equation with a single variable.
     *
     * (x + k*b) * a + (y - k*a) = GCD
     * k * b = desired_x - x
     * k = (desired_x - x) / b
     */
    auto result = extended_GCD(a, b);
    T k = (desired_x - result.x) / b;

    result.x = desired_x;
    result.y -= k * a;

    return result;
}

template <typename T>
ExtendedGCDResult<T> force_x_value(const T& a, const T& b, const T& desired_x, bool bigger=true)
{
    // See the function above for a better understanding.
    // Note that this function can return an x value equal
    //  the desired_x. If you don't want this behaviour,
    //  you can pass desired_x - 1 when the argument "bigger"
    //  is false (thus, less), or desired_x + 1 when "bigger"
    //  is true.

    auto result = extended_GCD(a, b);
    T denominator = b / result.GCD;
    T diff = desired_x - result.x;

    /*
     * Integer division performs truncation (floor for positive
     *  numbers, and ceil for negative numbers). Thus, we need
     *  to do some manipulation to get the right result.
     * Notice that if diff is positive, and we add (denominator - 1)
     *  to it before the division, this is equal to performing a ceil.
     *  And if we don't add, integer division performs floor by default
     *  for positive numbers.
     * Also, note that if diff is negative, and we subtract (denominator - 1)
     *  from it before the division, this is equal to performing a floor.
     *  And if we don't add, integer division performs a ceil by default
     *  for negative numbers.
     * We need to floor when "bigger" is false, and ceil if "bigger" is true.
     * Summary: depending on the sign of "diff", and the value of "bigger",
     *  we would have to add, subtract or do nothing.
     *  negative & smaller  ->  subtract
     *  negative & bigger   ->  do nothing
     *  positive & smaller  ->  do nothing
     *  positive & bigger   ->  add
     */

    bool negative = diff < 0;
    int sign = 1 + -2 * negative;

    T additional_value = sign * (denominator - 1) * (negative ^ bigger);
    T k = (diff + additional_value) / denominator;

    result.x += (k * b) / result.GCD;
    result.y -= (k * a) / result.GCD;

    return result;
}

void test_exact(int a, int b, int desired_x, double epsilon=1e-9)
{
    auto result = force_x_value_exact<double>(a, b, desired_x);
    auto error = result.x * a + result.y * b - result.GCD;
    if (error > epsilon || error < -epsilon)
        std::cout << "x and y are not correct" << std::endl;
    std::cout << result.x << " * " << a << " + " << result.y;
    std::cout << " * " << b << " = " << result.GCD << std::endl;
}

void test(int a, int b, int desired_x, bool bigger)
{
    auto result = force_x_value(a, b, desired_x, bigger);
    if ((bigger && result.x < desired_x) || (!bigger && result.x > desired_x))
        std::cout << "The value of x doesn't meet the constraints" << std::endl;
    if (result.x * a + result.y * b != result.GCD)
        std::cout << "x and y are not correct" << std::endl;
    std::cout << result.x << " * " << a << " + " << result.y;
    std::cout << " * " << b << " = " << result.GCD << std::endl;
}

int main()
{
    test_exact(23, 55, 12);
    test_exact(44, 33, 2002);

    test(123, 2323, 423, true);
    test(123, 2323, 423, false);

    test(78, 20, 3, true);
    test(78, 20, 3, false);

    test(44, 120, 12, true);
    test(44, 120, 12, false);
}