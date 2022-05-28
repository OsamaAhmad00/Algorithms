#include <iostream>
#include <vector>
#include <algorithm>

template <typename T, typename Comparator>
std::vector<T> merge_sort(
    const typename std::vector<T>::iterator& begin,
    const typename std::vector<T>::iterator& end,
    const Comparator& compare = std::less<>()
)
{
    if (begin + 1 == end)
        return { *begin };

    auto mid = begin + (end - begin) / 2;
    std::vector<T> left = merge_sort<T>(begin, mid, compare);
    std::vector<T> right = merge_sort<T>(mid, end, compare);
    std::vector<T> result(end - begin);

    int i = 0, j = 0, k = 0;
    while (i < left.size() && j < right.size())
        result[k++] = compare(left[i], right[j]) ? left[i++] : right[j++];

    while (i < left.size())
        result[k++] = left[i++];

    while (j < right.size())
        result[k++] = right[j++];

    return result;
}

template <typename Comparator>
void test(size_t size)
{
    std::vector<int> v(size);
    for (int & i : v)
        i = rand();

    auto sorted = merge_sort<int>(v.begin(), v.end(), Comparator());
    std::sort(v.begin(), v.end(), Comparator());

    if (sorted != v)
        std::cout << "Test Failed..." << std::endl;
    else
        std::cout << "Passed!" << std::endl;
}

int main()
{
    test<std::less<>>(1000000);
    test<std::greater<>>(1000000);
}