#include <iostream>
#include <vector>

template <typename T>
class CountInversions
{
    unsigned long long inversions = 0;

    std::vector<T> merge_sort(
        const typename std::vector<T>::iterator& begin,
        const typename std::vector<T>::iterator& end
    )
    {
        if (begin + 1 == end)
            return { *begin };

        auto mid = begin + (end - begin) / 2;
        std::vector<T> left = merge_sort(begin, mid);
        std::vector<T> right = merge_sort(mid, end);
        std::vector<T> result(end - begin);

        int i = 0, j = 0, k = 0;
        while (i < left.size() && j < right.size()) {
            // When writing this condition, make sure
            //  you don't count equal items as an inversion.
            //  Example: check if left[i] < right[j], and
            //   count the inversion in the else block.
            if (left[i] > right[j]) {
                // All the items from left[i] up to
                //  left.back() forms an inversion with
                //  right[j]
                inversions += left.size() - i;
                result[k++] = right[j++];
            } else {
                result[k++] = left[i++];
            }
        }

        while (i < left.size())
            result[k++] = left[i++];

        while (j < right.size())
            result[k++] = right[j++];

        return result;
    }

public:

    unsigned long long compute(
        const typename std::vector<T>::iterator& begin,
        const typename std::vector<T>::iterator& end
    )
    {
        inversions = 0;
        merge_sort(begin, end);
        return inversions;
    }
};

template <typename T>
unsigned long long count_inversions_slow(
    const typename std::vector<T>::iterator& begin,
    const typename std::vector<T>::iterator& end
)
{
    unsigned long long inversions = 0;
    for (int i = 0; begin + i != end; i++)
        for (int j = i + 1; begin + j != end; j++)
            if (*(begin + j) < *(begin + i))
                inversions++;
    return inversions;
}

void test(size_t size)
{
    std::vector<int> v(size);
    for (int & i : v)
        i = rand();

    auto inversions1 = CountInversions<int>().compute(v.begin(), v.end());
    auto inversions2 = count_inversions_slow<int>(v.begin(), v.end());

    if (inversions1 != inversions2)
        std::cout << "Test Failed..." << std::endl;
    else
        std::cout << "Passed!" << std::endl;
}

int main()
{
    test(1000);
    test(10000);
}