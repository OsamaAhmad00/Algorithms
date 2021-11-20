#include <iostream>
#include <vector>
#include <queue>

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

class MaxFlowCalculator
{
    int source, sink;
    const Graph flow_graph;
    Graph residual_graph;
    std::vector<int> parent_of;

    int get_flow_value(int from, int to) const
    {
        // This assumes that if x pushes some
        //  flow to y, y won't be pushing anything
        //  back to x. In other words, the flow
        //  flows only in one direction for any
        //  given pair of nodes.
        // If both from->to and to->from exists,
        //  we should subtract the edge to->from
        //  of the flow_graph from the one of the
        //  residual_graph. If to->from doesn't
        //  exist, we're fine since to->from of
        //  the flow_graph will be 0.
        int a = flow_graph.get_weight(to, from);
        int b = residual_graph.get_weight(to, from);
        return b - a;
    }

    int add_augmenting_path()
    {
        struct QueueNode
        {
            int node;
            int bottleneck;
        };

        std::queue<QueueNode> queue;
        std::fill(parent_of.begin(), parent_of.end(), -1);

        parent_of[source] = source;
        queue.push({source, INT_MAX});

        while (!queue.empty())
        {
            QueueNode current = queue.front();
            queue.pop();

            for (int child : residual_graph[current.node])
            {
                if (parent_of[child] != -1) continue;
                int weight = residual_graph.get_weight(current.node, child);
                if (weight <= 0) continue;

                parent_of[child] = current.node;
                int bottleneck = std::min(weight, current.bottleneck);

                if (child == sink)
                {
                    int parent = current.node;
                    while (child != source) {
                        residual_graph.add_weight(parent, child, -bottleneck);
                        residual_graph.add_weight(child, parent, bottleneck);
                        child = parent;
                        parent = parent_of[parent];
                    }

                    return bottleneck;
                }

                queue.push({child, bottleneck});
            }
        }

        return 0;
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
        // This is an improvement over Fold-Fulkerson's algorithm.
        // The only difference here is that BFS is used to find
        // the augmenting paths. We try to find the shortest paths
        // first, shortest in terms of edges, not weight, in other
        // words, paths with the fewest possible edges.
        // This changes the runtime from O(|E| * F) to O(|E|^2 * |V|)
        // where |E| is the number of edges and |V| is the number
        // of nodes in the graph. Even though this might be a lot,
        // it's polynomial, and gives us a bound that's independent
        // of the total flow value.

        // Why using BFS changes the runtime complexity to O(|E|^2 * |V|)?
        //  1 - Using BFS, we get the paths with the fewest possible edges first.
        //      This means that if we have n available augmenting paths with length
        //      l, and l is the minimum length for an augmenting path, then the first
        //      n times must return a path with length n.
        //  2 - For every augmenting path, there is at least an edge that gets fully
        //      saturated, and replaced by a reverse-edge.
        //  3 - (Important) If an edge e is fully saturated, it's reverse-edge won't
        //      be used in an augmenting path again until the minimum length for
        //      augmenting paths increase.
        //      Consider the following path:
        //      S --x--> U --1--> V --y--> T where S is the source and T is the sink
        //      (A --x--> B means that the minimum path from A to B contains x edges)
        //      Initially, distance(S, V) = x + 1, distance(U, T) = 1 + y, distance(S, T)
        //      = x + 1 + y (minimum distances).
        //      If the path from U to V is saturated, and replaced with a reverse-edge,
        //      distance(S, V) >= x + 1, distance(U, T) >= 1 + y.
        //      To use the reverse-edge of U->V which goes from V to U, this means that
        //      we have to at least get from S to V with a minimum distance >= 1 + y,
        //      then from V to U with a distance of 1, then from U to T with a minimum
        //      distance >= x + 1. This in total means that we have to take a path with
        //      length >= (1 + y) + 1 + (x + 1) = x + y + 3.
        //      Since the current minimum distance is x + 1 + y, the minimum length for
        //      an augmenting path should increase to more than or equal to x + y + 3 in
        //      order for this reverse-edge to be taken.
        //  4 - The length of the path with the fewest number of edges can only
        //      increase |V| times.
        //  5 - Each time the length increases, we can only have O(|E|) many saturated edges.
        //  6 - By number 4 and number 5, we can conclude that we can have only O(|V| * |E|)
        //      augmenting paths.
        //  7 - Each path takes O(|E|) to compute.
        //  8 - By number 6 and number 7, we can conclude that the total runtime is O(|E|^2 * |V|).

        residual_graph = flow_graph;
        this->source = source;
        this->sink = sink;
        while (add_augmenting_path() != 0);
    }

public:

    MaxFlowCalculator(const Graph& graph) : flow_graph(graph),
        parent_of(graph.size()), residual_graph({}) {}

    std::vector<Edge> get_max_flow(int source, int sink)
    {
        compute_residual_graph(source, sink);
        return get_flow_edges();
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

    int total_flow = 0;
    int source = 0;
    int sink = graph.size() - 1;
    auto edges = MaxFlowCalculator(graph).get_max_flow(source, sink);
    for (Edge& edge : edges)
    {
        std::cout << edge.from << " --" << edge.weight;
        if (edge.weight < 10) std::cout << ' ';
        std::cout << "--> " <<  edge.to << std::endl;
        if (edge.to == sink) total_flow += edge.weight;
    }
    std::cout << "Total Flow: " << total_flow << std::endl << std::endl;
}

int main()
{
    test(get_sample_graph_1());
    test(get_sample_graph_2());
}