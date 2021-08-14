#include <map>
#include <set>
#include <queue>
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>

struct Edge
{
    int to;
    int weight;

    bool operator<(const Edge &other) const
    {
        return to < other.to;
    }
};

typedef std::vector<std::set<Edge>> Tree;

std::pair<int, int> get_center_nodes_by_shrinking_tree(Tree tree)
{
    // NOTE: this function assumes that tree[0] is not used.
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

    // you shouldn't manipulate tree.size() directly
    //  (erase nodes randomly) since this will affect
    //  the indices of the nodes in the tree.
    // the 0-th index is not used here.
    int tree_size = tree.size() - 1;

    // 1 or 2 nodes left as leafs means that there
    // are 1 or 2 center nodes respectively.
    while (tree_size > 2)
    {
        int leaves_count = leaves.size();
        while (leaves_count--)
        {
            auto node_idx = leaves.front();
            leaves.pop();

            auto &node = tree[node_idx];
            auto &only_edge = *node.begin();
            auto parent_idx = only_edge.to;
            auto weight = only_edge.weight;

            auto &parent = tree[parent_idx];
            parent.erase({node_idx, weight});
            if (parent.size() == 1) {
                leaves.push(parent_idx);
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

void add_child(Tree &tree, int parent, int child, int weight)
{
    tree[parent].insert({child, weight});
    tree[child].insert({parent, weight});
}

// Tree 1 and 2 are isomorphic.

Tree get_sample_tree1()
{
    Tree tree(8);

    add_child(tree, 1, 2, 1);
    add_child(tree, 1, 3, 2);

    add_child(tree, 2, 6, 3);
    add_child(tree, 2, 7, 4);

    add_child(tree, 3, 4, 5);
    add_child(tree, 3, 5, 6);

    return tree;
}

Tree get_sample_tree2()
{
    Tree tree(8);

    add_child(tree, 7, 6, 1);
    add_child(tree, 7, 5, 2);

    add_child(tree, 6, 2, 3);
    add_child(tree, 6, 1, 4);

    add_child(tree, 5, 4, 5);
    add_child(tree, 5, 3, 6);

    return tree;
}

Tree get_sample_tree3()
{
    Tree tree(9);

    add_child(tree, 1, 2, 1);
    add_child(tree, 1, 3, 2);

    add_child(tree, 2, 6, 3);
    add_child(tree, 2, 7, 4);

    add_child(tree, 3, 4, 5);
    add_child(tree, 3, 5, 6);

    add_child(tree, 4, 8, 7);

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
