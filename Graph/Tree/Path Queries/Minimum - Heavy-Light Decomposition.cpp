#include <iostream>
#include <vector>

const int MAX_VALUE = INT_MAX;

template <typename T>
struct TreeNode : public std::vector<int>
{
    T value;
};

template <typename T>
using Tree = std::vector<TreeNode<T>>;

struct Range
{
    int start;
    int end;
};

template <typename T>
class MinSegmentTree
{
    // MAKE SURE TO USE A DATA TYPE THAT WON'T
    //  OVERFLOW. THE RESULTS ARE NOT GUARANTEED
    //  TO BE CORRECT ONCE AN OVERFLOW OCCURS.

    // The leaves will not be updated with
    //  their delta values (will always be 0).
    // Do we need both min and delta for the
    //  leaves or is only one is enough?

    const int root = 0;

    // Min and delta are set to be mutable
    //  to allow for pushing the deltas down
    //  the tree for const segment trees.
    mutable std::vector<T> min;
    mutable std::vector<T> delta;
    std::vector<Range> ranges;

    int left(int parent) const {
        return parent * 2 + 1;
    }

    int right(int parent) const {
        return left(parent) + 1;
    }

    bool is_complete_coverage(int node, const Range& range) const
    {
        // The given range covers the range of this
        //  node completely
        return ranges[node].start >= range.start &&
               ranges[node].end   <= range.end;
    }

    bool is_out_of_range(int node, const Range& range) const
    {
        return ranges[node].start > range.end ||
               ranges[node].end   < range.start;
    }

    void push_delta(int node) const
    {
        delta[left (node)] += delta[node];
        delta[right(node)] += delta[node];
        delta[node] = 0;
    }

    void increment(int node, const Range& range, const T& value)
    {
        if (is_out_of_range(node, range))
            return;

        // You can only update the delta and
        // array and return in case of a complete
        // coverage, otherwise, you'll have to
        // keep incrementing the children.

        if (is_complete_coverage(node, range))
        {
            delta[node] += value;
            return;
        }

        int l = left (node);
        int r = right(node);

        // Partial coverage case
        push_delta(node);
        increment(l, range, value);
        increment(r, range, value);
        min[node] = std::min(min[l] + delta[l], min[r] + delta[r]);
    }

    T query(int node, const Range& range) const
    {
        if (is_out_of_range(node, range))
            return MAX_VALUE;

        if (is_complete_coverage(node, range))
            return min[node] + delta[node];

        int l = left (node);
        int r = right(node);

        // Partial coverage case
        min[node] += delta[node];
        push_delta(node);
        T result = std::min(query(l, range), query(r, range));
        return result;
    }

    void init_ranges(int node, const Range& range)
    {
        ranges[node] = range;

        // a leaf node
        if (range.start == range.end)
            return;

        int middle = (range.start + range.end) / 2;
        init_ranges(left (node), {range.start, middle});
        init_ranges(right(node), {middle + 1, range.end});
    }

    void init(const std::vector<T>& array)
    {
        // A complete binary tree with n
        //  leaves will have n - 1 internal
        //  nodes (a total of 2n -1 nodes).
        // We have no guarantee that the
        //  binary tree will be complete,
        //  thus, we have to reserve one layer
        //  deeper in the tree to count for the
        //  case where the tree is not complete.
        // On each layer of a binary tree, the
        //  number of nodes double. This means
        //  that for us to reserve for the layer
        //  below the leaves (with n nodes), we'll
        //  have to reserve additional 2n nodes.
        int n = 4 * array.size() + 1;
        min.resize(n);
        delta.resize(n);
        ranges.resize(n);

        init_ranges(root, {0, (int)array.size() - 1});

        for (int i = 0; i < array.size(); i++)
            increment({i, i}, array[i]);
    }

public:

    void increment(const Range& range, const T& value) {
        return increment(root, range, value);
    }

    T query(const Range& range) const {
        return query(root, range);
    }

    explicit MinSegmentTree(const std::vector<T>& array) {
        init(array);
    }
};

template <typename T>
class MinHeavyLightDecomposition
{
    const Tree<T>& tree;
    MinSegmentTree<T> segment_tree;
    std::vector<int> depths;
    std::vector<int> parent_of;
    std::vector<int> chain_root;
    std::vector<int> heavy_child;
    std::vector<int> index_mapping;

    int segment_tree_index;
    int lower_node, higher_node;
    Range range;

    bool is_leaf_node(int node) {
        return tree[node].empty();
    }

    int init_subtree(int node, int depth)
    {
        depths[node] = depth;

        int subtree_size = 1;

        if (!is_leaf_node(node))
        {
            int max_child = node, max_size = 0;

            for (int child : tree[node])
            {
                parent_of[child] = node;
                int child_size = init_subtree(child, depth + 1);
                subtree_size += child_size;

                if (child_size > max_size) {
                    max_child = child;
                    max_size = child_size;
                }
            }

            heavy_child[node] = max_child;
        }

        return subtree_size;
    }

    void set_chain_root(int parent)
    {
        if (is_leaf_node(parent))
            return;

        int heavy = heavy_child[parent];
        chain_root[heavy] = chain_root[parent];
        set_chain_root(heavy);
    }

    void set_index_mapping(int node)
    {
        index_mapping[node] = segment_tree_index++;

        if (is_leaf_node(node))
            return;

        // We assign the next index to the heavy
        //  child first to ensure that indices
        //  in a heavy chain are consecutive.
        int heavy = heavy_child[node];
        set_index_mapping(heavy);

        for (int child : tree[node]) {
            if (child != heavy)
                set_index_mapping(child);
        }
    }

    void set_next_range()
    {
        // Lower node = bigger in depth.
        if (depths[chain_root[lower_node]] < depths[chain_root[higher_node]])
            std::swap(lower_node, higher_node);

        range = {chain_root[lower_node], lower_node};

        lower_node = parent_of[chain_root[lower_node]];
    }

    void map_current_range()
    {
        // This method maps the indices
        //  of the current range from
        //  tree node indices to the
        //  corresponding indices in
        //  the segment tree.
        int l = index_mapping[range.start];
        int r = index_mapping[range.end];
        range = {l, r};
    }

    T query_current_range() {
        map_current_range();
        return segment_tree.query(range);
    }

    void increment_current_range(const T& value) {
        map_current_range();
        return segment_tree.increment(range, value);
    }

    bool is_in_same_chain() {
        return chain_root[lower_node] == chain_root[higher_node];
    }

    void init_segment_tree()
    {
        for (int i = 0; i < index_mapping.size(); i++) {
            range = {i, i};
            increment_current_range(tree[i].value);
        }
    }

    void init(int root)
    {
        depths.resize(tree.size());
        parent_of.resize(tree.size());
        chain_root.resize(tree.size());
        heavy_child.resize(tree.size());
        index_mapping.resize(tree.size());

        init_subtree(root, 0);

        for (int i = 0; i < chain_root.size(); i++)
            chain_root[i] = i;

        set_chain_root(root);

        segment_tree_index = 0;
        set_index_mapping(root);

        init_segment_tree();
    }

public:

    MinHeavyLightDecomposition(const Tree<T>& tree, int root)
            : tree(tree), segment_tree(std::vector<T>(tree.size(), 0))
    {
        init(root);
    }

    T query(int node1, int node2)
    {
        // Fact: any path in the tree from x to y can be broken
        //  down to the chain from the LCA to x + the chain from
        //  the LCA to y. This makes the calculations much simpler.
        //  For a given query on any path, we can just break the
        //  query in to two queries, a query on the chain from the
        //  LCA to x, and a query on the chain from the LCA to y.
        // Every non-leaf node will have exactly one "heavy" child,
        //  the rest of the nodes are "light" children. A heavy
        //  child of a node is the child with the biggest subtree size.
        // Fact: the number of light edges in any given chain
        //  in a tree will be O(log(n)) where n is the number
        //  of nodes in the tree.
        // If the above fact is true, then also the number of
        //  heavy chains (not heavy edges) in a given chain
        //  is also O(log(n)). This is because heavy chains
        //  can only be broken by light edges, and there are
        //  only O(log(n)) of them.
        // Proof: For a parent node p, p has to have at least
        //  2 children to have a light child. This means that
        //  the size of the subtree of the heavy child is bigger
        //  than the size of the subtree of the light child,
        //  which means that the size of the subtree of the
        //  light child is < (size of subtree at p) / 2.
        //  This division by 2 can only happen log(n) of times.
        // Something to note here is that heavy edges always
        //  form chains down to the leaves. We can store for each
        //  heavy node, the root of it's chain (the node at which
        //  the heavy chain begins). This will enable us to
        //  jump directly from a given node in the heavy chain
        //  to the top of the chain, which enables us to treat a
        //  heavy CHAIN as a single edge. For light edges, we'll
        //  consider every edge to be a chain on its own. This
        //  way, the number of heavy chains and light chains in any
        //  given chain (thus, any given path) in the tree is O(log(n)).
        // This enables us to start from the 2 given nodes,
        //  and go up step by step (chain by chain) until the 2
        //  nodes meet. The only problem with this approach is that
        //  the LCA can be in the middle of a heavy chain, in
        //  which case it might be missed. To avoid this problem,
        //  we'll always go up from the node with the lower (bigger
        //  depth) chain root, and check if the CHAIN ROOTS of
        //  both nodes are the same, not the nodes themselves.
        //  We go up from the node with lower chain root to ensure
        //   that we don't miss the LCA and go to one of its ancestors.
        //  It's obvious that once we find that the chain roots of the
        //   two nodes are the same, then the higher node is the LCA.
        //   In case the LCA was in a middle of a heavy chain, we can't
        //   guarantee that the lower node will be the LCA as well.
        //   If the two nodes are not the same (are not at the LCA), then
        //    there is a part of a heavy chain from the higher node
        //    (the LCA) to the lower node that is left to be processed.
        //  While going up a chain, we'll perform a query on it. This
        //   means that we're gonna only make a O(log(n)) queries.
        //  But, how are we going to make queries on chains? Since chains
        //   are straight, with no branches, we can use a segment tree
        //   for each chain, and for each chain, query its own segment
        //   tree. This will work, but there is a simpler approach. We
        //   can use only 1 segment tree, and map each chain to a
        //   certain range in the segment tree. This way, we only have
        //   a single segment tree. We can map each chain to a range
        //   in the segment tree by giving every node in the tree
        //   an index in the segment tree, and make the indices of
        //   the chains consecutive. Note that light chains consists only
        //   of one node, which simplifies the situation. We only need
        //   to make the nodes along a heavy edge to be consecutive. This
        //   can be done by always assigning the heavy child first upon
        //   entering a node. This way, we can query any range on a chain.
        //  After making queries on the chains, we still need to make the
        //    last query on the LCA itself since it's not included yet.

        T result = MAX_VALUE;

        // We don't have to check here for the depths.
        // The function set_next_range will take care of this.
        lower_node = node1;
        higher_node = node2;

        while (!is_in_same_chain()) {
            set_next_range();
            result = std::min(result, query_current_range());
        }

        if (depths[lower_node] < depths[higher_node])
            std::swap(lower_node, higher_node);

        // By now, the higher node is at the LCA, but we
        //  don't know about the lower node yet.
        // We have 2 cases here:
        // 1 - higher_node != lower_node: in which case,
        //   we know that higher_node and lower_node are
        //   both on a heavy chain. We only need to query
        //   a part of the current heavy chain from the
        //   higher_node to the lower_node, not the whole
        //   chain. We can make use of the fact that the
        //   indices of nodes in a heavy chain are consecutive,
        //   we've set it to be like that by always assigning
        //   an index to the heavy child of the current node first.
        //  Because the indices are consecutive, we can guarantee
        //   that the chain from the higher_node to the lower_node
        //   can be defined in the segment tree by the range
        //   {higher_node, lower_node}. Note that this range
        //   will include the LCA, so we don't have to query
        //   the LCA later.
        // 2 - higher_node == lower_node (== LCA): in
        //   which case, we only need to query the LCA,
        //   or in other words, the chain from the LCA
        //   to the LCA, which can be defined by the range
        //   {LCA, LCA}. But, we know that both higher_node
        //   and lower_node are the same as the LCA, for this
        //   reason, again, we can just query the range
        //   {higher_node, lower_node} in the segment tree.

        range = {higher_node, lower_node};
        result = std::min(result, query_current_range());

        return result;
    }

    void increment(int node1, int node2, const T& value)
    {
        // The logic is exactly the
        // same as in the query method.

        lower_node = node1;
        higher_node = node2;

        while (!is_in_same_chain()) {
            set_next_range();
            increment_current_range(value);
        }

        if (depths[lower_node] < depths[higher_node])
            std::swap(lower_node, higher_node);

        range = {higher_node, lower_node};
        increment_current_range(value);
    }
};

void test(const Tree<int>& tree, int root, const std::vector<std::pair<int, int>>& queries)
{
    MinHeavyLightDecomposition<int> hdl(tree, root);

    for (auto& query : queries)
    {
        int x = query.first;
        int y = query.second;
        std::cout << "Minimum value along the path from " << x << " to " << y
                  << " is " << hdl.query(x, y) << std::endl;
    }
    std::cout << std::endl;

    for (int i = 0; i < tree.size(); i++) {
        for (int j = i; j < tree.size(); j++)
        {
            int min = hdl.query(i, j);
            int val = i * j + 10;
            hdl.increment(i, j, val);
            min += val;
            int query = hdl.query(i, j);
            if (query != min)
                std::cout << "Incorrect Result." << std::endl;
        }
    }
}

Tree<int> get_sample_tree_1()
{
    // root = 0
    Tree<int> tree(14);

    std::vector<int> values = {13, 4, 20, 2, 3, 2, 4, 1, 8, 2, 5, 3, 1, 1};
    for (int i = 0; i < values.size(); i++)
        tree[i].value = values[i];

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