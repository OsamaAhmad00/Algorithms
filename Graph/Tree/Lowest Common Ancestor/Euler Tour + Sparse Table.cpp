#include <iostream>
#include <vector>

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

    SparseTable() = default;
    SparseTable(const std::vector<T>& arr)
    {
        init(arr);
    }

    void set_array(const std::vector<T>& arr)
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

typedef SparseTable<int, min<int>, true , true> MinSparseTable;

typedef std::vector<std::vector<int>> Tree;

class LowestCommonAncestor
{
    const Tree& tree;

    // Stores the depth of
    // each node in the tree.
    std::vector<int> depths;

    std::vector<int> tour;

    // We can use any occurrence, but
    // the last is convenient to compute.
    std::vector<int> last_occurrence;

    MinSparseTable sparse_table;

    void construct_tour(int node, int depth)
    {
        tour.push_back(node);
        depths.push_back(depth);
        for (int child : tree[node]) {
            construct_tour(child, depth + 1);
            tour.push_back(node);
            depths.push_back(depth);
        }
    };

    void set_last_occurrence()
    {
        last_occurrence.resize(tree.size());

        for (int i = 0; i < tour.size(); i++)
            last_occurrence[tour[i]] = i;
    }

    void set_sparse_table() {
        sparse_table.set_array(depths);
    }

    void init(int root)
    {
        construct_tour(root, 0);
        set_last_occurrence();
        set_sparse_table();
    }

public:

    LowestCommonAncestor(const Tree& tree, int root) : tree(tree)
    {
        init(root);
    }

    int get_LCA(int node1, int node2)
    {
        //          0
        //         / \
        //        1   2
        //       / \ / \
        //      3  4 5  6
        // Consider the following tree, first, we make
        //  an Euler tour. The tour will be as following:
        //  0, 1, 3, 1, 4, 1, 0, 2, 5, 2, 6, 2, 0
        //  The depths of the nodes in the tour is:
        //  0, 1, 2, 1, 2, 1, 0, 1, 2, 1, 2, 1, 0
        // To get the LCA of x and y, we can get any
        //  occurrence of x and y, xi and yi in the tour,
        //  and get the node with the minimum depth in the
        //  range [xi, yi].
        // Note that we get the node with minimum depth,
        //  not minimum index.
        // Example:
        //  x = 3, y = 2:
        //  xi = 2, yi = 11
        //  LCA(3, 2) = node with min depth in
        //  {3, 1, 4, 1, 0, 2, 5, 2, 6, 2} = 0.

        int l = last_occurrence[node1];
        int r = last_occurrence[node2];
        if (l > r) std::swap(l, r);
        int index = sparse_table.query_index(l, r);
        return tour[index];
    }
};

void test(const Tree& tree, int root, const std::vector<std::pair<int, int>>& queries)
{
    LowestCommonAncestor lca(tree, root);
    for (auto& query : queries)
    {
        int x = query.first;
        int y = query.second;
        std::cout << "LCA of " << x << " and " << y
            << " is " << lca.get_LCA(x, y) << std::endl;
    }
    std::cout << std::endl;
}

Tree get_sample_tree_1()
{
    // root = 0
    Tree tree(14);

    tree[0].push_back(1);
    tree[0].push_back(2);
    tree[0].push_back(3);

    tree[1].push_back(4);

    tree[2].push_back(5);
    tree[2].push_back(6);

    tree[3].push_back(7);
    tree[3].push_back(8);

    tree[4].push_back(9);

    tree[6].push_back(10);

    tree[8].push_back(11);
    tree[8].push_back(12);

    tree[9].push_back(13);

    return tree;
}

std::vector<std::pair<int, int>> get_sample_queries_1()
{
    return {
        {4, 1},
        {7, 8},
        {1, 4},
        {4, 4},
        {12, 13},
        {5, 6},
        {2, 3}
    };
}

int main()
{
    test(get_sample_tree_1(), 0, get_sample_queries_1());
}