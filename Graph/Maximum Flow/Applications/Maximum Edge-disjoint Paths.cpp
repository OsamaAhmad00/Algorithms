#include <iostream>
#include <vector>

struct Edge
{
    int from;
    int to;
    int weight;
};

class Graph : public std::vector<std::vector<int>>
{
    std::vector<std::vector<int>> weights;

public:

    explicit Graph(int n) : std::vector<std::vector<int>>(n),
        weights(n, std::vector<int>(n, 0)) {}

    void add_weight(int from, int to, int weight) {
        if (weights[from][to] == 0) {
            (*this)[from].push_back(to);
        }
        weights[from][to] += weight;
    }

    int get_weight(int from, int to) const { return weights[from][to]; }
};

class MaxFlowCalculator
{
    int sink;
    const Graph flow_graph;
    Graph residual_graph;
    std::vector<bool> visited;

    int get_flow_value(int from, int to) const
    {
        int a = flow_graph.get_weight(to, from);
        int b = residual_graph.get_weight(to, from);
        return b - a;
    }

    int add_augmenting_path(int from, int bottleneck = INT_MAX)
    {
        if (from == sink) {
            return bottleneck;
        }

        int result = 0;
        visited[from] = true;

        for (int to : residual_graph[from])
        {
            if (visited[to]) continue;
            int weight = residual_graph.get_weight(from, to);
            int value = add_augmenting_path(to, std::min(weight, bottleneck));
            if (value > 0) {
                residual_graph.add_weight(from, to, -value);
                residual_graph.add_weight(to, from,  value);
                result = value;
                break;
            }
        }

        visited[from] = false;
        return result;
    };

    std::vector<Edge> get_flow_edges()
    {
        std::vector<Edge> result;

        for (int from = 0; from < flow_graph.size(); from++) {
            for (int to : flow_graph[from]) {
                int value = get_flow_value(from, to);
                if (value > 0) {
                    result.push_back({from, to, value});
                }
            }
        }

        return result;
    }

    void compute_residual_graph(int source, int sink)
    {
        residual_graph = flow_graph;
        this->sink = sink;
        while (add_augmenting_path(source) != 0);
    }

public:

    MaxFlowCalculator(const Graph& graph) : flow_graph(graph),
        visited(graph.size(), false), residual_graph({}) {}

    std::vector<Edge> get_max_flow(int source, int sink)
    {
        // Ford Fulkerson
        compute_residual_graph(source, sink);
        return get_flow_edges();
    }
};

std::vector<std::vector<int>> get_max_edge_disjoint_paths(const Graph& graph, int source, int sink)
{
    // Edge-disjoint paths are paths with no edges in common.

    // The idea here is to set the weight of each edge to 1.
    // this way, each edge is going to be used in at most 1 path.
    // If we want to allow some edge to be used for n times
    // (instead of just 1), we just set the weight of this edge to n.

    Graph copy = graph;
    for (int from = 0; from < copy.size(); from++) {
        for (int to : copy[from]) {
            int weight = copy.get_weight(from, to);
            if (weight > 0) {
                copy.add_weight(from, to, 1 - weight);
            }
        }
    }

    auto edges = MaxFlowCalculator(copy).get_max_flow(source, sink);

    std::vector<std::vector<int>> result;

    // since each edge is used at most in a single path, each
    // node (except for the source) will have at most 1 child.
    std::vector<int> child(copy.size(), -1);
    for (auto& edge : edges) {
        child[edge.from] = edge.to;
        if (edge.from == source) {
            // since the source has multiple children,
            // we need to list all children here before
            // proceeding to compute the paths.
            result.push_back({edge.from, edge.to});
        }
    }

    for (auto& path : result) {
        int current = path.back();
        while (current != sink) {
            current = child[current];
            path.push_back(current);
        }
    }

    return result;
}

Graph get_sample_graph_1()
{
    Graph graph(6);

    graph.add_weight(0, 1, 16);
    graph.add_weight(0, 2, 13);
    graph.add_weight(1, 2, 10);
    graph.add_weight(2, 1, 4);
    graph.add_weight(1, 3, 12);
    graph.add_weight(3, 2, 9);
    graph.add_weight(2, 4, 14);
    graph.add_weight(4, 3, 7);
    graph.add_weight(4, 5, 4);
    graph.add_weight(3, 5, 20);

    return graph;
}

Graph get_sample_graph_2()
{
    Graph graph(11);

    graph.add_weight(0, 1, 7);
    graph.add_weight(0, 2, 2);
    graph.add_weight(0, 3, 1);
    graph.add_weight(1, 4, 2);
    graph.add_weight(1, 5, 4);
    graph.add_weight(2, 5, 5);
    graph.add_weight(2, 6, 6);
    graph.add_weight(3, 4, 4);
    graph.add_weight(3, 8, 8);
    graph.add_weight(4, 7, 7);
    graph.add_weight(4, 8, 1);
    graph.add_weight(5, 7, 3);
    graph.add_weight(5, 9, 3);
    graph.add_weight(5, 6, 8);
    graph.add_weight(6, 9, 3);
    graph.add_weight(7, 10, 1);
    graph.add_weight(8, 10, 3);
    graph.add_weight(9, 10, 4);

    return graph;
}

void test(const Graph& graph)
{
    // assumes that the source is 0 and the
    // sink is the node with the largest number.

    int source = 0;
    int sink = graph.size() - 1;
    auto paths = get_max_edge_disjoint_paths(graph, source, sink);
    for (auto &path : paths) {
        for (int node : path) {
            std::cout << node << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test(get_sample_graph_1());
    test(get_sample_graph_2());
}