#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>

typedef std::vector<std::vector<int>> Graph;

std::vector<std::vector<int>> calc_shortest_paths (
    const Graph &graph,
    const std::vector<int> &starting_nodes,
    const std::vector<int> &ending_nodes
) {
    // BFS
    // this only works with unweighted graphs.

    std::vector<std::vector<int>> result;
    std::vector<int> parent_of(graph.size(), -1);

    std::vector<bool> is_end_node(graph.size(), false);
    for (auto node : ending_nodes) {
        is_end_node[node] = true;
    }

    std::queue<int> queue;
    for (auto node : starting_nodes) {
        // -1 means not visited. -2 means is a starting node.
        // shouldn't set a parent for a starting node..
        parent_of[node] = -2;
        queue.push(node);
    }

    while (!queue.empty())
    {
        int level_size = queue.size();

        while (level_size--)
        {
            int node = queue.front();
            queue.pop();

            for (int child : graph[node]) {

                if (parent_of[child] == -1) {
                    // not visited
                    parent_of[child] = node;
                    queue.push(child);
                }

                // adding a path only through its parent
                // to avoid having duplicate equal paths
                if (is_end_node[child] && parent_of[child] == node) {
                    std::vector<int> path;
                    int current_node = child;
                    while (current_node >= 0) {
                        path.push_back(current_node);
                        current_node = parent_of[current_node];
                    }
                    std::reverse(path.begin(), path.end());
                    result.push_back(path);
                }
            }
        }
    }

    return result;
}

Graph get_sample_graph()
{
    Graph graph(9);

    graph[1].push_back(2);
    graph[1].push_back(5);
    graph[1].push_back(8);

    graph[2].push_back(3);

    graph[3].push_back(4);

    graph[4].push_back(2);

    graph[5].push_back(6);

    graph[6].push_back(3);
    graph[6].push_back(7);
    graph[6].push_back(8);

    return graph;
}

void print_shortest_paths(const std::vector<std::vector<int>> &paths)
{
    for (auto &path : paths) {

        std::cout << "Shortest path from " << path.front() << " to " << path.back() << ": ";

        for (auto node : path) {
            std::cout << node << ' ';
        }
        std::cout << std::endl;
    }
}

int main()
{
    auto graph = get_sample_graph();
    auto paths = calc_shortest_paths(graph, {1, 2, 5}, {7, 4, 3});
    print_shortest_paths(paths);
}
