#include <iostream>
#include <vector>

typedef std::vector<std::vector<int>> Tree;

class CentroidFinder
{
    int root;
    const Tree& tree;
    std::vector<int> subtree_size;

    void calc_subtree_size(int node, int parent = -1)
    {
        subtree_size[node] = 1;
        for (int child : tree[node]) {
            if (child != parent) {
                calc_subtree_size(child, node);
                subtree_size[node] += subtree_size[child];
            }
        }
    }

    int find_centroid(int node, int parent = -1)
    {
        // A node is a centroid if the size of all its
        //  children has a size <= floor(tree.size() / 2).
        // For a given node, if we can find a child with
        //  subtree_size > tree.size() / 2, then this node
        //  is not a centroid.
        // If we have a child that has a subtree_size > tree.size() / 2,
        //  then the centroid can only be in its subtree. We don't
        //  need to search for the centroid in the subtrees of all
        //  children, just the one with the biggest subtree size.
        // To make sure that this node is a centroid, we should also make
        //  sure that tree.size() - subtree_size[node] <= tree.size() / 2.
        //  This is because when finding the centroid, the parent should
        //  also be considered a child and be taken into consideration.
        //  But we don't need to check for that because it'll always be
        //  true. The reason is that since the current node is entered,
        //  this means that it has a subtree_size > tree.size() / 2, which
        //  means that the number of all nodes outside this subtree will
        //  be <= tree.size() / 2.

        for (int child : tree[node])
        {
            // The subtree_size of the parent will always be
            // bigger than tree.size() / 2. This is because
            // this node has subtree_size bigger than
            // tree.size() / 2, and subtree_size[parent]
            // is bigger than subtree_size[node].
            if (child == parent)
                continue;

            // There can exist only one child with size bigger
            //  than tree.size() / 2. In case that child is
            //  found, the centroid has to be in its subtree.
            if (subtree_size[child] > tree.size() / 2)
                return find_centroid(child, node);
        }

        // If no child with subtree_size bigger than
        // tree_size / 2, then this node is a centroid.
        return node;
    }

public:

    CentroidFinder(const Tree& tree, int root = 0)
        : tree(tree), root(root), subtree_size(tree.size()) {}

    int find_centroid() {
        calc_subtree_size(root, -1);
        return find_centroid(root);
    }
};

void add_edge(Tree& tree, int parent, int child)
{
    tree[parent].push_back(child);
    tree[child].push_back(parent);
}

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

void test(const Tree& tree)
{
    int centroid = CentroidFinder(tree).find_centroid();
    for (int node = 0; node < tree.size(); node++)
        if (CentroidFinder(tree, node).find_centroid() != centroid)
            std::cout << "Fail..." << std::endl;

    std::cout << "Centroid is node " << centroid << "." << std::endl;
}

int main()
{
    test(get_sample_tree_1());
    test(get_sample_tree_2());
}