#include <iostream>
#include <vector>
#include <algorithm>

// This namespace is to avoid conflicts due to ADL (argument dependent lookup).
namespace BinarySearch
{

//  Quick Cheat Sheet:
//  ╔═══════════════════════════════╤════════════════════════════════════════════════════╗
//  ║ What should the bounds be?    │ Usually, low = 0, and high = container.size()      ║
//  ╠═══════════════════════════════╪════════════════════════════════════════════════════╣
//  ║ How to calculate mid?         │ For the standard lower and upper bounds,           ║
//  ║                               │ always = low + (high - low) / 2                    ║
//  ╟───────────────────────────────┼────────────────────────────────────────────────────╢
//  ║ When to use >= or <=?         │ >= is for lower bounds, and <= is for upper bounds ║
//  ╟───────────────────────────────┼────────────────────────────────────────────────────╢
//  ║ How should the pointers move? │ For the standard lower and upper bounds, always    ║
//  ║                               │ set low = mid + 1, and high = mid                  ║
//  ╟───────────────────────────────┼────────────────────────────────────────────────────╢
//  ║ What value to return?         │ low or high, both are the same                     ║
//  ╟───────────────────────────────┼────────────────────────────────────────────────────╢
//  ║ When to round mid up and      │ Round down in case of low = mid + 1,               ║
//  ║ when to round it down?        │ round up in case of high = mid - 1                 ║
//  ╚═══════════════════════════════╧════════════════════════════════════════════════════╝

// NOTE:
// The functions here assumes that the containers used support
// random access. If they don't (e.g. linked list), the functions
// will take an O(n * log(n)) instead of O(log(n)), and a linear
// search would be better.

template <typename Iterator, typename T>
Iterator lower_bound(const Iterator& begin, const Iterator& end, const T& value)
{
    // This function returns the first position at
    //  which you can insert "value", and have the
    //  elements still be sorted. If the value is present
    //  in the container, it'll return the index of
    //  its first occurrence, and if it's not present,
    //  it'll return the index at which you can insert
    //  the value, and have the elements still sorted.

    // The parameter "end" should be pointing to 1 beyond
    //  the last element in the container. In case of STD
    //  containers, it's just container.end(). In case of
    //  a pointer to an array of size n, it's (ptr + n).

    // Note that high = last index + 1, not last
    // index. This is because if the value being
    // searched for is bigger than the biggest
    // value in the container, we'd expect last
    // index + 1 to be returned.
    Iterator low = begin;
    Iterator high = end;

    while (low < high)
    {
        // The mid is calculated this way for 2 reasons:
        //  1 - To avoid some potential overflows.
        //  2 - To avoid a rounding bug. This bug won't
        //      happen here since we're dealing with
        //      non-negative integers only, but if we
        //      were dealing with negative integers,
        //      for expression "(low + high) / 2",
        //      if low + high is negative, the expression
        //      will be rounded up, while if it's positive,
        //      the expression will be rounded down.
        //      This is because integer division always
        //      truncates, and the rounding changes
        //      depending on the sign. You can see that
        //      (high - low) is always positive, thus
        //      the expression "low + (high - low) / 2"
        //      will always round down.
        //      If you want it to always round up, write:
        //      "mid = low + (high - low + 1) / 2".
        // Here, we'd need to round down and not up. The
        //  reason is that the low pointer is the one that
        //  moves past the mid pointer, thus we want to
        //  set mid = low so that in case the number of
        //  elements in the range is 2, we don't get stuck
        //  in an infinite loop.
        Iterator mid = low + (high - low) / 2;

        // Here, if *mid == value, we'd like to keep
        //  mid in our searching spaces, because it's
        //  a potential result. This is why we set
        //  high = mid, and not high = mid - 1.
        // The reason this function returns the lower bound
        //  is that it moves the high pointer backwards in
        //  the case of *mid == value.
        // If you want to change this to a higher bound, change
        //  it to move the low pointer forwards in the case of
        //  *mid == value.
        if (*mid >= value)
            high = mid;
        else
            low = mid + 1;
    }

    // By now, low == high.
    // We can return any of them.
    return low;
}

template <typename Iterator, typename T>
Iterator upper_bound(const Iterator& begin, const Iterator& end, const T& value)
{
    // This function returns the last position at
    //  which you can insert "value", and have the
    //  elements still be sorted. If the value is present
    //  in the container, it'll return the index of
    //  its last occurrence + 1, and if it's not present,
    //  it'll return the index at which you can insert
    //  the value, and have the elements still sorted.

    Iterator low = begin;
    Iterator high = end;

    while (low < high)
    {
        // You might expect that here, we'll change
        //  the assignment to the low and the high
        //  pointers to be as following:
        //   low = mid, high = mid - 1
        //  But this is not the case. We will keep
        //  low = mid + 1 and high = mid because
        //  this is what makes it return 1 past
        //  the last occurrence of "value", and
        //  thus, the position at which you can
        //  insert "value".

        Iterator mid = low + (high - low) / 2;

        if (*mid <= value)
            low = mid + 1;
        else
            high = mid;
    }

    return low;
}

template <typename Iterator, typename T>
Iterator shifted_lower_bound(const Iterator& begin, const Iterator& end, const T& value)
{
    // This function returns the first position after
    //  which you can insert "value", and have the
    //  elements still be sorted. If the value is present
    //  in the container, it'll return the index of
    //  its first occurrence - 1, and if it's not present,
    //  it'll return the index after which you can insert
    //  the value, and have the elements still sorted.

    // This probably has no use, but just to demonstrate
    //  the effect of playing with the assignments of the
    //  low and the high pointers, and also to show how to
    //  calculate the mid pointer in such cases.

    // Note that low = first index - 1, not first
    // index. This is because if the value being
    // searched for is smaller than the smallest
    // value in the container, we'd expect first
    // index - 1 to be returned.
    Iterator low = begin - 1;
    Iterator high = end - 1;

    while (low < high)
    {
        // In this case, since the high pointer is
        // the one that goes past the mid pointer,
        // we'd need to round up to avoid infinite
        // loops.
        Iterator mid = low + (high - low + 1) / 2;

        // Here, the high pointer is the one that
        //  moves past the mid pointer.
        if (*mid >= value)
            high = mid - 1;
        else
            low = mid;
    }

    return low;
}

template <typename Iterator, typename T>
Iterator shifted_upper_bound(const Iterator& begin, const Iterator& end, const T& value)
{
    // This function returns the last position after
    //  which you can insert "value", and have the
    //  elements still be sorted. If the value is present
    //  in the container, it'll return the index of
    //  its last occurrence, and if it's not present,
    //  it'll return the index after which you can insert
    //  the value, and have the elements still sorted.

    Iterator low = begin - 1;
    Iterator high = end - 1;

    while (low < high)
    {
        Iterator mid = low + (high - low + 1) / 2;

        if (*mid <= value)
            low = mid;
        else
            high = mid - 1;
    }

    return low;
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
        int shifted_lb = index(BinarySearch::shifted_lower_bound);
        int shifted_ub = index(BinarySearch::shifted_upper_bound);

        #undef index

        if (my_lb != lb || my_ub != ub || shifted_lb != (lb - 1) || shifted_ub != (ub - 1)) {
            std::cout << "Fail..." << std::endl;
        }
    }
}

int main()
{
    test_lower_upper_bound({1, 1, 1, 1, 2, 3, 3, 3, 4, 5, 6, 6, 6, 7, 9});
}