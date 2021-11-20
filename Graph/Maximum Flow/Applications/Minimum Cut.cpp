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
        // if the weight of an edge is set to 0, then
        // some weight is added, the edge from "from"
        // to "to" will be added again. it's not worth
        // checking for this condition.
        if (weights[from][to] == 0) {
            (*this)[from].push_back(to);
        }
        weights[from][to] += weight;
    }

    int get_weight(int from, int to) const { return weights[from][to]; }
};

class MinCutCalculator
{
    int sink;
    const Graph flow_graph;
    Graph residual_graph;
    std::vector<bool> visited;

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

    void compute_residual_graph(int source, int sink)
    {
        // Ford Fulkerson
        residual_graph = flow_graph;
        this->sink = sink;
        int bottleneck = -1;
        while (bottleneck != 0) {
            std::fill(visited.begin(), visited.end(), false);
            bottleneck = add_augmenting_path(source);
        }
    }

    void mark_reachable_edges(int from)
    {
        visited[from] = true;
        for (int to : residual_graph[from]) {
            // not the flow value
            if (!visited[to] && residual_graph.get_weight(from, to) > 0) {
                mark_reachable_edges(to);
            }
        }
    }

    std::vector<Edge> get_min_cut_edges()
    {
        std::vector<Edge> result;
        for (int from = 0; from < flow_graph.size(); from++) {
            if (!visited[from]) continue;
            for (int to : flow_graph[from]) {
                if (!visited[to]) {
                    result.push_back({from, to, flow_graph.get_weight(from, to)});
                }
            }
        }
        return result;
    }

public:

    MinCutCalculator(const Graph& graph) : flow_graph(graph),
                                            visited(graph.size(), false), residual_graph({}) {}

    std::vector<Edge> get_min_cut(int source, int sink)
    {
        // Intuition:
        // Let's define 2 sets, S and T. Let S be a set with the source
        // and all reachable nodes from the source in it and T be  a set
        // with the sink and all nodes that can reach the sink in it.
        // 1 - applying a cut on the graph makes S and T disconnected. If
        //  there is an edge from S to T {u, v} such that u is in S and v
        //  is in T, this would be a contradiction since v will be in S
        //  (since it's reachable from the source).
        // 2 - By computing the maximum flow, every path in the residual
        //  graph will have at least one edge that is fully saturated,
        //  which means that we can't push more flow through it, which
        //  means that we can't use this edge anymore.
        // 3 - The maximum flow value = the sum of the capacity of the
        //  unique bottleneck edges of each path from the source to the
        //  sink. Note that there can be a single bottleneck edge for
        //  multiple paths.
        // 4 - Cutting these bottleneck edges will result in a cut and
        //  will make S and T disjoint with a cost = the maximum flow.
        // 5 - Since the maximum flow value <= any cut value, if the
        //  maximum flow value = the cut value, the cut is a min cut.

        // Since after computing the maximum flow, the residual graph
        // will have the bottleneck edges remaining capacities set to
        // 0, which means that they're no longer a valid edge (in the
        // residual graph), we can think of it as if a cut was made.
        // But if we think this way, if a path contains multiple bottleneck
        // edges, all of these will be considered to be cut. This means
        // that we can't just return all bottleneck edges, rather, we
        // want to have only a single bottleneck edge to be cut in each
        // path. To achieve this, we'll only add the first bottleneck
        // edge that we encounter. Since S and T are disconnected, we
        // can mark all nodes that are reachable from the source, "reachable"
        // here means that we can reach it through edges that still have
        // remaining capacity. After marking them, now we know that any
        // marked node is in S and any unmarked node is in T. We can just
        // return any edge that connects from S to T, in other words,
        // an edge that goes from a marked node to an unmarked node.
        // These nodes are guaranteed to be a bottleneck edges, and the
        // sum of their values = the maximum flow. They're also the first
        // bottlenecks to encounter in the paths from the source to the sink.

        compute_residual_graph(source, sink);
        mark_reachable_edges(source);
        return get_min_cut_edges();
    }
};

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

    int min_cut = 0;
    int source = 0;
    int sink = graph.size() - 1;
    auto edges = MinCutCalculator(graph).get_min_cut(source, sink);
    for (Edge& edge : edges)
    {
        std::cout << edge.from << " --" << edge.weight;
        if (edge.weight < 10) std::cout << ' ';
        std::cout << "--> " <<  edge.to << std::endl;
        min_cut += edge.weight;
    }
    std::cout << "Minimum Cut: " << min_cut << std::endl << std::endl;
}

int main()
{
    test(get_sample_graph_1());
    test(get_sample_graph_2());
}