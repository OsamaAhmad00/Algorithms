#include <queue>
#include <vector>
#include <iostream>

typedef std::vector<std::vector<int>> Tree;

std::pair<int, int> get_center_nodes_by_shrinking_tree(const Tree& tree)
{
    // some optimizations can be done here to be faster.

    // using a queue to ensure that old leaves
    // gets processed before the new ones.
    std::queue<int> leaves;

    for (int i = 0; i < tree.size(); i++) {
        int degree = tree[i].size();
        if (degree == 1) {
            leaves.push(i);
        }
    }

    // since we don't delete nodes from the tree,
    // the size of the tree will not change. we'll
    // make a copy of the size, and each time we
    // process a node, we'll decrement the size by 1.
    int tree_size = tree.size();

    // instead of actually deleting edges going to
    // children, we can just keep track of how many
    // children are left for each node.
    std::vector<int> child_count(tree.size());
    for (int i = 0; i < tree.size(); i++)
        child_count[i] = tree[i].size();

    // if 2 nodes remain, then
    // the tree has 2 centers.
    while (tree_size > 2)
    {
        int leaves_count = leaves.size();
        while (leaves_count--)
        {
            int node = leaves.front();
            leaves.pop();

            // since the current node is a leaf, it'll
            // have a single edge, going to its parent.
            int parent = tree[node].front();

            int& children = child_count[parent];
            if ((--children) == 1) {
                leaves.push(parent);
            }

            tree_size--;
        }
    }

    int res1 = leaves.front(); leaves.pop();
    int res2 = -1;

    if (tree_size == 2) {
        res2 = leaves.front();
    }

    return {res1, res2};
}

void add_child(Tree &tree, int parent, int child)
{
    tree[parent].push_back(child);
    tree[child].push_back(parent);
}

// Tree 1 and 2 are isomorphic.

Tree get_sample_tree1()
{
    Tree tree(7);

    add_child(tree, 0, 1);
    add_child(tree, 0, 2);

    add_child(tree, 1, 5);
    add_child(tree, 1, 6);

    add_child(tree, 2, 3);
    add_child(tree, 2, 4);

    return tree;
}

Tree get_sample_tree2()
{
    Tree tree(7);

    add_child(tree, 6, 5);
    add_child(tree, 6, 4);

    add_child(tree, 5, 1);
    add_child(tree, 5, 0);

    add_child(tree, 4, 3);
    add_child(tree, 4, 2);

    return tree;
}

Tree get_sample_tree3()
{
    Tree tree(8);

    add_child(tree, 0, 1);
    add_child(tree, 0, 2);

    add_child(tree, 1, 5);
    add_child(tree, 1, 6);

    add_child(tree, 2, 3);
    add_child(tree, 2, 4);

    add_child(tree, 3, 7);

    return tree;
}

void print_centers(const std::vector<Tree> &trees)
{
    for (auto &tree : trees) {
        auto centers = get_center_nodes_by_shrinking_tree(tree);
        std::cout << centers.first << ", " << centers.second << std::endl;
    }
}

int main()
{
    std::vector<Tree> trees = {
        get_sample_tree1(),
        get_sample_tree2(),
        get_sample_tree3(),
    };

    print_centers(trees);
}
