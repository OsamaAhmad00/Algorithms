#include <iostream>
#include <vector>
#include <functional>

// Because of the fast power function, mod^2
//  should fit in the data type of the hash.
typedef long long HashType;

HashType power_mod(HashType num, int pow, HashType mod)
{
    if (pow == 0)
        return 1;

    HashType result = power_mod(num, pow / 2, mod);
    result *= result;

    if (pow % 2 == 1)
        result *= num;

    return result % mod;
}

class RabinKarp
{
    // Change the constant values if you want.
    // Also, you can have different bases for
    //  different hashes if you want.

    // The base (or number of unique digits)
    //  must be >= the number unique characters
    //  in the string. This is because every unique
    //  character will be mapped to a unique digit.
    //  256 allows all ASCII characters.
    const static int base = (1 << 8);

    // Since the hash grows quickly, we have to take
    //  the mod, otherwise the hash will overflow.
    const static HashType main_mod = 1e9 + 7;

    int m; // length of the pattern.
    int current_pos;

    // This is stored so that later additions
    //  of new hash functions is possible.
    std::string pattern;

    struct Hash
    {
        const int base;
        const HashType mod;
        const HashType base_multiplier;
        HashType hash;

        void slide_start(char c) {
            hash -= c * base_multiplier;
            hash %= mod;
            hash += mod;
            hash %= mod;
        }

        void slide_end(char c) {
            hash *= base;
            hash += c;
            hash %= mod;
        }

        void set_hash(const std::string& string, int start, int len)
        {
            hash = 0;

            for (int i = 0; i < len; i++) {
                slide_end(string[start + i]);
            }
        }
    };

    std::vector<Hash> hashes;
    std::vector<HashType> pattern_hashes;

    void slide_window(const std::string& string)
    {
        for (Hash& h : hashes) {
            h.slide_start(string[current_pos]);
            h.slide_end(string[current_pos + m]);
        }

        current_pos++;
    }

    bool is_match()
    {
        for (int i = 0; i < hashes.size(); i++)
            if (hashes[i].hash != pattern_hashes[i])
                return false;
        return true;
    }

    void init()
    {
        m = pattern.size();
        add_hash(main_mod);
    }

public:

    void add_hash(HashType mod)
    {
        HashType multiplier = power_mod(base, m - 1, mod);
        hashes.push_back({base, mod, multiplier, 0});

        Hash& h = hashes.back();
        h.set_hash(pattern, 0, pattern.size());
        pattern_hashes.push_back(h.hash);
        h.hash = 0;
    }

    RabinKarp(std::string pattern) : pattern(std::move(pattern))
    {
        init();
    }

    std::vector<int> get_matches(const std::string& string)
    {
        // The idea here is simple. We'll represent the pattern
        //  as a number, and will have a window with size = m
        //  sliding through the given string. At every position
        //  of the window, we compare the number (the hash) of
        //  the window, and the number of the pattern. If they
        //  match, then we have a match.
        // How to represent a string as a number?
        //  Given that the number of unique characters = x, we
        //   can represent the string in a base x number system.
        //  Each character will be assigned a unique digit, and
        //   each position in the string corresponds to a certain
        //   power of the base. Last character will be multiplied
        //   by 1, the one before it will be multiplied by x, and
        //   the one before it will be multiplied by x^2, and so on.
        // By doing so, the number can grow quickly, which will cause
        //  an overflow, thus, incorrect results. To overcome this
        //  problem, we take the mod to keep the hash within the
        //  representable range.
        // By taking the mod, though it's not very likely, we might
        //  get wrong results. Because of that, the matches are
        //  "Potential" matches.
        // We can use more than one hash function, which will greatly
        //  decrease the possibility of a false match. Also, we can
        //  compare the strings themselves upon a potential match, but
        //  since a probability of getting a false match is very small,
        //  it's not worth it to compare the strings.

        if (string.size() < m || pattern.empty())
            return {};

        std::vector<int> result;

        for (Hash& h : hashes) {
            h.set_hash(string, 0, m);
        }

        current_pos = 0;

        while (true)
        {
            if (is_match())
                result.push_back(current_pos);

            if (current_pos == string.size() - m)
                break;

            slide_window(string);
        }

        return result;
    }
};

void test(const std::string& string, const std::string& pattern, const std::vector<HashType>& mods = {})
{
    RabinKarp rk(pattern);
    for (HashType mod : mods) {
        rk.add_hash(mod);
    }

    std::cout << "String: " << string << std::endl;
    std::cout << "Pattern: " << pattern << std::endl;

    std::vector<int> matches = rk.get_matches(string);

    std::cout << "Potential matches start at indices: ";
    for (int match : matches)
        std::cout << match << ' ';
    std::cout << std::endl << std::endl;
}

int main()
{
    test("anananana", "ana");
    test("aaaa", "a");
    test("ababababa", "aba");
    test("ababdabacdababcababcabab", "ababcabab", {(long long)(1e9 + 9)});
    test("aaaabaaaab", "aaaac");
    test("ababab", "abababa");
    test("ababab", "");
    test("", "abababa");
}