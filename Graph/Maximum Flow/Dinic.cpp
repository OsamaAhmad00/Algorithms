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
    std::vector<bool> visited;

    std::vector<int> levels;
    std::vector<int> next_child_index;

    int get_flow_value(int from, int to) const
    {
        int a = flow_graph.get_weight(to, from);
        int b = residual_graph.get_weight(to, from);
        return b - a;
    }

    bool calculate_level_graph()
    {
        std::fill(levels.begin(), levels.end(), -1);
        std::queue<int> queue;

        queue.push(source);
        levels[source] = 0;

        int last_level = 1;
        while (!queue.empty())
        {
            int size = queue.size();
            while (size--)
            {
                int node = queue.front();
                queue.pop();

                for (int child : residual_graph[node]) {
                    int weight = residual_graph.get_weight(node, child);
                    if (weight > 0 && levels[child] == -1) {
                        levels[child] = last_level;
                        queue.push(child);
                    }
                }
            }
            last_level++;
        }

        // The sink is not reachable. If this is the
        // case, no more calculations will be done.
        return levels[sink] != -1;
    }

    int add_augmenting_path(int from, int bottleneck = INT_MAX)
    {
        if (bottleneck == 0 || from == sink) {
            return bottleneck;
        }

        int result = 0;
        visited[from] = true;

        // To avoid trying to visit already visited nodes or revisiting dead-ends, we
        // store the next index to try from the current node. This is reset to 0 when
        // the level graph is recalculated.
        for (int& i = next_child_index[from]; i < residual_graph[from].size(); i++)
        {
            int to = residual_graph[from][i];
            if (visited[to] || levels[to] != (levels[from] + 1)) continue;
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
        // There are at most V phases, each phase
        //  takes O(EV), so the complexity is O(E * V^2).
        // On unit networks, Dinic's algorithm works in
        //  O(E * sqrt(V)).
        // Visual explanation: https://www.youtube.com/watch?v=M6cm8UeeziI
        // Read more here: https://cp-algorithms.com/graph/dinic.html

        residual_graph = flow_graph;
        this->source = source;
        this->sink = sink;

        while (calculate_level_graph())
        {
            std::fill(next_child_index.begin(), next_child_index.end(), 0);

            int bottleneck = -1;
            while (bottleneck != 0) {
                std::fill(visited.begin(), visited.end(), false);
                bottleneck = add_augmenting_path(source);
            }
        }
    }

public:

    MaxFlowCalculator(const Graph& graph) : flow_graph(graph),
        visited(graph.size()), residual_graph({}), levels(graph.size()), next_child_index(graph.size()) {}

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