#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <cmath>

const int MAX_VALUE = 1'000'000;

struct Coord
{
    int x = 0;
    int y = 0;
};

struct NodeInfo
{
    Coord coord;
    // you can add more here.
};

struct Edge
{
    int to;
    int weight;
};

typedef std::vector<std::vector<Edge>> Graph;

struct ShortestPathsInfo
{
    std::vector<int> shortest_distances;
    std::vector<int> parent_of;

    ShortestPathsInfo(int n) {
        shortest_distances.resize(n, MAX_VALUE);
        parent_of.resize(n, -1);
    }
};

struct QueueFrame
{
    int node;
    int parent;
    int weight;
    int additional_weight;

    int total_weight() const {
        return weight + additional_weight;
    }

    bool operator<(const QueueFrame &other) const {
        return total_weight() < other.total_weight();
    }

    bool operator>(const QueueFrame &other) const {
        return total_weight() > other.total_weight();
    }
};

int euclidean_distance(const Coord &a, const Coord &b)
{
    int x_diff = a.x - b.x;
    int y_diff = a.y - b.y;

    return ceil(sqrt(x_diff * x_diff + y_diff * y_diff));
}

int get_additional_heuristics(const std::vector<NodeInfo> &info, int from, int to)
{
    return euclidean_distance(info[from].coord, info[to].coord);
}

ShortestPathsInfo A_Star(const Graph &graph, const std::vector<NodeInfo> &info, int source, int target)
{
    ShortestPathsInfo result(graph.size());
    auto &shortest_distances = result.shortest_distances;
    auto &parent_of = result.parent_of;

    std::priority_queue<QueueFrame, std::vector<QueueFrame>, std::greater<QueueFrame>> queue;

    queue.push({.node=source, .parent=-1, .weight=0, .additional_weight=0});

    for (int i = 0; i < graph.size() - 1 && !queue.empty();)
    {
        auto closest = queue.top();
        queue.pop();

        int node = closest.node;

        if (closest.weight >= shortest_distances[node]) {
            continue;
        }

        shortest_distances[node] = closest.weight;
        parent_of[node] = closest.parent;
        i++;

        for (auto& edge : graph[node]) {

            int additional_heuristics = get_additional_heuristics(info, node, edge.to);

            if (closest.weight + edge.weight < shortest_distances[edge.to]) {
                queue.push({
                    .node = edge.to,
                    .parent = node,
                    .weight = closest.weight + edge.weight,
                    .additional_weight = additional_heuristics
                });
            }
        }
    }

    return result;
}

void add_child(Graph &graph, int parent, int child, int weight)
{
    graph[parent].push_back({child, weight});
    graph[child].push_back({parent, weight});
}

std::pair<Graph, std::vector<NodeInfo>> get_sample_graph()
{
    const int n = 6;

    Graph graph(n);
    add_child(graph, 1, 2, 8);
    add_child(graph, 2, 3, 100);
    add_child(graph, 3, 5, 3);
    add_child(graph, 4, 1, 5);
    add_child(graph, 5, 4, 2);

    std::vector<NodeInfo> info(n);
    info[1].coord = {0, 4};
    info[2].coord = {4, 4};
    info[3].coord = {4, 0};
    info[4].coord = {0, 0};
    info[5].coord = {2, 2};

    return {graph, info};
}

void print_shortest_path_info(const Graph& graph, const std::vector<NodeInfo> &graph_info, int source, int target)
{
    auto info = A_Star(graph, graph_info, source, target);

    struct StackFrame
    {
        int node;
        int weight;
    };

    std::stack<StackFrame> stack;

    int current = target;
    while (current != source) {
        int parent = info.parent_of[current];
        int weight = info.shortest_distances[current] -
                     info.shortest_distances[parent];
        stack.push({.node=current, .weight=weight});
        current = parent;
    }

    std::cout << "The total distance from " << source << " to " << target
            << " is " << info.shortest_distances[target] << "." << std::endl;

    std::cout << "Path: (" << source << ")";
    while (!stack.empty()) {
        auto frame = stack.top(); stack.pop();
        std::cout << " --" << frame.weight << "--> (" << frame.node << ")";
    }
    std::cout << std::endl;
}

int main()
{
    auto result = get_sample_graph();
    print_shortest_path_info(result.first, result.second, 5, 2);
}