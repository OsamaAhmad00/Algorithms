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

class MaxFlowCalculator
{
    int sink;
    const Graph flow_graph;
    Graph residual_graph;
    std::vector<bool> visited;

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

    void calc_residual_graph(int source, int sink)
    {
        // The algorithm:
        // 1 - total_flow = 0
        // 2 - Find an "augmenting path" in the "residual graph".
        //     An augmenting path is just a path from the source to
        //     the sink in the residual graph.
        // 3 - Add the value of the bottleneck of the path to total_flow.
        //     This value is reserved for the flow
        // 4 - Subtract the value of the bottleneck of the path from
        //     each edge along the path.
        // 5 - Add the value of the bottleneck to the reverse edge of
        //     each edge along the path. This is a way of allowing
        //     some flow to return later.
        // 6 - if 2 is possible, start from 2 again, otherwise, return.

        // Intuition:
        // if the augmenting path doesn't contain any of the created
        // reverse edges, then it's clear that we've just found another
        // path for the flow to flow in.
        // if on the other hand, the augmenting path contains some of
        // these edges, then this path isn't a real complete path, it's
        // just a representation for adding some more edges and redirecting
        // some flow to get more flow going through the network.
        // Suppose there was an augmenting path S->a->b->c->d->T where b->c
        // is a reverse edge that is not in the original flow_graph, S = source, and T = sink.
        // The meaning of taking b->c is the following (drawing might help):
        // First, if b->c is a reverse edge, this means that there is an edge c->b.
        // We will split the path into two parts since each part has a meaning to it.
        // The part b->c->d->T means: redirect the flow (or some of it)
        // that is going through the edge c->b and let it go through c->d, the path
        // c->d->T can handle this extra flow.
        // But doing this alone will make the flow going to the node b less, thus
        // the flow going out of it should decrease too. This is where the S->a->b part
        // comes into play. This part means: the path S->a->b (that is connected to the
        // source) can supply the amount that is taken away from the node b so that
        // the edges coming out of node b are unchanged.
        // This way, since we've taken more from the source, we've increased the total
        // flow by some amount, which is the bottleneck of the augmenting path.

        // This also calculates the Minimum Cut of the graph. The reason is that, if
        // you think about it, the maximum flow will be bounded by any cut on the graph.
        // And since the max flow <= every single cut value, only the minimum cut can only
        // be equal to the max flow.

        // This code has an O(|E| * F) where |E| is the number of edges and F is
        // the maximum flow. This is dependent on the value of the flow, we'd love
        // to change the runtime complexity so that it's irrelevant of the max flow.

        residual_graph = flow_graph;
        this->sink = sink;
        while (add_augmenting_path(source) != 0);
    }

public:

    MaxFlowCalculator(const Graph& graph) : flow_graph(graph),
        visited(graph.size(), false), residual_graph({}) {}

    std::vector<Edge> get_max_flow(int source, int sink)
    {
        calc_residual_graph(source, sink);
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