#include <iostream>
#include <vector>
#include <map>

#define pair(x) std::pair<x, x>

template <typename T>
class Split2EqualSums
{
    T sum;
    bool result_found;
    const std::vector<T>& array;
    std::map<pair(T), pair(std::vector<T>)> first_half_combinations;

    void set_sum()
    {
        sum = 0;
        for (auto& i : array)
            sum += i;
    }

    void set_first_half_combinations(int i, pair(T) current_sums, pair(std::vector<T>)& current_elements)
    {
        if (i > array.size() / 2) {
            first_half_combinations[current_sums] = current_elements;
            return;
        }

        current_sums.first += array[i];
        current_elements.first.push_back(array[i]);
        set_first_half_combinations(i + 1, current_sums, current_elements);
        current_elements.first.pop_back();
        current_sums.first -= array[i];

        current_sums.second += array[i];
        current_elements.second.push_back(array[i]);
        set_first_half_combinations(i + 1, current_sums, current_elements);
        current_elements.second.pop_back();
    }

    pair(std::vector<T>) check_other_half(int i, pair(T) current_sums, pair(std::vector<T>)& current_elements)
    {
        if (i == array.size()) {
            T half = sum / 2;
            pair(T) desired_sums = {half - current_sums.first, half - current_sums.second};
            auto it = first_half_combinations.find(desired_sums);
            if (it != first_half_combinations.end()) {
                result_found = true;
                for (auto& num : it->second.first)
                    current_elements.first.push_back(num);
                for (auto& num : it->second.second)
                    current_elements.second.push_back(num);
                return current_elements;
            } else {
                return {};
            }
        }

        current_sums.first += array[i];
        current_elements.first.push_back(array[i]);
        auto result = check_other_half(i + 1, current_sums, current_elements);
        if (result_found)
            return result;
        current_elements.first.pop_back();
        current_sums.first -= array[i];

        // There are two cases for the returned value:
        //  - result is found, in which case, return result
        //  - result is not found, in which case, result = {},
        //    return {}, which is equivalent to returning result.
        current_sums.second += array[i];
        current_elements.second.push_back(array[i]);
        result = check_other_half(i + 1, current_sums, current_elements);
        current_elements.second.pop_back();
        return result;
    }

public:

    Split2EqualSums(const std::vector<T>& array) : array(array) {}

    std::pair<std::vector<T>, std::vector<T>> split()
    {
        /*
         * To compute this, it would take 2^n to check for all combinations of the numbers.
         * The idea here is to split the array into two equal halves, compute and store
         *  all possible combinations of the first half, then compute all possible combinations
         *  of the second half, and while computing for the second half, check if the
         *  complements of the current combination have appeared when computing the
         *  combinations for the first half.
         * The technique used here is called "Meet in the Middle".
         * This will take 2^(n / 2) instead of 2^n. This is ignoring the log factor, which
         *  we can get rid of easily.
         */

        set_sum();
        if (sum % 2 == 1) {
            // There is no way to split the array into two
            //  equal sums because the sum of the numbers
            //  in the array is not divisible by 2.
            return {};
        }

        pair(std::vector<T>) temp;
        set_first_half_combinations(0, {0, 0}, temp);

        result_found = false;
        return check_other_half((array.size() + 1) / 2, {0, 0}, temp);
    }
};

void test(const std::vector<int>& array)
{
    std::cout << "Array: ";
    for (int i : array)
        std::cout << i << ' ';
    std::cout << std::endl;

    int sum = 0;
    for (int i : array)
        sum += i;
    std::cout << "Sum: " << sum << std::endl;

    auto result = Split2EqualSums<int>(array).split();

    if (result == pair(std::vector<int>){}) {
        std::cout << "There is no way to split the array into to equal sums";
        std::cout << std::endl << std::endl;
        return;
    }

    std::cout << "Set 1: ";
    for (int i : result.first)
        std::cout << i << ' ';
    std::cout << std::endl;

    std::cout << "Set 2: ";
    for (int i : result.second)
        std::cout << i << ' ';
    std::cout << std::endl;

    std::cout << std::endl;
}

int main()
{
    test({1, 2, 3});
    test({1, 2, 3, 4, 5, 9, 6, 4, 2, 3, -1});
    test({23, 23, 1, 34, 5, 65, 55});
}