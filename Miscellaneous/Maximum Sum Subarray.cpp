#include <iostream>
#include <vector>
#include <random>

template <typename T>
struct Range
{
    int from;
    int to;
    T sum;
};

template <typename T>
Range<T> get_max_sum(const std::vector<T>& array)
{
    Range<T> result = {0, 0, array[0]};
    Range<T> current = {0, 0, 0};

    for (int i = 0; i < array.size(); i++)
    {
        if (current.sum < 0) {
            current.sum = 0;
            current.from = i;
        }

        current.sum += array[i];
        current.to = i;

        if (current.sum > result.sum)
            result = current;
    }

    return result;
}

void test(int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 200 + 1);

    std::vector<int> v(size);

    for (int &i : v)
        i = (int)distrib(gen) - 100;

    int max_sum = v[0];

    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            int sum = 0;
            for (int k = i; k <= j; k++)
                sum += v[k];
            max_sum = std::max(max_sum, sum);
        }
    }

    auto result = get_max_sum(v);

    if (result.sum != max_sum) {
        std::cout << "Fail..." << std::endl;
    }

    std::cout << "Array: ";
    for (int i : v) std::cout << i << ' ';
    std::cout << std::endl;
    std::cout << "Max sum = " << result.sum << " (from index " << result.from;
    std::cout << " to " << result.to << ")" << std::endl << std::endl;
}

int main()
{
    test(100);
}