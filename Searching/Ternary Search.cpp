#include <iostream>
#include <vector>

// https://cp-algorithms.com/num_methods/ternary_search.html
// https://www.youtube.com/watch?v=7h86n97naH4

// Better interval search methods:
//  - Dichotomous Search
//  - Fibonacci Search
//  - Golden-section Search

// Given a function f(x) which is unimodal on an interval
//  [l, r], ternary search finds its global maximum or global
//  minimum in the interval [l, r].
// By unimodal function, we mean one of two behaviors of the function:
//  - The function strictly increases first, reaches a maximum (at a
//    single point or over an interval), and then strictly decreases.
//  - The function strictly decreases first, reaches a minimum, and
//    then strictly increases.

// You might think that, from the name,
//  this algorithm divides the search
//  spaces by 3 on each iteration, thus
//  it has a runtime complexity of log_3(n),
//  but this is not the case. This algorithm
//  drops 1/3 of the search space on each
//  iteration, thus it has a runtime complexity
//  of log_1.5(n).

template <typename T>
using IT = typename std::vector<T>::const_iterator;

template <typename T>
bool min_it_comparator(const IT<T>& a, const IT<T>& b) { return *a < *b; };

template <typename T>
bool max_it_comparator(const IT<T>& a, const IT<T>& b) { return *a > *b; };

template <typename T>
bool min_num_comparator(const T& a, const T& b) { return a < b; };

template <typename T>
bool max_num_comparator(const T& a, const T& b) { return a > b; };

template <typename T, typename Function>
struct min_func_comparator
{
    Function function;
    bool operator()(const T& a, const T& b) const {
        return min_num_comparator(function(a), function(b));
    }
};

template <typename T, typename Function>
struct max_func_comparator
{
    Function function;
    bool operator()(const T& a, const T& b) const {
        return max_num_comparator(function(a), function(b));
    }
};

template <typename Iterator, typename Comparator>
Iterator double_ternary_search(const Iterator& begin, const Iterator& end, const Comparator& compare, double epsilon = 1e-9)
{
    // When explaining, we will assume the function has a global maximum.
    //  The second scenario is completely symmetrical to the first.

    // Consider any 2 points m1, and m2 in this interval: l < m1 < m2 <r.
    //  We evaluate the function at m1 and m2, i.e. find the values of f(m1)
    //  and f(m2). Now, we get one of three options:
    //  - f(m1) < f(m2):
    //     The desired maximum can not be located on the left side of m1,
    //     i.e. on the interval [l,m1], since either both points m1 and m2
    //     or just m1 belong to the area where the function increases.
    //     In either case, this means that we have to search for the
    //     maximum in the segment [m1,r].
    //  - f(m1) > f(m2):
    //     This situation is symmetrical to the previous one: the maximum
    //     can not be located on the right side of m2, i.e. on the interval
    //     [m2,r], and the search space is reduced to the segment [l,m2].
    //  - f(m1) = f(m2):
    //     We can see that either both of these points belong to the area
    //     where the value of the function is maximized, or m1 is in the
    //     area of increasing values and m2 is in the area of descending
    //     values (here we used the strictness of function increasing/decreasing).
    //     Thus, the search space is reduced to [m1,m2]. To simplify the code,
    //     this case can be combined with any of the previous cases.
    // Thus, based on the comparison of the values in the two inner points, we
    //  can replace the current interval [l, r] with a new, shorter interval [l′, r′].
    //  Repeatedly applying the described procedure to the interval, we can get
    //  an arbitrarily short interval. Eventually, its length will be less than
    //  a certain pre-defined constant (accuracy), and the process can be stopped.
    //  This is a numerical method, so we can assume that after that the function
    //  reaches its maximum at all points of the last interval [l, r]. Without
    //  loss of generality, we can take f(l) as the return value.

    Iterator low = begin;
    Iterator high = end;

    while (high - low > epsilon)
    {
        // We didn't impose any restrictions on the choice of points m1 and m2. This
        //  choice will define the convergence rate and the accuracy of the implementation.
        //  The most common way is to choose the points so that they divide the interval
        //  [l,r] into three equal parts. Thus, we have
        // If m1 and m2 are chosen to be closer to each other, the convergence rate will
        //  increase slightly.
        Iterator m1 = low  + (high - low) / 3;
        Iterator m2 = high - (high - low) / 3;

        // The arguments are being passed in this order so that
        //  the writing of the comparators is more intuitive.
        //  For a max comparator, if the order of the arguments
        //  was m1 then m2, it would've been:
        //      bool comp(a, b) { return a < b; }
        //  Writing < for max comparators and > for min comparators
        //  is a bit confusing.
        if (compare(m2, m1))
            low = m1;
        else
            high = m2;
    }

    return low;
}

template <typename T, typename Function>
T double_ternary_search_min(const T& begin, const T& end, Function function) {
    return double_ternary_search(begin, end, min_func_comparator<T, Function>{function});
}

template <typename T, typename Function>
T double_ternary_search_max(const T& begin, const T& end, Function function) {
    return double_ternary_search(begin, end, max_func_comparator<T, Function>{function});
}

template <typename Iterator, typename Comparator>
Iterator integer_ternary_search(const Iterator& begin, const Iterator& end, const Comparator& compare)
{
    // If f(x) takes integer parameter, the interval
    //  [low, high] becomes discrete. Since we did not
    //  impose any restrictions on the choice of points
    //  m1 and m2, the correctness of the algorithm is not
    //  affected. m1 and m2 can still be chosen to divide
    //  [low, high] into 3 approximately equal parts.
    // The difference occurs in the stopping criterion
    //  of the algorithm. Ternary search will have to stop
    //  when (high - low) < 3, because in that case we can
    //  no longer select m1 and m2 to be different from each
    //  other as well as from low and high, and this can cause
    //  an infinite loop. Once (high - low) < 3, the remaining
    //  pool of candidate points (low, low+1, …, high) needs to
    //  be checked to find the point which produces the maximum
    //  value f(x).

    Iterator low = begin;
    Iterator high = end;

    while (high - low >= 3)
    {
        Iterator m1 = low  + (high - low) / 3;
        Iterator m2 = high - (high - low) / 3;

        if (compare(m2, m1))
            low = m1;
        else
            high = m2;
    }

    for (Iterator it = low + 1; it <= high; it++)
        if (compare(it, low))
            low = it;

    return low;
}

template <typename T, typename Iterator>
Iterator integer_ternary_search_min(const Iterator& begin, const Iterator& end) {
    return integer_ternary_search(begin, end, min_it_comparator<T>);
}

template <typename T, typename Iterator>
Iterator integer_ternary_search_max(const Iterator& begin, const Iterator& end) {
    return integer_ternary_search(begin, end, max_it_comparator<T>);
}

void test_integer_ternary_search()
{
    std::vector<int> min_arr = {5, 4, 3, 2, 1, 1, 1, 2, 3, 4, 5};
    std::cout << "Array: ";
    for (int value : min_arr)
        std::cout << value << ' ';
    std::cout << std::endl;
    auto result = integer_ternary_search_min<int>(min_arr.begin(), min_arr.end());
    std::cout << "Result: " << *result << " at index " << result - min_arr.begin() << "." << std::endl;

    std::vector<int> max_arr = {1, 2, 3, 4, 5, 5, 5, 4, 3, 2, 1};
    std::cout << "Array: ";
    for (int value : max_arr)
        std::cout << value << ' ';
    std::cout << std::endl;
    result = integer_ternary_search_max<int>(max_arr.begin(), max_arr.end());
    std::cout << "Result: " << *result << " at index " << result - max_arr.begin() << "." << std::endl;

    std::cout << std::endl;
}

double max_unimodal_function_example(double x)
{
    double max = 1.23456789;
    if (x < max) return x;
    return max - x;
}

double min_unimodal_function_example(double x)
{
    return -max_unimodal_function_example(x);
}

void test_double_ternary_search()
{
    double begin = -10;
    double end = 10;

    std::cout << "Result: " << std::endl;
    std::cout << double_ternary_search_min(begin, end, min_unimodal_function_example) << std::endl;
    std::cout << double_ternary_search_max(begin, end, max_unimodal_function_example) << std::endl;
    std::cout << std::endl << std::endl;
}

int main()
{
    test_integer_ternary_search();
    test_double_ternary_search();
}