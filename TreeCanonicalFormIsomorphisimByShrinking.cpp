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

const std::string opening_str = "(";
const std::string closing_str = ")";

std::string get_canonical_string(int weight)
{
    return opening_str + std::to_string(weight) + closing_str;
}

std::string get_canonical_string(std::multiset<std::string> set)
{
    std::string result = opening_str;
    for (auto& str : set) {
        result += str;
    }
    result += closing_str;
    return result;
}

std::pair<std::string, std::string> get_canonical_form_by_shrinking_tree(Tree tree)
{
    // NOTE: this function assumes that tree[0] is not used.
    // some optimizations can be done here to be faster.

    std::vector<std::multiset<std::string>> canonical_representation(tree.size());

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

            std::string last_edge_canonical = get_canonical_string(weight);
            canonical_representation[node_idx].insert(std::move(last_edge_canonical));

            auto node_canonical = get_canonical_string(canonical_representation[node_idx]);
            canonical_representation[parent_idx].insert(std::move(node_canonical));

            auto &parent = tree[parent_idx];
            parent.erase({node_idx, weight});
            if (parent.size() == 1) {
                leaves.push(parent_idx);
            }

            tree_size--;
        }
    }

    int center_node1_idx = leaves.front();
    std::string res1;
    std::string res2;

    if (tree_size == 1) {
        res1 = get_canonical_string(canonical_representation[center_node1_idx]);
    } else if (tree_size == 2) {

        auto edge = *tree[center_node1_idx].begin();
        int center_node2_idx = edge.to;

        auto edge_canonical = get_canonical_string(edge.weight);

        // insert temporarily
        canonical_representation[center_node1_idx].insert(edge_canonical);
        canonical_representation[center_node2_idx].insert(edge_canonical);

        res1 = get_canonical_string(canonical_representation[center_node1_idx]);
        res2 = get_canonical_string(canonical_representation[center_node2_idx]);

        // erase back
        canonical_representation[center_node1_idx].erase(edge_canonical);
        canonical_representation[center_node2_idx].erase(edge_canonical);

        canonical_representation[center_node1_idx].insert(res2); // insert old representation 2 into representation 1
        canonical_representation[center_node2_idx].insert(res1); // insert old representation 1 into representation 2

        res1 = get_canonical_string(canonical_representation[center_node1_idx]);
        res2 = get_canonical_string(canonical_representation[center_node2_idx]);
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

bool is_isomorphic(const std::pair<std::string, std::string> &a, const std::pair<std::string, std::string> &b)
{
    return a.first == b.first ||
            (
                (!a.second.empty() && !b.second.empty()) &&
                (a.second == b.first || a.first == b.second || a.second == b.second )
            );
}

void print(const std::vector<Tree> &trees)
{
    std::vector<std::pair<std::string, std::string>> canonical_representation(trees.size());

    for (int i = 0; i < trees.size(); i++) {
        canonical_representation[i] = get_canonical_form_by_shrinking_tree(trees[i]);
        std::cout << canonical_representation[i].first << " - " << canonical_representation[i].second << std::endl;
    }

    std::cout << std::endl;

    for (int i = 0; i < trees.size(); i++) {
        for (int j = i + 1; j < trees.size(); j++) {
            if (is_isomorphic(canonical_representation[i], canonical_representation[j])) {
                std::cout << "Trees " << i + 1 << " and " << j + 1 << " are isomorphic." << std::endl;
            }
        }
    }
}

int main()
{
    std::vector<Tree> trees = {
        get_sample_tree1(),
        get_sample_tree2(),
        get_sample_tree3(),
    };

    print(trees);
}
