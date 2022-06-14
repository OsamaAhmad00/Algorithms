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
T mod_multiplicative_inverse(const T& number, const T& mod)
{
    auto result = extended_GCD((number + mod) % mod, mod);
    return ((result.x + mod) % mod) * (std::abs(result.GCD) == 1);
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
     * Even though a solution can exist even if system.mods are not pairwise coprime,
     *  system.mods must be pairwise coprime for the result of this function to be correct.
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
     * To turn a number x (mod n) to y (mod n), we need to
     *  multiply x by inverse(x, n) * y. In other words,
     *  x * inverse(x, n) * y ≡ y (mod n).
     *
     * Recall that for two numbers x, and n, if GCD(x, n) != 1,
     *  then x has no multiplicative inverse mod n.
     *
     * Assume that all mods are pairwise coprime, and let N = product of all the mods.
     *
     * Notice that for every mod m, m and (N / m) are coprime, and that for any other
     *  mod n, (N / m) and n are not coprime. This is because GCD(N/m, n) = n since N
     *  = the product of all the mods, including n.
     * This implies that (N / m) % n = 0 for any mod n, except for the mod m.
     * This also implies that (N / m) has no multiplicative inverse mod any n, and only
     *  has an inverse mod m.
     *
     * Combining all of the above, let's construct a solution for x:
     *
     *  First, we know that the smallest positive solution will be < the LCM. We know that
     *   because we know that the solution set = {some solution +/- k*LCM(mods)}, and given
     *   any solution, we can always construct a positive solution that is smaller than the LCM.
     *
     *  Let mods = {m1, m2, ..., mn}, remainders = {r1, r2, ..., rn}, N = the product of the mods.
     *   and yi = N / mi.
     *  Claim:
     *   One of the solution = r1*y1*inverse(y1, m1) + r2*y2*inverse(y2, m2) + ... rn*yn*inverse(yn, mn).
     *  Proof:
     *   Observe what happens when we take our constructed solution % one of the mods, mi, all
     *    terms except for ri*yi*inverse(yi, mi) will become 0. This is because every other term
     *    contains mi as one of its factors, except for this term.
     *   Notice that the inverse component in the other terms will not act as an inverse when taking
     *    mod mi since the inverse is calculated for a specific mod, and doesn't work with other mods.
     *   Now, let's evaluate the term ri*yi*inverse(yi, mi). Notice that inverse(yi, mi) will cancel
     *    the yi term, leaving only ri, which is what we want.
     *   Conclusion: for every mod mi, this constructed solution will result in ri when taking mod mi.
     *
     *  The smallest positive integer will be this solution % LCM, giving the same effect as subtracting
     *   as many LCMs as possible.
     *  Knowing that one solution is < the LCM, if you don't remember the details of this algorithm, you
     *   can try all number only up to the LCM, and if there is no solution, then the system is not solvable,
     *   otherwise, the solution set = {the found solution +/- k*LCM}.
     */

    size_t n = system.size();
    SystemOfCongruencesResult<T> result{true, 0, 1};

    // Since the mods are pairwise coprime, the LCM = their product.
    for (size_t i = 0; i < n; i++)
        result.LCM *= system.mods[i];
    T& product = result.LCM;

    for (size_t i = 0; i < n; i++)
    {
        T m = system.mods[i];
        T x = system.remainders[i];
        T y = product / m;
        T z = mod_multiplicative_inverse(y % m, m);

        if (z == 0)
            return {false};

        // An overflow can happen here too.
        result.solution += (((x * y) % result.LCM) * z) % result.LCM;
        result.solution %= result.LCM;
    }

    result.solution = (result.solution + result.LCM) % result.LCM;

    return result;
}

void test(const SystemOfCongruences<long long>& system)
{
    auto result = solve(system);

    if (!all_pairwise_coprime(system)) {
        std::cout << "Since the mods are not pairwise coprime, ";
        std::cout << "the function will return a wrong result" << std::endl;
    }

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

    // Wrong results
    test({{2, 3, 4}, {1, 2, 3}}); // not pairwise coprime
    test({{1024, 59049, 390625, 16807}, {123, 2323, 23421, 2000}}); // overflow
}