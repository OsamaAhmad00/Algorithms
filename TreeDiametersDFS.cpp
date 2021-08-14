#include <map>
#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>

struct Edge
{
    int to;
    int weight;
};

// TODO you might extract a class out of these functions.

typedef std::vector<std::vector<Edge>> Tree;

struct LongestPathInfo
{
    int longest_branch_length;

    // not passing through the parent
    int diameter_length;
    int passes_through;

    LongestPathInfo() {}

    LongestPathInfo(int longest_branch_length) :
            longest_branch_length(longest_branch_length) {}

    LongestPathInfo(int longest_branch_length, int diameter_length, int passes_through) :
            longest_branch_length(longest_branch_length),
            diameter_length(diameter_length),
            passes_through(passes_through)
            {}

};

bool compare_longest_branches(const LongestPathInfo &a, const LongestPathInfo &b) {
    return a.longest_branch_length > b.longest_branch_length;
}

LongestPathInfo get_max_diameter(const LongestPathInfo &a, const LongestPathInfo &b) {
    if (a.diameter_length > b.diameter_length) {
        return a;
    }
    return b;
};

LongestPathInfo get_tree_diameter_info_dfs(const Tree &tree, int starting_node = 1)
{
    std::function<LongestPathInfo(int, int)> get_diameter_info = [&](int node, int parent) {

        LongestPathInfo result = {0, 0, node};
        std::vector<LongestPathInfo> longest_branches = {0, 0, 0};

        for (auto child : tree[node]) {

            if (child.to == parent) {
                continue;
            }

            auto info = get_diameter_info(child.to, node);
            info.longest_branch_length += child.weight;

            result = get_max_diameter(result, info);

            longest_branches[2] = info;
            std::sort(longest_branches.begin(), longest_branches.end(), compare_longest_branches);
        }

        LongestPathInfo longest_two_branches_union;
        longest_two_branches_union.diameter_length =
                longest_branches[0].longest_branch_length + longest_branches[1].longest_branch_length;
        longest_two_branches_union.passes_through = node;

        result = get_max_diameter(result, longest_two_branches_union);

        result.longest_branch_length = longest_branches[0].longest_branch_length;

        return result;
    };

    return get_diameter_info(starting_node, -1);
}

// set max_count to -1 to get all diameters.
std::vector<std::vector<int>> generate_diameters(const Tree &tree, const LongestPathInfo &info, int max_count = -1)
{
    int starting_node = info.passes_through;

    // leaves[i] are the leaf nodes with a distance of i from the starting node.
    // the length of a path can't exceed the diameter's length (longest path) length.
    std::map<int, std::vector<int>> leaves;
    std::vector<int> parent_of(tree.size());

    std::function<void(int, int, int)> set_leaves_and_parents = [&](int node, int parent, int length) {

        parent_of[node] = parent;

        if (tree[node].empty() ||
                (tree[node].size() == 1 && tree[node][0].to == parent)) {

            // you might consider deleting the closer nodes that should
            // be deleted in this block to reduce the memory usage.

            leaves[length].push_back(node);
            return;
        }

        for (auto child : tree[node]) {

            if (child.to == parent) continue;

            length += child.weight;
            set_leaves_and_parents(child.to, node, length);
            length -= child.weight;
        }
    };

    auto combine_two_paths = [&](std::vector<int> &&path1, const std::vector<int> &path2) {
        path1.insert(path1.end(), path2.begin(), path2.end());
        return path1;
    };

    auto generate_path_to_root = [&](int child) {
        std::vector<int> result;
        int current_node = child;
        while (current_node != -1) {
            result.push_back(current_node);
            current_node = parent_of[current_node];
        }
        return result;
    };

    auto generate_paths = [&](const std::vector<int> &from, const std::vector<int> &to) {

        // assumes that from equals to, or that there
        // are no common nodes between from and to.

        std::vector<std::vector<int>> result;

        // this can be improved
        for (int i = 0; i < from.size(); i++) {

            int leaf2 = from[i];
            auto path2 = generate_path_to_root(leaf2);

            path2.pop_back(); // deleting the starting node since it's already in path1.
            std::reverse(path2.begin(), path2.end());

            for (int j = to.size() - 1; j >= 0; j--) {

                int leaf1 = to[j];

                if (leaf1 == leaf2) {
                    // from equals to. should stop
                    // here to avoid repetition.
                    break;
                }

                if (max_count-- == 0) {
                    return result;
                }

                auto path1 = generate_path_to_root(leaf1);
                result.push_back(combine_two_paths(std::move(path1), path2));
            }
        }

        return result;
    };

    set_leaves_and_parents(starting_node, -1, 0);

    auto it = leaves.end(); it--;
    auto &furthest_leaves = it->second;

    if (furthest_leaves.size() > 1) {
        return generate_paths(furthest_leaves, furthest_leaves);
    }

    it--;
    return generate_paths(furthest_leaves, it->second);
}

void add_child(Tree &tree, int parent, int child, int weight)
{
    tree[parent].push_back({child, weight});
    tree[child].push_back({parent, weight});
}

Tree get_sample_tree()
{
    Tree tree(8);

    add_child(tree, 1, 2, 1);
    add_child(tree, 1, 3, 1);

    add_child(tree, 2, 6, 1);
    add_child(tree, 2, 7, 1);

    add_child(tree, 3, 4, 1);
    add_child(tree, 3, 5, 1);

    return tree;
}

void print_longest_path_info(const LongestPathInfo &info)
{
    std::cout << "Longest path length: " << info.diameter_length
        << ", Longest branch length: " << info.longest_branch_length
        << ", and passes through " << info.passes_through << std::endl;
}

void print_all_diameters(const Tree &tree, const LongestPathInfo &info)
{
    auto paths = generate_diameters(tree, info, -1);

    for (auto &path : paths) {
        for (auto node : path) {
            std::cout << " -> " << node;
        }
        std::cout << std::endl;
    }
}

int main()
{
    auto tree = get_sample_tree();
    auto info = get_tree_diameter_info_dfs(tree);
    print_longest_path_info(info);

    print_all_diameters(tree, info);
}
