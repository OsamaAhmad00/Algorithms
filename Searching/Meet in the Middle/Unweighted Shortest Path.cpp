#include <iostream>
#include <vector>
#include <queue>

const int NOT_FOUND = -1;

typedef std::vector<std::vector<int>> Graph;

int shortest_path(const Graph& graph, int source, int target)
{
    // The idea here is, instead of going from the source
    //  until we find the target, we go from both the source
    //  and the target, and meet in the middle.

    struct QueueNode { int node; int weight; };

    int current_side = 0;
    std::vector<std::queue<QueueNode>> queues(2);
    std::vector<std::vector<int>> shortest_paths(2, std::vector<int>(graph.size(), NOT_FOUND));

    queues[0].push({source, 0});
    queues[1].push({target, 0});

    while (!queues[0].empty() && !queues[1].empty())
    {
        auto& queue = queues[current_side];

        auto front = queue.front();
        queue.pop();

        if (shortest_paths[!current_side][front.node] != NOT_FOUND) {
            return front.weight + shortest_paths[!current_side][front.node];
        }

        for (int child : graph[front.node]) {
            auto& weight = shortest_paths[current_side][child];
            if (weight == NOT_FOUND) {
                weight = front.weight + 1;
                queue.push({child, weight});
            }
        }

        current_side = !current_side;
    }

    return NOT_FOUND;
}

void add_child(Graph &graph, int parent, int child)
{
    graph[parent].push_back(child);
    graph[child].push_back(parent);
}

Graph get_sample_graph()
{
    Graph graph(6);

    add_child(graph, 1, 2);
    add_child(graph, 2, 3);
    add_child(graph, 3, 5);
    add_child(graph, 4, 1);
    add_child(graph, 5, 4);

    return graph;
}

void test(const Graph& graph)
{
    for (int i = 0; i < graph.size(); i++) {
        for (int j = i + 1; j < graph.size(); j++)
        {
            int edges = shortest_path(graph, i, j);

            if (edges == NOT_FOUND) {
                std::cout << "There is no path from " << i;
                std::cout << " to " << j << "." << std::endl;
            } else {
                std::cout << "The shortest path from " << i;
                std::cout << " to " << j << " consists of ";
                std::cout << edges << " edges." << std::endl;
            }
        }
    }
}

int main()
{
    test(get_sample_graph());
}