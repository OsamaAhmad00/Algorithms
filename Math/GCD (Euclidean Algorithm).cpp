#include <iostream>

// GCD of the numbers a = x1^a * x2^b * ... * xn^c
//                and b = x1^d * x2^e * ... * xn^f
// = x1^min(a, d) * x2^min(b, e) * ... * xn^min(c, f)

template <typename T>
T GCD_iterative(T a, T b)
{
    // if b < a, first
    // iteration will swap only.
    while (a > 0) {
        b %= a;
        std::swap(a, b);
    }

    return b;
}

template <typename T>
T GCD_recursive(const T& a, const T& b)
{
    if (a == 0)
        return b;
    return GCD_recursive(b % a, a);
}

void test(int a, int b)
{
    int result1 = GCD_iterative(a, b);
    int result2 = GCD_recursive(a, b);
    if (result1 != result2)
        std::cout << "The two results are not equal..." << std::endl;
    std::cout << "GCD of " << a << " and " << b << " is " << result1 << std::endl;
}

int main()
{
    test(5, 230);
    test(33, 390);
    test(1, 1);
    test(1, 0);
    test(0, 1);
    test(200, 200);
    test(1000, 99);
}