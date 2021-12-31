#include <iostream>
#include <vector>

typedef std::vector<std::vector<int>> Tree;

int log2_floor(int x)
{
    int result = 0;
    while (x >>= 1) result++;
    return result;
}

class LowestCommonAncestor
{
    const Tree& tree;

    std::vector<int> depths;

    // ancestors[k][i] = the
    //  (2^k)th ancestor of the node i.
    std::vector<std::vector<int>> ancestors;

    int i, j;

    void dfs(int node, int depth)
    {
        depths[node] = depth;
        for (int child : tree[node]) {
            ancestors[0][child] = node;
            dfs(child, depth + 1);
        }
    }

    void init_ancestors()
    {
        // This assumes that ancestors[0] is already filled.
        // We can compute ancestors[k] from ancestors[k-1].
        //  Example: for k = 2, ancestors[2][x] stores the 2^2
        //   = 4th ancestor of node x. We can compute this by
        //   making 2 jumps of size 2^(k - 1) = 2^1 = 2. First,
        //   we jump from x to its 2nd ancestor, call it y, then
        //   we jump from y to its 2nd ancestor, and this should
        //   be the 4th ancestor of node x.
        for (int k = 1; k < ancestors.size(); k++) {
            for (int node = 0; node < tree.size(); node++) {
                int ancestor = ancestors[k - 1][node];
                if (ancestor == -1) {
                    ancestors[k][node] = -1;
                } else {
                    ancestors[k][node] = ancestors[k - 1][ancestor];
                }
            }
        }
    }

    void init(int root)
    {
        // You can figure out the root by looking
        //  at the node that is not a child of any
        //  other node (assuming this is a rooted
        //  tree), but it's not worth the time.
        //  Just take it as a parameter.

        int n = tree.size();
        int max_power = log2_floor(n);

        depths.resize(n);
        ancestors.resize(max_power + 1, std::vector<int>(n));

        ancestors[0][root] = -1;
        dfs(root, 0);

        init_ancestors();
    }

    int parent_of(int x) {
        return ancestors[0][x];
    }

    void set_at_same_depth()
    {
        // This assumes that i <= j.
        int diff = depths[j] - depths[i];

        while (diff)
        {
            int biggest_power_of_2 = log2_floor(diff);
            j = ancestors[biggest_power_of_2][j];
            diff -= (1 << biggest_power_of_2);
        }
    }

public:

    int get_LCA(int _i, int _j)
    {
        i = _i;
        j = _j;
        if (depths[i] > depths[j])
            std::swap(i, j);

        // Note: all ancestors of the LCA are also
        //  common between nodes i and j.
        // If the two nodes are not at the same level,
        //  we'll have to calculate different jump lengths
        //  for each node. To avoid this, we'll first
        //  ensure that we make the two nodes at the
        //  same level.
        //  Since we know the depth of the two nodes, we can
        //  calculate exactly how long should the jump be.
        //  For a jump that is not a power of 2, we can
        //  break it into multiple jumps that are powers
        //  of 2. For example, to jump to the 9th ancestor,
        //  we can make a jump of size 8, then a jump of
        //  size 1.
        // After having the two nodes at the same level,
        //  we want to jump to the LCA, but this time, we
        //  don't know how much of a jump should be done.
        //  To overcome this problem, we can just aim for
        //  the node that is 1 jump from the LCA (direct
        //  child of the LCA). This makes it easier since
        //  all we have to do is to make the biggest jumps
        //  possible that don't make i and j land at a
        //  common node (the LCA or any of its ancestors).
        //  Once we can't jump anymore, then we know that
        //  the parent of i or j is the LCA.

        set_at_same_depth();

        // In case i was an
        // ancestor of j or i = j.
        if (i == j)
            return i;

        int k = log2_floor(depths[i]);

        for (; k >= 0; k--) {
            if (ancestors[k][i] != ancestors[k][j]) {
                i = ancestors[k][i];
                j = ancestors[k][j];
            }
        }

        return parent_of(i);
    }

    LowestCommonAncestor(const Tree& tree, int root) : tree(tree)
    {
        init(root);
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