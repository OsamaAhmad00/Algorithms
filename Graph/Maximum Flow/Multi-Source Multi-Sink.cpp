#include <iostream>
#include <vector>
#include <algorithm>

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

    void resize(int size)
    {
        std::vector<std::vector<int>>::resize(size);
        weights.resize(size);

        for (auto& x : *this)
            x.resize(size, 0);

        for (auto& x : weights)
            x.resize(size, 0);
    }
};

class MaxFlowCalculator
{
    int source, sink;
    int weights_sum;
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
        if (bottleneck == 0 || from == sink) {
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

        return result;
    };

    std::vector<Edge> get_flow_edges()
    {
        std::vector<Edge> result;

        for (int from = 0; from < flow_graph.size(); from++) {
            if (from == source) continue;
            for (int to : flow_graph[from]) {
                if (to == sink) continue;
                int value = get_flow_value(from, to);
                if (value > 0) {
                    result.push_back({from, to, value});
                }
            }
        }

        return result;
    }

    void compute_residual_graph(const std::vector<int>& sources, const std::vector<int>& sinks)
    {
        residual_graph = flow_graph;

        int n = residual_graph.size();
        residual_graph.resize(n + 2);
        visited.resize(n + 2, false);

        source = n, sink = n + 1;
        for (int s : sources) add_source(s);
        for (int s : sinks  ) add_sink(s);

        int bottleneck = -1;
        while (bottleneck != 0) {
            std::fill(visited.begin(), visited.end(), false);
            bottleneck = add_augmenting_path(source);
        }
    }

    void calc_weights_sum()
    {
        weights_sum = 0;
        for (int from = 0; from < flow_graph.size(); from++) {
            for (int to : flow_graph[from]) {
                weights_sum += flow_graph.get_weight(from, to);
            }
        }
    }

    void add_source(int node) {
        // TODO add a way to specify maximum output from
        //  the source instead of always assigning weight_sum.
        residual_graph.add_weight(source, node, weights_sum);
    }

    void add_sink(int node) {
        // TODO add a way to specify maximum input to
        //  the sink instead of always assigning weight_sum.
        residual_graph.add_weight(node, sink, weights_sum);
    }

public:

    MaxFlowCalculator(const Graph& graph) : flow_graph(graph),
        visited(graph.size(), false), residual_graph({}) { calc_weights_sum(); }

    std::vector<Edge> get_max_flow(const std::vector<int>& sources, const std::vector<int>& sinks)
    {
        compute_residual_graph(sources, sinks);
        return get_flow_edges();
    }
};

Graph get_sample_graph_1()
{
    Graph graph(6);

    graph.add_weight(0, 2, 13);
    graph.add_weight(1, 2, 10);
    graph.add_weight(1, 3, 12);
    graph.add_weight(3, 2, 9);
    graph.add_weight(2, 4, 14);
    graph.add_weight(2, 5, 4);
    graph.add_weight(3, 5, 20);

    return graph;
}

Graph get_sample_graph_2()
{
    Graph graph(11);

    graph.add_weight(0, 1, 7);
    graph.add_weight(0, 2, 2);
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

void test(const Graph& graph, const std::vector<int> &sources, const std::vector<int> &sinks)
{
    int total_flow = 0;
    auto edges = MaxFlowCalculator(graph).get_max_flow(sources, sinks);
    for (Edge& edge : edges)
    {
        std::cout << edge.from << " --" << edge.weight;
        if (edge.weight < 10) std::cout << ' ';
        std::cout << "--> " <<  edge.to << std::endl;

        bool is_sink = std::find(sinks.begin(), sinks.end(), edge.to) != sinks.end();
        if (is_sink) {
            total_flow += edge.weight;
        }
    }
    std::cout << "Total Flow: " << total_flow << std::endl << std::endl;
}

int main()
{
    test(get_sample_graph_1(), {0, 1}, {4, 5});
    test(get_sample_graph_2(), {0, 3}, {10});
}