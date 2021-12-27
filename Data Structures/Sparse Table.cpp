#include <iostream>
#include <vector>
#include <chrono>

// func = min, max, gcd, sum... (any associative function)
// A function f is overlap-friendly if f(f(a, b), f(b, c)) = f(a, f(b, c)).
//  min is an overlap-friendly function while sum is not.
//  Queries for overlap-friendly functions can be computed
//  in O(1) while for non-overlap-friendly functions, the
//  queries are done in O(log(n)).
template <typename T, typename functor, bool is_overlap_friendly, bool is_indexable, bool precompute_log = true>
class SparseTable
{
    int n;
    int max_depth; // floor(log2(n))
    functor function;

    std::vector<std::vector<T>> table;

    // Used to get the index of the result element.
    //  In other words, this is a reverse mapping
    //  of the table. THIS IS USED WITH FUNCTIONS
    //  LIKE MIN, MAX, ..., BUT NOT GCD, SUM, ...
    //  We'll call this kind of functions
    //  "indexable functions".
    std::vector<std::vector<int>> index;

    // Used to compute floor(log2(i))
    //  in O(1) for any index i <= n.
    std::vector<int> log2floor;

    void resize_table(std::vector<std::vector<T>>& table) {
        table.resize(max_depth + 1, std::vector<T>(n));
    }

    int compute_log2_floor(int x)
    {
        // We won't get x = 0 since there is no 0 length.
        int result = 0;
        while (x >>= 1) result++;
        return result;
    }

    int log2_floor(int x)
    {
        if (precompute_log)
            return log2floor[x];
        return compute_log2_floor(x);
    }

    void init_table()
    {
        for (int depth = 1; depth <= max_depth; depth++)
        {
            int range = 1 << depth;

            // We check for the element at index
            //  i + range - 1 being withing the
            //  bounds of the array. We don't need
            // This is equivalent to i + range - 1 < n.
            for (int i = 0; i + range <= n; i++)
            {
                int prev_depth = depth - 1;
                int prev_range = (1 << prev_depth);

                T left = table[prev_depth][i];
                T right = table[prev_depth][i + prev_range];
                table[depth][i] = function(left, right);

                if (is_indexable) {
                    index[depth][i] =
                            (table[depth][i] == left) ?
                            index[prev_depth][i] :
                            index[prev_depth][i + prev_range];
                }
            }
        }
    }

    void init(const std::vector<T>& arr)
    {
        n = arr.size();

        if (precompute_log) {
            log2floor.resize(n + 1);
            for (int i = 2; i <= n; i++)
                log2floor[i] = log2floor[i / 2] + 1;
        }

        max_depth = log2_floor(n);

        resize_table(table);
        if (is_indexable)
            resize_table(index);

        for (int i = 0; i < n; i++) {
            table[0][i] = arr[i];
            if (is_indexable)
                index[0][i] = i;
        }

        init_table();
    }

    T query_o1(int l, int r)
    {
        int len = r - l + 1;
        int depth = log2_floor(len);
        int range = 1 << depth;
        T left = table[depth][l];
        T right = table[depth][r - range + 1];
        return function(left, right);
    }

    T query_ologn(int l, int r)
    {
        int len = r - l + 1;
        int depth = log2_floor(len);
        int range = 1 << depth;

        T result = table[depth][l];
        l += range;

        while (l <= r)
        {
            len = r - l + 1;
            depth = log2_floor(len);
            range = 1 << depth;

            result = function(result, table[depth][l]);
            l += range;
        }

        return result;
    }

public:

    SparseTable(const std::vector<T>& arr)
    {
        init(arr);
    }

    T query(int l, int r)
    {
        return (is_overlap_friendly ? query_o1(l, r) : query_ologn(l, r));
    }

    int query_index(int l, int r)
    {
        static_assert(is_indexable, "This type of sparse table is not indexable.");

        int len = r - l + 1;
        int depth = log2_floor(len);
        int range = 1 << depth;

        r = r - range + 1;

        T left = table[depth][l];
        T right = table[depth][r];

        if (function(left, right) == left)
            return index[depth][l];
        return index[depth][r];
    }
};

template <typename T>
struct min
{
    T operator()(const T& a, const T& b) { return std::min(a, b); }
};

template <typename T>
struct sum
{
    T operator()(const T& a, const T& b) { return a + b; }
};

template <typename T>
struct gcd
{
    T operator()(const T& a, const T& b) {
        if (b == 0) return a;
        return operator()(b, a % b);
    }
};

typedef SparseTable<int, min<int>, true , true > IntMinSparseTable;
typedef SparseTable<int, sum<int>, false, false> IntSumSparseTable;
typedef SparseTable<int, gcd<int>, true , false> IntGCDSparseTable;
typedef SparseTable<int, min<int>, true , false> NoIndexingIntMinSparseTable;


void min_test(int size)
{
    std::vector<int> arr;
    for (int i = 0; i < size; i++)
        arr.push_back(rand());

    IntMinSparseTable table(arr);

    for (int i = 0; i < arr.size(); i++) {
        for (int j = i; j < arr.size(); j++) {

            int result = INT_MAX;
            for (int k = i; k <= j; k++)
                result = std::min(result, arr[k]);

            bool correct_value = result == table.query(i, j);
            bool correct_index = arr[table.query_index(i, j)] == result;

            if (!correct_value)
                std::cout << "Wrong value!" << std::endl;

            if (!correct_index)
                std::cout << "Wrong index!" << std::endl;
        }
    }
}

void sum_test(int size)
{
    std::vector<int> arr;
    for (int i = 0; i < size; i++)
        arr.push_back(rand());

    IntSumSparseTable table(arr);

    for (int i = 0; i < arr.size(); i++) {
        for (int j = i; j < arr.size(); j++) {

            int result = 0;
            for (int k = i; k <= j; k++)
                result += arr[k];

            bool correct_value = result == table.query(i, j);

            if (!correct_value)
                std::cout << "Wrong value!" << std::endl;
        }
    }
}

void gcd_test(int size)
{
    std::vector<int> arr;
    for (int i = 0; i < size; i++)
        arr.push_back(rand());

    gcd<int> g;
    IntGCDSparseTable table(arr);

    int ms = 0;

    for (int i = 0; i < arr.size(); i++) {
        for (int j = i; j < arr.size(); j++) {

            int result = arr[i];
            for (int k = i + 1; k <= j; k++)
                result = g(result, arr[k]);

            auto start = std::chrono::high_resolution_clock::now();
            bool correct_value = result == table.query(i, j);
            auto end = std::chrono::high_resolution_clock::now();

            ms += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            if (!correct_value)
                std::cout << "Wrong value!" << std::endl;
        }
    }
}

template <typename TableType>
void time_test(int size)
{
    int ms = 0;
    std::cout << "Creating array..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<int> arr(size);
    for (int i = 0; i < size; i++)
        arr[i] = rand();
    auto end = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Array created (" << ms << " ms). Creating sparse table..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    TableType table(arr);
    end = std::chrono::high_resolution_clock::now();
    ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Sparse table crated (" << ms << " ms). Starting queries..." << std::endl;

    ms = 0; // micro-seconds this time.
    for (int i = 0; i < size; i++) {
        start = std::chrono::high_resolution_clock::now();
        int value = table.query(0, i);
        (void)value;
        end = std::chrono::high_resolution_clock::now();
        ms += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    std::cout << "Test done. Queries took " << ms << " micro-seconds." << std::endl << std::endl;
}

int main()
{
    int size = 1000000;
    time_test<IntMinSparseTable>(size);
    time_test<IntSumSparseTable>(size);
    time_test<NoIndexingIntMinSparseTable>(size);

    size = 1000;
    min_test(size);
    sum_test(size);
    gcd_test(size);
}