#include <iostream>
#include <vector>

typedef std::vector<std::vector<int>> Tree;

class LowestCommonAncestor
{
    const Tree& tree;
    std::vector<int> depths;
    std::vector<int> parent_of;
    std::vector<int> chain_root;
    std::vector<int> heavy_child;

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

        for (int child : tree[parent])
            set_chain_root(child);
    }

    void init(int root)
    {
        depths.resize(tree.size());
        parent_of.resize(tree.size());
        chain_root.resize(tree.size());
        heavy_child.resize(tree.size());

        init_subtree(root, 0);

        for (int i = 0; i < chain_root.size(); i++)
            chain_root[i] = i;
        set_chain_root(root);
    }

public:

    LowestCommonAncestor(const Tree& tree, int root) : tree(tree)
    {
        init(root);
    }

    int get_LCA(int node1, int node2)
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
        //   guarantee that the lower node will be the LCA as well, but we
        //   only care about finding the LCA, the other node doesn't matter.

        int lower_node = node1;
        int higher_node = node2;

        while (chain_root[lower_node] != chain_root[higher_node])
        {
            if (depths[chain_root[lower_node]] < depths[chain_root[higher_node]])
                std::swap(lower_node, higher_node);

            lower_node = parent_of[chain_root[lower_node]];
        }

        if (depths[higher_node] > depths[lower_node])
            std::swap(higher_node, lower_node);

        return higher_node;
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