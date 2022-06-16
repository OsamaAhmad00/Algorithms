#include <iostream>
#include <vector>
#include <algorithm>

constexpr char base_char();

class BigInt
{
    // FIXME: This is not very fast. Needs some optimization.

    bool is_negative = false;
    typedef char NumType;
    std::vector<NumType> number;

    typedef BigInt&(BigInt::*Operator)(const BigInt&);

public:

    static const int BASE = 10;

    BigInt() { number.push_back(0); }

    BigInt(const std::string &str) { setNumber(str); }

    BigInt(const char *str) { setNumber(str); }

    BigInt(const BigInt& other) {
        *this = other;
    }

    BigInt(BigInt&& other) noexcept {
        *this = std::move(other);
    }

    BigInt& operator=(const BigInt& other)
    {
        number = other.number;
        is_negative = other.is_negative;
        return *this;
    }

    BigInt& operator=(BigInt&& other) noexcept
    {
        number = std::move(other.number);
        is_negative = other.is_negative;
        return *this;
    }

    unsigned long long size() const { return number.size(); }

    NumType& numberAtPos(int index) { return number[index]; }

    NumType numberAtPos(int index) const { return number[index]; }

    operator long long() const
    {
        long long result = 0;
        for (int i = 0; i < size(); i++) {
            result *= BASE;
            result += numberAtPos(i);
        }
        return result;
    }

    operator int() const {
        return (int) ((long long) *this);
    }

    bool operator<(const BigInt &other) const
    {

        if (is_negative ^ other.is_negative) return is_negative;
        if (size() < other.size()) return !is_negative;
        if (size() > other.size()) return is_negative;

        for (int i = size() - 1; i >= 0; i--)
        {
            NumType a = numberAtPos(i);
            NumType b = other.numberAtPos(i);
            if (a != b) {
                return (a < b) && !is_negative;
            }
        }

        return false;
    }

    bool operator==(const BigInt &other) const {
        return number == other.number && is_negative == other.is_negative;
    }

    void add_no_sign(const BigInt &other)
    {
        int max_size = std::max(size(), other.size());
        number.resize(max_size);
        for (int i = 0; i < other.size(); i++) {
            numberAtPos(i) += other.numberAtPos(i);
        }
        normalize();
    }

    void subtract_no_sign(const BigInt &other)
    {
        int max_size = std::max(size(), other.size());
        number.resize(max_size);
        for (int i = 0; i < other.size(); i++) {
            numberAtPos(i) -= other.numberAtPos(i);
        }
        normalize();
    }

    BigInt &operator+=(const BigInt &other)
    {
        if (is_negative != other.is_negative) {
            subtract_no_sign(other);
        } else {
            add_no_sign(other);
        }

        return *this;
    }

    BigInt &operator-=(const BigInt &other)
    {
        // TODO this can be optimized
        if (is_negative != other.is_negative) {
            add_no_sign(other);
        } else {
            if (*this >= other) {
                subtract_no_sign(other);
            } else {
                *this = other - *this;
                this->is_negative = true;
            }
        }

        return *this;
    }

    BigInt &operator*=(const BigInt &other)
    {
        // TODO this function is very slow!

        // if this = 0
        if (number.back() == 0) return *this;

        is_negative ^= other.is_negative;

        BigInt result;
        result.number.reserve(size() + other.size() + 1);

        for (int i = other.size() - 1; i >= 0; i--) {
            // TODO CHANGE THIS!!
            if (i != other.size() - 1) {
                result.number.insert(result.number.begin(), 0);
            }

            NumType multiplier = other.numberAtPos(i);
            if (multiplier == 0) continue;

            BigInt temp = *this;

            for (int j = 0; j < size(); j++) {
                temp.numberAtPos(j) *= multiplier;
            }

            result += temp; // this normalizes.
        }

        *this = std::move(result);

        return *this;
    }

    BigInt &divideBy2()
    {
        if (size() == 1) {
            numberAtPos(0) /= 2;
            return *this;
        }

        std::vector<NumType> new_number;
        NumType current = number.back();
        number.pop_back();

        while (!number.empty())
        {
            current *= BASE;
            current += number.back();
            number.pop_back();

            new_number.push_back(current / 2);

            current %= 2;
        }

        std::reverse(new_number.begin(), new_number.end());
        number = std::move(new_number);

        normalize();

        return *this;
    }

    BigInt &operator/=(const BigInt &other)
    {
        if (other > *this) {
            *this = "0";
            return *this;
        }

        BigInt lowerBound = *this;
        lowerBound.is_negative = true;
        BigInt upperBound = *this;
        upperBound.is_negative = false;
        BigInt multiplier;

        while (true)
        {
            multiplier = lowerBound + upperBound;
            multiplier.divideBy2();

            BigInt value = other * multiplier;

            if (value == *this) {
                break;
            }

            if (value < *this) {
                if (lowerBound == multiplier) break;
                lowerBound = multiplier;
            } else {
                if (upperBound == multiplier) break;
                upperBound = multiplier;
            }
        }

        multiplier.is_negative = is_negative ^ other.is_negative;

        *this = std::move(multiplier);

        return *this;
    }

    BigInt &operator%=(const BigInt &other)
    {
        BigInt div;

        if (other > *this) {
            return *this;
        }

        if (other == *this) {
            *this = "0";
            return *this;
        }

        if (other.size() == 1 && other.number.front() == 2) {
            std::string res = std::string(1, '0' + numberAtPos(0) % 2);
            *this = res;
            return *this;
        } else {
            div = *this / other;
        }

        BigInt quotient = div * other;
        *this -= quotient;
        return *this;
    }

    BigInt pow(int power) const
    {
        BigInt result = "1";
        BigInt multiplier = *this;

        while (power > 0)
        {
            if (power % 2 == 1) {
                result *= multiplier;
            }

            multiplier *= multiplier;
            power /= 2;
        }

        return result;
    }

    BigInt pow_mod(BigInt power, const BigInt &mod) const
    {
        BigInt result = "1";
        BigInt multiplier = *this % mod;

        while (power > "0")
        {
            if (power % "2" == "1") {
                result *= multiplier;
                result %= mod;
            }

            multiplier *= multiplier;
            multiplier %= mod;
            power.divideBy2();
        }

        return result;
    }

    std::string to_string() const
    {
        std::string result;
        result.resize(size() + is_negative);

        for (int i = 0; i < size(); i++) {
            result[i] = base_char() + numberAtPos(i);
        }

        if (is_negative)
            result.back() = '-';

        std::reverse(result.begin(), result.end());

        return result;
    }

    void normalize()
    {
        for (int i = 0; i < size(); i++)
        {
            NumType &current = numberAtPos(i);
            NumType div = current / BASE;
            current %= BASE;

            if (current < 0) {
                if (i == size() - 1) {
                    current = -current;
                    is_negative = !is_negative;
                } else {
                    current += BASE;
                    div--;
                }
            }

            if (div != 0) {
                if (i == size() - 1) number.push_back(0);
                numberAtPos(i + 1) += div;
            }
        }

        while (size() > 1 && number.back() == 0) {
            number.pop_back();
        }

        // the number is 0
        if (number.back() == 0) is_negative = false;
    }

    void setNumber(std::string str)
    {
        std::reverse(str.begin(), str.end());

        is_negative = str.back() == '-';
        if (is_negative) str.pop_back();

        number.resize(str.size());

        for (int i = 0; i < size(); i++) {
            numberAtPos(i) = str[i] - base_char();
        }

        if (number.empty()) number.push_back(0);

        while (number.size() > 1 && number.back() == 0) number.pop_back();
    }

    unsigned long long digits_count() const {
        return size();
    }

    // TODO is it worth it? can be optimized.
    bool operator!=(const BigInt& other) const { return !operator==(other); }
    bool operator>=(const BigInt& other) const { return !operator<(other); }
    bool operator> (const BigInt& other) const {return operator>=(other) && operator!=(other); }
    bool operator<=(const BigInt& other) const { return operator<(other) || operator==(other); }

    BigInt arithmetic_operators_common(Operator compoundOp, const BigInt &other) const
    {
        BigInt result = *((const BigInt *)this);
        (result.*compoundOp)(other);
        return result;
    }

    // TODO refactor the arithmetic operators

    BigInt operator+(const BigInt& other) const
    {
        return arithmetic_operators_common(BigInt::operator+=, other);
    }

    BigInt operator-(const BigInt& other) const
    {
        return arithmetic_operators_common(BigInt::operator-=, other);
    }

    BigInt operator*(const BigInt& other) const
    {
        return arithmetic_operators_common(BigInt::operator*=, other);
    }

    BigInt operator/(const BigInt& other) const
    {
        return arithmetic_operators_common(BigInt::operator/=, other);
    }

    BigInt operator%(const BigInt& other) const
    {
        return arithmetic_operators_common(BigInt::operator%=, other);
    }
};

constexpr char base_char() { return (BigInt::BASE <= 10 ? '0' : 'A'); }

std::istream &operator>>(std::istream &stream, BigInt &number)
{
    std::string input;
    stream >> input;

    number.setNumber(input);
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const BigInt &number) {
    stream << number.to_string();
    return stream;
}

int main()
{
    // TODO: This needs further testing.
    BigInt i("10000");
    std::cout << i * "123123123123123123123" << std::endl;
}