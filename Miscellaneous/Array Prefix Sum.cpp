#include <iostream>
#include <vector>

template <typename T>
class PrefixSum1D
{
    std::vector<T> sums;

public:

    PrefixSum1D(const std::vector<T>& array) : sums(array.size() + 1)
    {
        sums[0] = 0;

        for (int i = 0; i < array.size(); i++) {
            sums[i + 1] = sums[i] + array[i];
        }
    }

    T query(int l, int r) {
        return sums[r + 1] - sums[l];
    }
};

void test(const std::vector<int>& v)
{
    PrefixSum1D<int> sums(v);
    for (int i = 0; i < v.size(); i++) {
        for (int j = i; j < v.size(); j++) {
            int sum = 0;
            for (int k = i; k <= j; k++)
                sum += v[k];
            if (sum != sums.query(i, j))
                std::cout << "Fail..." << std::endl;
        }
    }
}

int main()
{
    test({32, 23, 1, 56, 12, 6, 1, 5, 55, 90, 23, 11});
}