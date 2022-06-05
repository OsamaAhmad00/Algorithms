#include <iostream>

// GCD of the numbers a = x1^a * x2^b * ... * xn^c
//                and b = x1^d * x2^e * ... * xn^f
// = x1^min(a, d) * x2^min(b, e) * ... * xn^min(c, f)

template <typename T>
T GCD_iterative(T a, T b)
{
    // if b < a, first
    // iteration will swap only.
    while (a != 0) {
        b %= a;
        std::swap(a, b);
    }

    return b;
}

// LCM of the numbers a = x1^a * x2^b * ... * xn^c
//                and b = x1^d * x2^e * ... * xn^f
// = x1^max(a, d) * x2^max(b, e) * ... * xn^max(c, f)
// Notice that here, we look for the max of the two
//  powers while in the case of the GCD, we look for
//  the min of the two powers.

template <typename T>
T LCM(const T& a, const T& b)
{
    // a * b will add the powers of the primes.
    // Dividing by the GCD (which is the product
    //  of the primes raised to the minimum power
    //  of the two numbers) will subtract the minimum
    //  power, leaving the primes raised to the maximum
    //  power of the primes of the two numbers.
    return (a * b) / GCD_iterative(a, b);
}

void test(int a, int b)
{
    std::cout << "LCM of " << a << " and " << b << " is " << LCM(a, b) << std::endl;
}

int main()
{
    test(5   , 230);
    test(33  , 390);
    test(1   , 1);
    test(1   , 0);
    test(0   , 1);
    test(200 , 200);
    test(1000, 99);

    test(-5   , 230);
    test(-33  , 390);
    test(-1   , 1);
    test(-1   , 0);
    test(-0   , 1);
    test(-200 , 200);
    test(-1000, 99);

    test(5   , -230);
    test(33  , -390);
    test(1   , -1);
    test(1   , -0);
    test(0   , -1);
    test(200 , -200);
    test(1000, -99);

    test(-5   , -230);
    test(-33  , -390);
    test(-1   , -1);
    test(-1   , -0);
    test(-0   , -1);
    test(-200 , -200);
    test(-1000, -99);
}