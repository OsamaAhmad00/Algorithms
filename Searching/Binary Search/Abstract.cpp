#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

unsigned long long fibonacci(int i)
{
    // Maximum fibonacci number that
    // fits in an unsigned 64-bit integer.
    static const int max_index = 93;
    static unsigned long long results[max_index + 1];

    static bool done = false;
    if (!done)
    {
        results[0] = 0;
        results[1] = 1;
        for (int i = 2; i <= max_index; i++)
            results[i] = results[i-1] + results[i-2];
        done = true;
    }

    return results[i];
}

// This namespace is to avoid conflicts due to ADL (argument dependent lookup).
namespace BinarySearch
{

// Binary search can be used to search for any
// value in a monotonic (non-increasing, or
// non-decreasing) function like finding the
// index of a fibonacci number.


template <typename T, typename U, typename Function>
U binary_search(const U& begin, const U& end, const T& value, const Function& function)
{
    U low = begin;
    U high = end;

    while (low < high)
    {
        U mid = low + (high - low) / 2;

        if (function(mid, value))
            high = mid;
        else
            low = mid + 1;
    }

    return low;
}

template <typename Iterator, typename T>
Iterator lower_bound(const Iterator& begin, const Iterator& end, const T& value)
{
    auto comp = [](const Iterator& a, const T& b) { return *a >= b; };
    return BinarySearch::binary_search(begin, end, value, comp);
}

template <typename Iterator, typename T>
Iterator upper_bound(const Iterator& begin, const Iterator& end, const T& value)
{
    auto comp = [](const Iterator& a, const T& b) { return *a > b; };
    return BinarySearch::binary_search(begin, end, value, comp);
}

int fibonacci_index(unsigned long long fibonacci_number, const std::function<bool(int, unsigned long long)>& comp)
{
    // Since the Fibonacci sequence is a monotonic function,
    // we can use binary search to determine the index.
    static const int max_index = 93;
    // passing max_index + 1 since the high pointer should be last index + 1.
    return BinarySearch::binary_search(0, max_index + 1, fibonacci_number, comp);
}

int fibonacci_first_index(unsigned long long fibonacci_number)
{
    auto comp = [](int index, unsigned long long value) {
        return fibonacci(index) >= value;
    };
    return fibonacci_index(fibonacci_number, comp);
}

int fibonacci_last_index(unsigned long long fibonacci_number)
{
    auto comp = [](int index, unsigned long long value) {
        return fibonacci(index) > value;
    };
    return fibonacci_index(fibonacci_number, comp) - 1;
}

}

void test_fibonacci()
{
    const int max_index = 93;

    // fibonacci(1) and fibonacci(2) = 1,
    //  thus starting from index 3.
    for (int i = 3; i <= max_index; i++)
    {
        unsigned long long result = fibonacci(i);
        if (BinarySearch::fibonacci_first_index(result) != i ||
            BinarySearch::fibonacci_last_index(result)  != i)
        {
            std::cout << "Fail..." << std::endl;
        }
    }
}

void test_lower_upper_bound(const std::vector<int>& v)
{
    int min = *std::min_element(v.begin(), v.end()) - 1;
    int max = *std::max_element(v.begin(), v.end()) + 1;

    for (int value = min; value <= max; value++)
    {
#define index(func) func(v.begin(), v.end(), value) - v.begin()

        int my_lb = index(BinarySearch::lower_bound);
        int my_ub = index(BinarySearch::upper_bound);
        int lb = index(std::lower_bound);
        int ub = index(std::upper_bound);

#undef index

        if (my_lb != lb || my_ub != ub) {
            std::cout << "Fail..." << std::endl;
        }
    }
}

int main()
{
    test_fibonacci();
    test_lower_upper_bound({1, 1, 1, 1, 2, 3, 3, 3, 4, 5, 6, 6, 6, 7, 9});
}