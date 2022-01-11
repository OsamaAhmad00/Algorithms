#include <iostream>
#include <vector>

namespace BinarySearch
{

template <typename Iterator, typename T>
Iterator lower_bound(const Iterator& begin, const Iterator& end, const T& value)
{
    Iterator low = begin;
    Iterator high = end;

    while (low < high)
    {
        Iterator mid = low + (high - low) / 2;

        if (*mid >= value)
            high = mid;
        else
            low = mid + 1;
    }

    return low;
}

}

template <typename Iterator, typename T>
Iterator exponential_search(Iterator begin, Iterator end, const T& value)
{
    int offset = 1;

    while ((begin + offset) < end && *(begin + offset) < value)
        offset *= 2;

    begin += offset / 2;
    end = std::min(begin + offset, end);

    return BinarySearch::lower_bound(begin, end, value);
}

void test(const std::vector<int>& v)
{
    for (int value : v)
    {
        auto result = exponential_search(v.begin(), v.end(), value);
        if (*result != value) {
            std::cout << "Fail..." << std::endl;
        }
    }
}


int main()
{
    test({1, 1, 1, 1, 2, 3, 3, 3, 4, 5, 6, 6, 6, 7, 9});
}