#include <iostream>
#include <vector>

// Use a different data type, or change
//  this value if it's not suitable for
//  your application.
const int MAX_VAL = 1'000'000;

struct Edge
{
    int from;
    int to;
    int weight;
};

class Graph : public std::vector<std::vector<int>>
{
    std::vector<std::vector<int>> weights;
    std::vector<std::vector<int>> costs;

public:

    // Note that costs here are per unit of flow.
    //  If the cost is 2, and there is a flow of
    //  3, then the cost is 6 and not 2.

    // Is a good idea to initialize costs with 0s?
    explicit Graph(int n) : std::vector<std::vector<int>>(n),
                            weights(n, std::vector<int>(n, 0)),
                            costs(n, std::vector<int>(n, 0)) {}

    void add_weight(int from, int to, int weight) {
        if (weights[from][to] == 0) {
            (*this)[from].push_back(to);
        }
        weights[from][to] += weight;
    }

    // TODO set a better way to deal with costs
    void add_weight(int from, int to, int weight, int cost)
    {
        add_weight(from, to, weight);
        costs[from][to] += cost;
        costs[to][from] -= cost;
    }

    int get_weight(int from, int to) const { return weights[from][to]; }
    int get_cost(int from, int to) const { return costs[from][to]; }
};

class MinCostFlowCalculator
{
    int source;
    int sink;
    const Graph flow_graph;
    Graph residual_graph;

    int get_flow_value(int from, int to) const
    {
        int a = flow_graph.get_weight(to, from);
        int b = residual_graph.get_weight(to, from);
        return b - a;
    }

    std::vector<Edge> get_residual_edges_with_remaining_capacity() const
    {
        std::vector<Edge> result;
        for (int from = 0; from < residual_graph.size(); from++) {
            for (int to: residual_graph[from]) {
                int weight = residual_graph.get_weight(from, to);
                if (weight > 0) {
                    result.push_back({from, to, weight});
                }
            }
        }
        return result;
    }

    std::vector<Edge> get_shortest_augmenting_path()
    {
        // Bellman-Ford is being used here since
        //  it can deal with negative values.

        // Since the shortest path algorithm is
        //  concerned only about the cost, it can
        //  pick an edge with no capacity left.
        // To avoid this case, we only process
        //  edges with a remaining capacity.
        std::vector<Edge> edges = get_residual_edges_with_remaining_capacity();

        int V = residual_graph.size();
        std::vector<int> minimum_cost(V, MAX_VAL);
        std::vector<Edge> prev_edge(V, {-1, -1, -1});

        minimum_cost[source] = 0;

        for (int i = 0; i < V - 1; i++)
        {
            bool relaxed = false;

            for (auto &e : edges)
            {
                int cost = residual_graph.get_cost(e.from, e.to);
                int old_cost = minimum_cost[e.to];
                int new_cost = minimum_cost[e.from] + cost;

                if (new_cost < old_cost) {
                    minimum_cost[e.to] = new_cost;
                    prev_edge[e.to] = e;
                    relaxed = true;
                }
            }

            if (!relaxed) {
                break;
            }
        }

        if (prev_edge[sink].to != sink) {
            // No path from the source to the sink.
            return {};
        }

        std::vector<Edge> path;

        // This path is reversed, but it won't matter for this purpose.
        for (int node = sink; node != source; node = prev_edge[node].from)
            path.push_back(prev_edge[node]);

        return path;
    }

    int add_shortest_augmenting_path()
    {
        std::vector<Edge> path = get_shortest_augmenting_path();

        if (path.empty()) return 0;

        int bottleneck = MAX_VAL;
        for (Edge &e : path)
            bottleneck = std::min(bottleneck, e.weight);

        for (Edge &e : path)
        {
            residual_graph.add_weight(e.from, e.to, -bottleneck);
            residual_graph.add_weight(e.to, e.from,  bottleneck);
        }

        return bottleneck;
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
        this->source = source;
        this->sink = sink;
        int bottleneck = -1;
        while (bottleneck != 0) {
            // The idea here is exactly the same
            //  as the normal Ford-Fulkerson algorithm,
            //  the only difference is that instead of
            //  adding any augmenting paths, we add the
            //  "shortest" augmenting paths (shortest
            //  in terms of cost, and not weight) first.
            // In other words, we find the paths with
            //  the least cost and add them first.
            bottleneck = add_shortest_augmenting_path();
        }
    }

public:

    MinCostFlowCalculator(const Graph& graph)
        : flow_graph(graph), residual_graph({}) {}

    std::vector<Edge> get_min_cost_flow(int source, int sink)
    {
        compute_residual_graph(source, sink);
        return get_flow_edges();
    }
};

Graph get_sample_graph_1()
{
    Graph g(4);
    g.add_weight(0, 1, 4, 10);
    g.add_weight(0, 2, 2, 30);
    g.add_weight(1, 2, 2, 10);
    g.add_weight(1, 3, 0, 9999);
    g.add_weight(2, 3, 4, 10);
    return g;
}

Graph get_sample_graph_2()
{
    Graph g(6);

    g.add_weight(0, 1, 15, 4);
    g.add_weight(0, 2, 8, 4);
    g.add_weight(1, 2, 20, 2);
    g.add_weight(1, 3, 4, 2);
    g.add_weight(1, 4, 10, 6);
    g.add_weight(2, 3, 15, 1);
    g.add_weight(2, 4, 4, 3);
    g.add_weight(3, 4, 20, 2);
    g.add_weight(3, 5, 5, 0);
    g.add_weight(4, 5, 15, 0);

    return g;
}

void test(const Graph& graph)
{
    // assumes that the source is 0 and the
    // sink is the node with the largest number.

    int total_flow = 0;
    int total_cost = 0;
    int source = 0;
    int sink = graph.size() - 1;
    auto edges = MinCostFlowCalculator(graph).get_min_cost_flow(source, sink);
    for (Edge& edge : edges)
    {
        std::cout << edge.from << " --" << edge.weight;
        if (edge.weight < 10) std::cout << ' ';
        std::cout << "--> " <<  edge.to;
        std::cout << " (cost per unit = " << graph.get_cost(edge.from, edge.to);
        std::cout << ")" << std::endl;
        if (edge.to == sink) total_flow += edge.weight;
        total_cost += graph.get_cost(edge.from, edge.to) * edge.weight;
    }
    std::cout << "Total Flow: " << total_flow << std::endl;
    std::cout << "Total Cost: " << total_cost << std::endl << std::endl;
}

int main()
{
    test(get_sample_graph_1());
    test(get_sample_graph_2());
}
