#include <iostream>
#include <vector>
#include <map>

// Given an array and a value, return 4 values
//  from the array that sum up to the given value.

template <typename T>
std::vector<T> get_values(const std::vector<T>& array, const T& value)
{
    // The technique used here is called "Meet in the Middle".
    // It would take O(n^4) to iterate over all possible 4 numbers.
    // The idea here is to compute the sums of all pairs of numbers,
    //  then iterate over all possible pairs of pairs.
    // We can do this in 2 ways:
    //  1 - Store the sums of all pairs of numbers, then iterate over
    //      each possible pair from those numbers. This way is not
    //      very good, because we'll have to keep track of indices
    //      to make sure that we're not counting the same number twice.
    //  2 - To only store the pairs of numbers we've gone over so far.
    //      This is what is being done here. This ensures that if we
    //      find a pair that we can use, the indices are not conflicting
    //      for sure.
    // This should take O(n^2) instead of O(n^4), ignoring the log factor
    //  which we can get rid of easily.

    // map[x] = indices of the pair of
    //  numbers that sums up to x.
    std::map<T, std::pair<int, int>> map;

    for (int i = 0; i < array.size(); i++)
    {
        for (int j = i + 1; j < array.size(); j++) {
            T desired_value = value - array[i] - array[j];
            auto it = map.find(desired_value);
            if (it != map.end()) {
                return {it->second.first, it->second.second, i, j};
            }
        }

        for (int k = 0; k < i; k++) {
            map[array[i] + array[k]] = {k, i};
        }
    }

    return {};
}

void test(const std::vector<int>& array, int value)
{
    std::cout << "Array: ";
    for (int i : array)
        std::cout << i << ' ';
    std::cout << std::endl;

    auto result = get_values(array, value);

    if (result.empty()) {
        std::cout << "There are no 4 values that sums up to " << value;
        std::cout << std::endl << std::endl;
        return;
    }

    std::cout << "Four numbers that sum up to " << value << " are:" << std::endl;
    for (int i : result)
        std::cout << array[i] << " at index " << i << std::endl;

    std::cout << std::endl;
}

int main()
{
    test({1, 2, 3, 4, 5, 6, 7, 8, 9}, 20);
    test({1, 2, 4, 6, 7}, 20);
}