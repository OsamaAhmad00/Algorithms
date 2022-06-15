#include <iostream>
#include <vector>
#include <map>

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
T GCD(T a, T b)
{
    while (a != 0) {
        b %= a;
        std::swap(a, b);
    }

    return b;
}

template <typename T>
class TurnToCoprimes
{
    /*
     * A system is solvable in two conditions:
     *  1 - if all mods are relatively coprime
     *  or
     *  2 - if for every i, and j, remainders[i] ≡
     *      remainders[j] (mod GCD(mods[i], mods[j]))
     *      The reason for this is that if mods[i] and mods[j]
     *      are not coprimes, you can split the 2 congruences
     *      into 4 congruences with solutions that are a superset
     *      of the solutions for the 2 congruences.
     *      Suppose the two congruences are:
     *       x ≡ r1 (mod m1)
     *       x ≡ r2 (mod m2)
     *       d = GCD(m1, m2) != 1
     *      This can be broken down to these 4 congruences:
     *       x ≡ r1 % (m1/d) (mod m1/d)
     *       x ≡ r2 % (m2/d) (mod m2/d)
     *       x ≡ r1 % d      (mod d)
     *       x ≡ r2 % d      (mod d)
     *      Notice the last 2 congruences. This implies that
     *       r1 % GCD(m1, m2) must equal r2 % GCD(m1, m2) for
     *       the system to have a solution.
     *      Note that these 3 resulting congruences (last 2 are the same)
     *       don't have to have coprime mods. Suppose:
     *        m1 = i*i*i, m2 = i*j => GCD = i
     *       Notice that the 3 mods are {(i*i*i)/GCD, (i*j)/GCD, GCD}
     *        = {i*i, j, i} which are not coprimes.
     *      Also note that the solutions for these 3 congruences
     *       are a superset of the solutions for the original 2
     *       congruences.
     *       Example:
     *        Original congruences:
     *         x = 3 (mod 9)
     *         x = 18 (mod 30)
     *         x = 178 (mod 200)
     *         Solution = {1578 +/- k * 1800}
     *        The unique congruences after breaking the original ones:
     *         x = 0 (mod 2)
     *         x = 0 (mod 3)
     *         x = 3 (mod 5)
     *         Solution = {18 +/- k * 30}
     *        Clearly, the solutions of the later system of congruences
     *         are a superset of the solutions of other system of congruences.
     *
     * This class computes this system of congruences with coprime mods.
     *
     * TODO: Note that this implementation is not the best. Feel free to improve it later.
     */

    const SystemOfCongruences<T>& _system;
    std::map<T, T> map;

    bool add(const T& mod, T remainder)
    {
        if (mod == 1) return true;

        remainder %= mod;
        auto other = map.find(mod);

        if (other != map.end())
            return other->second == remainder;

        bool broken = false;

        for (auto it = map.begin(); it != map.end(); it++) {
            if (GCD(mod, it->first) != 1) {
                T mod2 = it->first;
                T remainder2 = it->second;
                map.erase(it++);
                if (!turn_to_coprimes({{mod, mod2}, {remainder, remainder2}}))
                    return false;
                it--;
                broken = true;
            }
        }

        if (!broken)
            map[mod] = remainder;

        return true;
    }

    bool replace(const T& mod1, const T& mod2)
    {
        T remainder = map[mod1];
        map.erase(map.find(mod1));
        return add(mod2, remainder);
    }

    bool turn_to_coprimes(const SystemOfCongruences<T>& system)
    {
        for (size_t i = 0; i < system.size(); i++)
        {
            T m1 = system.mods[i];
            T r1 = system.remainders[i];

            std::vector<std::pair<T, T>> temp_add;
            std::vector<std::pair<T, T>> temp_replace;

            for (auto& x : map)
            {
                const T& m2 = x.first;
                const T& r2 = x.second;
                auto gcd = GCD(m1, m2);

                if (gcd == 1) continue;

                temp_add.push_back({gcd, r1});
                temp_add.push_back({gcd, r2});
                temp_replace.push_back({m2, m2 / gcd});

                m1 /= gcd;
                r1 %= m1;
            }

            temp_add.push_back({m1, r1});

            bool successful = true;

            // We must replace first before adding the new congruences.
            for (auto& x : temp_replace)
                successful &= replace(x.first, x.second);

            for (auto& x : temp_add)
                successful &= add(x.first, x.second);

            if (!successful)
                return false;
        }

        return true;
    }

    SystemOfCongruences<T> construct() const
    {
        SystemOfCongruences<T> result;

        result.mods.reserve(map.size());
        result.remainders.reserve(map.size());

        for (auto& x : map) {
            result.mods.push_back(x.first);
            result.remainders.push_back(x.second);
        }

        return result;
    }

public:

    TurnToCoprimes(const SystemOfCongruences<T>& system) : _system(system) {}

    SystemOfCongruences<T> compute()
    {
        map.clear();
        if (!turn_to_coprimes(_system))
            return {{}, {}};
        return construct();
    }
};

void test(const SystemOfCongruences<int>& system)
{
    std::cout << "Input: " << std::endl;
    for (int i = 0; i < system.size(); i++) {
        std::cout << "x = " << system.remainders[i];
        std::cout << " (mod " << system.mods[i] << ")" << std::endl;
    }

    auto result = TurnToCoprimes<int>(system).compute();

    std::cout << "Result: " << std::endl;

    if (result.size() == 0) {
        std::cout << "The input couldn't be broken";
        std::cout << "down into coprime congruences" << std::endl;
    }

    for (int i = 0; i < result.size(); i++) {
        std::cout << "x = " << result.remainders[i];
        std::cout << " (mod " << result.mods[i] << ")" << std::endl;
    }

    std::cout << std::endl;
}

int main()
{
    test({{2, 3, 4}, {1, 2, 3}});
    test({{60, 90, 150}, {54, 84, 24}});
    test({{9, 30, 200}, {3, 18, 178}});
}