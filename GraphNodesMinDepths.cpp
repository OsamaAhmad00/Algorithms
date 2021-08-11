#include <queue>
#include <vector>
#include <iostream>

typedef std::vector<std::vector<int>> Graph;

std::vector<int> calc_nodes_min_depths(const Graph &graph, int starting_node = 1)
{
    // BFS
    // the result depends on the starting_node.

    // -1 means not visited yet.
    std::vector<int> result(graph.size(), -1);

    std::queue<int> queue;
    queue.push(starting_node);

    int depth = 0;

    while (!queue.empty())
    {
        // the count of the nodes in the current level (depth).
        // this piece of information might be useful.
        int level_size = queue.size();

        while (level_size--)
        {
            int node = queue.front();
            queue.pop();

            result[node] = depth;

            for (int child : graph[node]) {
                if (result[child] == -1) {
                    queue.push(child);
                }
            }
        }

        depth++;
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

void print_nodes_depth(const std::vector<int> &depths)
{
    for (int i = 1; i < depths.size(); i++) {
        std::cout << "Node " << i << " has a minimum depth of " << depths[i] << std::endl;
    }
}

int main()
{
    auto graph = get_sample_graph();
    auto depths = calc_nodes_min_depths(graph);
    print_nodes_depth(depths);
}
