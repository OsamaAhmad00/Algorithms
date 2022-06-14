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

    return {a, x, y};
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
bool all_pairwise_coprime(const SystemOfCongruences<T>& system)
{
    size_t n = system.size();
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (GCD(system.mods[i], system.mods[j]) != 1)
                return false;
    return true;
}

template <typename T>
bool is_solvable(const SystemOfCongruences<T>& system)
{
    // The system is solvable in two conditions:
    //  1 - if all mods are relatively coprime
    //  or
    //  2 - if for every i, and j, remainders[i] ≡
    //      remainders[j] (mod GCD(mods[i], mods[j]))

    if (all_pairwise_coprime(system))
        return true;

    size_t n = system.size();

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            T gcd = GCD(system.mods[i], system.mods[j]);
            if (system.remainders[i] % gcd != system.remainders[j] % gcd)
                return false;
        }
    }

    return true;
}

template <typename T>
SystemOfCongruencesResult<T> solve(const SystemOfCongruences<T>& system)
{
    /*
     *  This version of the function is much better than the other one since
     *   it solves any system that has a solution, less prone to overflow, and
     *   it's simpler.
     *
     * The system is solvable in two conditions:
     *  1 - if all mods are relatively coprime
     *  or
     *  2 - if for every i, and j, remainders[i] ≡
     *      remainders[j] (mod GCD(mods[i], mods[j]))
     *
     * If there exists a solution, there exist infinite solutions.
     *  The solutions are {solution +/- k*LCM(mods)} for any integer k.
     *  Let's see why this is the case:
     *   Let mods = {6, 10, 15}, and remainders = {3, 7, 12}, this means that:
     *    x ≡ 3 (mod 6)   -> x = 3  + 6k  for any integer k.
     *    x ≡ 7 (mod 10)  -> x = 7  + 10k for any integer k.
     *    x ≡ 12 (mod 15) -> x = 12 + 15k for any integer k.
     *   Notice that for each congruence, we can add any multiple of its mod.
     *   If we have a solution for x, what is the least common multiple of
     *    all 3 mods that we can add?
     *    Not surprisingly, the least common multiple of the 3 mods.
     *   Notice that it's the LCM, and not the product of all mods. Here, the
     *    LCM = 30 while the product of the mods = 900. Also notice that the
     *    LCM and the product will be the same if all mods are pairwise coprime.
     *   You can verify this for yourself. The solution for x = 27 +/- k * 30
     *    for any integer k.
     *
     *  The smallest positive integer will be this solution % LCM, giving the same effect as subtracting
     *   as many LCMs as possible.
     *  Knowing that one solution is < the LCM, if you don't remember the details of this algorithm, you
     *   can try all number only up to the LCM, and if there is no solution, then the system is not solvable,
     *   otherwise, the solution set = {the found solution +/- k*LCM}.
     *
     *  The idea here is simpler than the other version of this function. The
     *   idea here is that we'll start to merge 2 congruences at a time, and
     *   replace these two with the final result congruence, until there is
     *   only one congruence, which will be the result.
     *
     *  How to merge two congruences?
     *   If we have the two congruences, n ≡ r1 (mod m1) and n ≡ r2 (mod m2), This is equivalent to:
     *    n = x*m1 + r1
     *    n = y*m2 + r2
     *   For some integers x and y.
     *   Equating the two equations:
     *    x*m1 + r1 = y*m2 + r2
     *   Rearranging the terms:
     *    x*m1 - y* m2 = r2 - r1
     *    x*m1 + y*-m2 = r2 - r1
     *   The value of m1, m2, r1, and r2 are known. The only unknowns are x and y.
     *   What does this look like? This is a linear Diophantine equation with
     *    a = m1, b = -m2, c = r2 - r1.
     *   We can get the value of x or y easily, plug it into one of the 2 equations,
     *    and get the value of n.
     *   If there is no solution to the linear Diophantine equation, then there is no solution.
     *   Remember that we're looking for 2 values to be able to merge the 2 congruences:
     *    1 - n, the number that solves the two congruences
     *    2 - the mod of the new congruence
     *   We've already calculated the value of n, still remains the new mod.
     *   From the notes above, we can conclude that the new mod = LCM(m1, m2).
     *
     *  After merging two congruences, we can keep doing
     *   so until we end up with only one final congruence.
     *
     *  Take a look at https://codeforces.com/blog/entry/61290
     */

    size_t n = system.size();
    SystemOfCongruencesResult<T> result{true, system.remainders[0], system.mods[0]};
    T& lcm = result.LCM;

    for (size_t i = 1; i < n; i++)
    {
        // On each iteration, current
        // congruence is x ≡ solution (mod LCM)

        // x*m1 - y* m2 = r2 - r1
        // x*m1 + y*-m2 = r2 - r1
        T a = lcm;
        T b = -system.mods[i];
        T c = system.remainders[i] - result.solution;
        auto answer = linear_diophantine_equation(a, b, c);

        if (!answer.has_solutions)
            return {false};

        // n = x*m1 + r1
        // Recall that the mods have a pretty cool property,
        //  which is that ca % cb = c(a % b).
        // Using this property, we can reduce the likelihood of
        //  an overflow occurring.
        // Recall that LCM(a, b) = (a * b) / GCD(a, b).
        //  Here, we're computing the expression
        //  (x * m1) % LCM(m1, m2) = (x * m1) % (m1 * m2 / GCD(m1, m2)).
        // Notice that m1 is common between the (x * m1) and the LCM.
        //  We can factor m1 (here, it's the variable "lcm") out, take
        //  the mod, then multiply by it. This will make it less likely
        //  that an overflow happens.
        T k = system.mods[i] / GCD(lcm, system.mods[i]);
        lcm *= k; // m1 *= m2 / GCD(m1, m2).
        result.solution += ((answer.x % k) * a) % lcm;
    }

    result.solution = (result.solution + lcm) % lcm;

    return result;
}

void test(const SystemOfCongruences<long long>& system)
{
    auto result = solve(system);

    if (result.has_solution != is_solvable(system))
        std::cout << "Wrong conclusion" << std::endl;

    for (int i = 0; i < system.size(); i++) {
        if (result.solution % system.mods[i] != system.remainders[i]) {
            std::cout << "Wrong result" << std::endl;
        }
    }

    std::cout << std::endl;
}

int main()
{
    test({{2, 3, 5}, {1, 2, 3}});
    test({{64, 27, 125, 49}, {34, 23, 99, 23}});

    test({{2, 3, 4}, {1, 2, 3}}); // not pairwise coprime
    test({{1024, 59049, 390625, 16807}, {123, 2323, 23421, 2000}}); // no overflow
}