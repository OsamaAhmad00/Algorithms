#include <iostream>
#include <cmath>

double square_root(double number, double epsilon = 1e-9)
{
    double low = 0;
    // if the number is less than 1, the
    //  square root will be bigger than
    //  the number.
    double high = std::max(number, 1.0);

    while (high - low > epsilon)
    {
        double mid = low + (high - low) / 2;

        // Here, it's simpler. Since the search
        // space is continuous, can we don't need
        // to move neither the low nor the high
        // pointer past the mid pointer.
        if (mid * mid < number)
            low = mid;
        else
            high = mid;
    }

    return low;
}

// A better way is to use Newton's method:
double newton_square_root(double number, double epsilon = 1e-9)
{
    double x = number / 2;
    double d = number;
    double nx = 0;

    while (std::abs(d) > epsilon)
    {
        nx = x - (x * x - number) / (2 * x);
        d = nx - x;
        x = nx;
    }

    return nx;
}

void test_square_root(double epsilon = 1e-9)
{
    for (double i = 0; i < 1000; i+=0.001) {
        double correct = std::sqrt(i);
        double result = square_root(i, epsilon);
        if (std::abs(result - correct) > epsilon) {
            std::cout << "Failed. Number = " << i << ", expected "
                << std::sqrt(i) << ", got " << square_root(i) << std::endl;
        }
    }
}

int main()
{
    test_square_root();
}