#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>

// Note: you can perform "Edge splitting" if the edge weights are
// relatively small. Also, you can perform "Vertex splitting" if
// only some vertices have costs on them (also relatively small).

typedef std::vector<std::vector<int>> Tree;

struct QueueFrame
{
    int node;
    int parent;
};

std::vector<int> get_tree_diameters_bfs(const Tree &tree, int starting_node = 1)
{
    // Algorithm: get one furthest node from a random node.
    // This node is one end of a diameter. Now, get the furthest
    // node from it. This is the other end of the diameter.

    std::vector<int> parent_of(tree.size());

    auto get_furthest_node = [&](const Tree &tree, int node) {

        std::queue<QueueFrame> queue;
        queue.push({node, -1});

        int furthest_node = -1;

        while (!queue.empty())
        {
            furthest_node = queue.front().node;

            int level_size = queue.size();
            for (int i = 0; i < level_size; i++) {

                auto frame = queue.front();
                queue.pop();

                parent_of[frame.node] = frame.parent;

                for (auto child: tree[frame.node]) {
                    if (child != frame.parent) {
                        queue.push({child, frame.node});
                    }
                }
            }
        }

        return furthest_node;
    };

    auto get_path_to_root = [&](int child) {
        std::vector<int> result;
        int current_node = child;
        while (current_node != -1) {
            result.push_back(current_node);
            current_node = parent_of[current_node];
        }
        return result;
    };

    auto get_path = [&](int from, int to) {
        std::vector<int> from_to_root = get_path_to_root(from);
        std::vector<int> to_to_root = get_path_to_root(to);

        from_to_root.pop_back();

        from_to_root.insert(from_to_root.end(), to_to_root.rbegin(), to_to_root.rend());

        return from_to_root;
    };

    int furthest_node = get_furthest_node(tree, starting_node);

    // all diameters have the same length. the other_furthest_nodes
    // will all have a same distance from any node in the furthest_nodes.
    // thus, only one node is sufficient to get all of the wanted nodes.
    int other_furthest_node = get_furthest_node(tree, furthest_node);

    return get_path(furthest_node, other_furthest_node);
}

void add_child(Tree &tree, int parent, int child)
{
    tree[parent].push_back(child);
    tree[child].push_back(parent);
}

Tree get_sample_tree()
{
    Tree tree(8);

    add_child(tree, 1, 2);
    add_child(tree, 1, 3);

    add_child(tree, 2, 6);
    add_child(tree, 2, 7);

    add_child(tree, 3, 4);
    add_child(tree, 3, 5);

    return tree;
}

void print_all_diameters(const std::vector<int> &path)
{
    for (auto node : path) {
        std::cout << " -> " << node;
    }
    std::cout << std::endl;
}

int main()
{
    auto tree = get_sample_tree();
    auto diameters = get_tree_diameters_bfs(tree);
    print_all_diameters(diameters);
}
