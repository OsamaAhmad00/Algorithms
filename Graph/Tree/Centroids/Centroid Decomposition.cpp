#include <iostream>
#include <vector>
#include <queue>

typedef std::vector<std::vector<int>> Tree;

struct RootedTree
{
    int root;
    Tree tree;
};

void add_edge(Tree& tree, int parent, int child)
{
    tree[parent].push_back(child);
    tree[child].push_back(parent);
}

class CentroidDecomposition
{
    const Tree& tree;

    int root;
    Tree centroid_decomposition;
    std::vector<bool> is_blocked;
    std::vector<int> subtree_size;
    std::queue<int> current_centroids;

    void calc_subtree_size(int node, int parent = -1)
    {
        subtree_size[node] = 1;
        for (int child : tree[node]) {
            if (!is_blocked[child] && child != parent) {
                calc_subtree_size(child, node);
                subtree_size[node] += subtree_size[child];
            }
        }
    }

    int find_centroid(int node, int tree_size, int parent = -1)
    {
        for (int child : tree[node])
        {
            if (is_blocked[child] || child == parent)
                continue;

            if (subtree_size[child] > tree_size / 2)
                return find_centroid(child, tree_size, node);
        }

        return node;
    }

    void init_root_centroid()
    {
        std::fill(is_blocked.begin(), is_blocked.end(), false);

        const int initial_root = 0;
        calc_subtree_size(initial_root);
        root = find_centroid(initial_root, tree.size());
        current_centroids.push(root);
        is_blocked[root] = true;
    }

    void process_next_centroid()
    {
        int centroid = current_centroids.front();
        current_centroids.pop();

        for (int child : tree[centroid]) {
            if (!is_blocked[child])
            {
                // We don't need to precompute the sizes
                // of all subtrees beforehand, we can just
                // calculate the size for each subtree right
                // before starting to process it.
                calc_subtree_size(child);

                int child_centroid = find_centroid(child, subtree_size[child]);
                add_edge(centroid_decomposition, centroid, child_centroid);
                current_centroids.push(child_centroid);
                is_blocked[child_centroid] = true;
            }
        }
    }

    void construct_centroid_decomposition()
    {
        init_root_centroid();

        // We need to use a queue when processing the centroids
        // to ensure that higher level centroids (with smaller
        // depths) get processed before the lower level centroids.
        while (!current_centroids.empty())
            process_next_centroid();
    }

public:

    // Properties of a Centroid Decomposition Tree:
    // 1 - Its height is O(log(n)), which means that the
    //      height of its longest branch is O(log(n)).
    //      This is because each time you set the root of
    //      the subtree to be the centroid, you make the size
    //      of children <= subtree_size / 2, and you can do that
    //      at most O(log(n)) times.
    // 2 - Each subtree in the centroid decomposition tree forms a connected
    //      component in the original tree. This is because of our process of
    //      construction: for every node, when it's added to the centroid
    //      decomposition tree, it's part of some connected subtree that hasn't
    //      been removed yet. Once we remove that node, every node in that subtree
    //      will be added as a child of that node, and thus all of its children
    //      in the centroid decomposition tree will be part of that connected subtree.
    // 3 - For any node x:
    //       let A = ancestors of x in the centroid decomposition tree.
    //       let T = The connected component that represents the subtree of
    //        x in the centroid decomposition tree.
    //      Every adjacent node to T (but not part of T) will be in the set A, in other
    //      words, Every adjacent node to T will be an ancestor of x in the centroid
    //      decomposition tree. This doesn't go the other way, not every node in A will
    //      is adjacent to T.
    //      This is a corollary of property number 2.
    //      Consider a node y that is adjacent to T, but not part of T, since y is adjacent
    //      to T, this means that it should be part of T. The only way for it to be connected
    //      to T, yet not a part of T is by being a higher level centroid (an ancestor of the
    //      nodes in T)
    //      Another way to think about it:
    //      Since a subtree in the centroid decomposition tree forms a connected component in
    //      the original tree, node y, that is adjacent to T can be one of two things:
    //       - Part of T, in which case, it'll be a part of the subtree represented by T in
    //         the centroid decomposition tree.
    //       - Not a part of T (which is the case), in which case, it'll not be a child of the
    //         subtree represented by T, but it should still be connected to the subtree, and
    //         the only way that this is possible is to be an ancestor of T.

    CentroidDecomposition(const Tree& tree) :
        tree(tree),
        subtree_size(tree.size()),
        centroid_decomposition(tree.size()),
        is_blocked(tree.size())
    {}

    RootedTree get_centroid_decomposition()
    {
        construct_centroid_decomposition();
        return {root, std::move(centroid_decomposition)};
    }
};

Tree get_sample_tree_1()
{
    // Centroid = 0

    Tree tree(7);

    add_edge(tree, 0, 1);
    add_edge(tree, 0, 2);

    add_edge(tree, 1, 5);
    add_edge(tree, 1, 6);

    add_edge(tree, 2, 3);
    add_edge(tree, 2, 4);

    return tree;
}

Tree get_sample_tree_2()
{
    // Centroid = 5

    Tree tree(16);

    add_edge(tree, 3, 0);
    add_edge(tree, 3, 1);
    add_edge(tree, 3, 2);

    add_edge(tree, 4, 3);

    add_edge(tree, 5, 4);
    add_edge(tree, 5, 6);
    add_edge(tree, 5, 9);

    add_edge(tree, 6, 7);
    add_edge(tree, 6, 8);

    add_edge(tree, 9, 10);

    add_edge(tree, 10, 11);
    add_edge(tree, 10, 12);

    add_edge(tree, 11, 13);

    add_edge(tree, 12, 14);
    add_edge(tree, 12, 15);

    return tree;
}

void print_tree(const Tree& tree, int node, int parent = -1, int indentation = 0)
{
    for (int i = 0; i < indentation; i++)
        std::cout << ' ';
    std::cout << node << std::endl;
    for (int child : tree[node])
        if (child != parent)
            print_tree(tree, child, node, indentation + 2);
}

int max_depth(const Tree& tree, int node, int parent = -1)
{
    int max = 0;
    for (int child : tree[node])
        if (child != parent)
            max = std::max(max, max_depth(tree, child, node));
    return 1 + max;
}

void test(const Tree& tree)
{
    auto cd = CentroidDecomposition(tree).get_centroid_decomposition();

    print_tree(cd.tree, cd.root);
    std::cout << "Max Depth for the original tree rooted at " << cd.root << " = ";
    std::cout << max_depth(tree, cd.root) << std::endl;
    std::cout << "Max Depth for the centroid decomposition rooted at " << cd.root << " = ";
    std::cout << max_depth(cd.tree, cd.root) << std::endl;
    std::cout << std::endl;
}

int main()
{
    test(get_sample_tree_1());
    test(get_sample_tree_2());
}