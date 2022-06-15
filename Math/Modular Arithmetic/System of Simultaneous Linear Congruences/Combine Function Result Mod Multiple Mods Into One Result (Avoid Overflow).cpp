#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>

template <typename T>
struct FunctionAnswer
{
    bool can_be_computed = true;
    T answer;
};

template <typename T>
struct Power
{
    T number;
    unsigned int power;
};

template <typename T>
struct ExtendedGCDResult
{
    T GCD, x, y;
    // GCD = GCD(a, b) = x * a + y * b
};

template <typename T>
struct LinearDiophantineResult
{
    // This is set to false when there are no solutions.
    bool has_solutions = true;

    // for an input a, b, and c,
    //  x * a + y * b = c
    // c has to be divisible by GCD(a, b).
    T x, y;
};

template <typename T>
struct SystemOfCongruences
{
    // System of simultaneous linear congruences.
    // xi ≡ remainders[i] (mod mods[i])
    std::vector<T> mods;
    std::vector<T> remainders;
    size_t size() const { return mods.size(); }
};

template <typename T>
struct SystemOfCongruencesResult
{
    bool has_solution = true;

    // Smallest positive solution.
    T solution;

    // If there exists a solution, there exist infinite solutions.
    //  Solutions are {solution +/- k*LCM(mod_1, mod_2, ..., mod_n)}
    //  for any integer k.
    T LCM;
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

template <typename T>
T mod_multiplicative_inverse(const T& number, const T& mod)
{
    auto result = extended_GCD((number + mod) % mod, mod);
    return ((result.x + mod) % mod) * (std::abs(result.GCD) == 1);
}

template <typename T>
T GCD(T a, T b)
{
    while (a != 0) {
        b %= a;
        std::swap(a, b);
    }

    return b;
}

template <typename T, typename U>
T fast_power(T number, U power)
{
    T result = 1;
    while (power > 0)
    {
        if (power % 2 != 0)
            result *= number;
        number = number * number;
        power /= 2;
    }
    return result;
}

template <typename T, typename U, typename V>
T fast_mod_power(T number, U power, V mod)
{
    T result = 1;
    while (power > 0)
    {
        if (power % 2 != 0)
            result = (result * number) % mod;
        number = (number * number) % mod;
        power /= 2;
    }

    return (result + mod) % mod;
}

template <typename T>
Power<T> compute_power(T& number, const T& base)
{
    Power<T> result = {base, 0};
    while (number % base == 0) {
        result.power++;
        number /= base;
    }
    return result;
}

template <typename T>
std::vector<Power<T>> prime_factorization(T number)
{
    std::vector<Power<T>> result;

    if (number % 2 == 0)
        result.push_back(compute_power(number, 2));

    for (T i = 3; i * i <= number; i+=2)
        if (number % i == 0)
            result.push_back(compute_power(number, i));

    if (number != 1)
        result.push_back({number, 1});

    return result;
}

template <typename T>
LinearDiophantineResult<T> linear_diophantine_equation(const T& a, const T& b, const T& c)
{
    auto answer = extended_GCD(a, b);

    if (c % answer.GCD != 0)
        return {false};

    auto q = c / answer.GCD;

    return {true, answer.x * q, answer.y * q};
}

template <typename T>
SystemOfCongruencesResult<T> solve(const SystemOfCongruences<T>& system)
{
    size_t n = system.size();
    SystemOfCongruencesResult<T> result{true, system.remainders[0], system.mods[0]};
    T& lcm = result.LCM;

    for (size_t i = 1; i < n; i++)
    {
        T a = lcm;
        T b = -system.mods[i];
        T c = system.remainders[i] - result.solution;
        auto answer = linear_diophantine_equation(a, b, c);

        if (!answer.has_solutions)
            return {false};

        T k = system.mods[i] / GCD(lcm, system.mods[i]);
        lcm *= k; // m1 *= m2 / GCD(m1, m2).
        result.solution += ((answer.x % k) * a) % lcm;
    }

    result.solution = (result.solution + lcm) % lcm;

    return result;
}

template <typename T, typename Function>
SystemOfCongruencesResult<T> combine_answers(const Function& function, std::vector<T> mods)
{
    /*
     * The idea here is very simple. Since we know how to solve
     *  a system of simultaneous linear congruences, we can compute
     *  a function mod different mods, and represent them a system
     *  of congruences as follows:
     *   x ≡ f(mod_1) (mod mod_1)
     *   x ≡ f(mod_2) (mod mod_2)
     *   ...
     *   x ≡ f(mod_n) (mod mod_n)
     *  And solve this system, and the final answer will be equal to
     *   f(LCM(mod_1, mod_2, ..., mod_n)).
     *
     *  This has a very neat use, if we're trying to compute a value,
     *   and we know that the final answer will fit in a certain data
     *   type (int for example), but we also know that the intermediate
     *   calculations will overflow, this function can be useful.
     *  First, notice that taking a mod bigger than the value has no effect.
     *   Example: 123 % 1000 = 123, Notice how the 123 remained the same.
     *  If we know that the answer of our function won't exceed x, then
     *   answer % y where y is any integer > x won't affect the answer.
     *  Since this function allows us to combine answers, we can compute
     *   the answer mod different small mods, and get the answer mod their
     *   LCM.
     *  If the LCM is bigger than x, then taking the mod won't change the answer.
     *  By doing so, we're getting rid of the intermediate overflow (since the
     *   different mods are relatively small), and at the same time, getting
     *   the correct result at the end.
     */

    SystemOfCongruences<T> system;
    system.remainders.resize(mods.size());
    for (int i = 0; i < mods.size(); i++)
        system.remainders[i] = function(mods[i]);
    system.mods = std::move(mods);

    return solve(system);
}

template <typename T, typename Function>
FunctionAnswer<T> compute_function(const Function& function, const T& mod)
{
    /*
     * If the function can be computed only % a power of a prime, this function
     *  can be used to compute it % any number. It computes the given function
     *  multiple times, each time taking the mod of a different prime power in the
     *  given mod, and combining the result at the end.
     */

    auto factorization = prime_factorization(mod);

    std::vector<T> mods(factorization.size());
    for (int i = 0; i < mods.size(); i++)
        mods[i] = fast_power(factorization[i].number, factorization[i].power);

    auto result = combine_answers(function, std::move(mods));

    return {result.has_solution, result.solution};
}

template <int N>
int test_compute_100th_power_mod(int mod)
{
    int result = 1;
    for (int i = 0; i < 100; i++)
        result = (result * (N % mod)) % mod;
    return result;
}

template <typename T>
struct FunctionThatOverflows
{
    // A function that overflows when using an int,
    //  but the final result fits in an int.

    T a;

    T operator()(const T& coprime_mod) const
    {
        // This function boils down to (2 * a) % prime_mod.
        // the mod has to be coprime to x to have an inverse.
        T x = a % coprime_mod;
        T y = (x * x) % coprime_mod;
        T z = mod_multiplicative_inverse(x, coprime_mod);
        return (y * z * 2) % coprime_mod;
    }
};

template <typename Function>
void test(const Function& function, int mods_count)
{
    for (int i = 2; i <= mods_count; i++) {
        auto result = compute_function(function, i);
        if (!result.can_be_computed) continue;
        if (result.answer != function(i)) {
            std::cout << "Wrong result" << std::endl;
            return;
        }
    }
    std::cout << "Test Passed!" << std::endl << std::endl;
}

void test_overflow()
{
    int a = 1e7;
    int mod = 997 * 1009 * 1013;

    std::cout << "Result using \"int\" data type (overflows): ";
    std::cout << FunctionThatOverflows<int>{a}(mod) << std::endl;
    std::cout << "Result using \"long long\" data type (doesn't overflow): ";
    std::cout << FunctionThatOverflows<long long>{a}(mod) << std::endl;
    std::cout << "Result using \"int\" data type, combining answers (doesn't overflow): ";
    auto answer = combine_answers<int>(FunctionThatOverflows<int>{a}, {997, 1009, 1013});
    std::cout << answer.solution << std::endl;
}

int main()
{
    test(test_compute_100th_power_mod<12>, 1000);
    test_overflow();
}
