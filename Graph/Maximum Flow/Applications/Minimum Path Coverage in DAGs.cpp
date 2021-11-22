#include <iostream>
#include <vector>
#include <algorithm>

struct Edge
{
    int from;
    int to;
};

class Graph : public std::vector<std::vector<int>>
{
    std::vector<std::vector<bool>> weights;

public:

    explicit Graph(int n) : std::vector<std::vector<int>>(n),
        weights(n, std::vector<bool>(n, 0)) {}

    void connect(int from, int to) {
        if (!weights[from][to]) {
            (*this)[from].push_back(to);
        }
        weights[from][to] = true;
    }

    void unconnect(int from, int to) {
        weights[from][to] = false;
    }

    bool is_connected(int from, int to) const { return weights[from][to]; }
};

class MaximumBipartiteMatchingCalculator
{
    int n;
    int source, sink;
    std::vector<Edge> edges;
    Graph residual_graph;
    std::vector<bool> visited;

    bool has_flow(int from, int to) const
    {
        return residual_graph.is_connected(to, from);
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
            int weight = residual_graph.is_connected(from, to);
            int value = add_augmenting_path(to, std::min(weight, bottleneck));
            if (value > 0) {
                residual_graph.unconnect(from, to);
                residual_graph.connect(to, from);
                result = value;
                break;
            }
        }

        return result;
    };

    std::vector<Edge> get_flow_edges()
    {
        std::vector<Edge> result;

        for (auto &edge : edges) {
            if (has_flow(edge.from, edge.to)) {
                result.push_back(edge);
            }
        }

        return result;
    }

    void compute_residual_graph()
    {
        residual_graph = Graph(n + 2);

        for (Edge& edge : edges) {
            residual_graph.connect(edge.from, edge.to);
            residual_graph.connect(source, edge.from);
            residual_graph.connect(edge.to, sink);
        }

        int bottleneck = -1;
        while (bottleneck != 0) {
            std::fill(visited.begin(), visited.end(), false);
            bottleneck = add_augmenting_path(source);
        }
    }

public:

    // This is an edited version of the general
    // multi-source multi-sink max flow solver
    // that allows edges to have only a weight of 1.

    // Let A and B be the two sets of the bipartite graph.
    // For an edge e, e.from is considered to be a node in set A
    // and e.to will be considered as a node in set B.
    MaximumBipartiteMatchingCalculator(int n, std::vector<Edge>  edges)
        : n(n), edges(std::move(edges)), residual_graph({})
    {
        source = n;
        sink = n + 1;
        visited.resize(n + 2, false);
    }

    std::vector<Edge> get_max_flow()
    {
        compute_residual_graph();
        return get_flow_edges();
    }
};

std::vector<std::vector<int>> get_min_path_coverage(const Graph& graph)
{
    // This function returns the minimum number of
    // paths to cover every node in the graph exactly once.

    // Intuition:
    // Let g be a bipartite graph constructed from the given DAG.
    // Constructing a bipartite graph out of a DAG happens as follows:
    //  - let n = size of the DAG
    //  - create a bipartite graph with size = n * 2
    //  - for every edge in the DAG from u to v, connect u and (v + n) in the bipartite graph.
    // You can think of the node x where 0 <= x < n in the bipartite graph as the node that represents the
    // node x as being a parent and the node (x + n) as being the node that represents the node x as a child.
    // Now, get the maximum possible matchings of g.
    // Each match in g from u to (v + n) means that the edge u->v will be included in the result.
    // Trying to maximize the number of matches = trying to get as many nodes to be connected by an edge.
    // Since each node 0 <= u < n will have at most 1 edge going out of it, and each node n <= (u + v) <= n * 2
    // will have at most one node going into it in the graph with maximum matches, This means that each
    // node will have at most one parent and at most one child.
    // By maximizing the number of connected nodes, we minimize the number of the overall paths.
    // the number of the paths = the number of nodes - the number of matches. This is because the unmatched
    // nodes are nodes with no edges going out of it (or with no edges going into it if the index >= n), and
    // a node with no edges going out of it is an end of a path. So, we can simply count the number of
    // the nodes with no edges going out of it (or into it) to get the minimum paths count, or simply,
    // the number of nodes - the number of matches.

    // https://stackoverflow.com/questions/17020326/minimum-path-cover-in-dag

    std::vector<Edge> edges;

    int n = graph.size();

    for (int from = 0; from < n; from++) {
        for (int to : graph[from]) {
            edges.push_back({from, to + n});
        }
    }

    std::vector<Edge> result = MaximumBipartiteMatchingCalculator(n * 2, edges).get_max_flow();

    std::vector<int> parent_of(n, -1);
    std::vector<int> child_of(n, -1);
    for (Edge& edge : result) {
        int to = edge.to - n;
        parent_of[to] = edge.from;
        child_of[edge.from] = to;
    }

    int matches_count = result.size();
    int paths_count = n - matches_count;

    std::vector<std::vector<int>> paths;
    for (int i = 0; i < n; i++) {
        if (parent_of[i] == -1) {
            paths.emplace_back();
            int current = i;
            while (current != -1) {
                paths.back().push_back(current);
                current = child_of[current];
            }
        }
    }

    // ASSERT(paths.size() == paths_count);
    return paths;
}

Graph get_sample_graph_1()
{
    Graph graph(4);

    graph.connect(0, 1);
    graph.connect(0, 2);
    graph.connect(1, 3);
    graph.connect(2, 3);

    return graph;
}

Graph get_sample_graph_2()
{
    Graph graph(6);

    graph.connect(0, 1);
    graph.connect(0, 4);
    graph.connect(1, 4);
    graph.connect(2, 3);
    graph.connect(4, 5);
    graph.connect(5, 2);
    graph.connect(5, 3);

    return graph;
}

Graph get_sample_graph_3()
{
    Graph graph(6);

    graph.connect(0, 1);
    graph.connect(0, 4);
    graph.connect(1, 4);
    graph.connect(2, 3);
    graph.connect(5, 2);
    graph.connect(5, 3);

    return graph;
}

Graph get_sample_graph_4()
{
    Graph graph(6);

    graph.connect(0, 4);
    graph.connect(1, 4);
    graph.connect(4, 5);
    graph.connect(5, 2);
    graph.connect(5, 3);

    return graph;
}

void test(const Graph& graph)
{
    auto paths = get_min_path_coverage(graph);
    std::cout << "Minimum number of paths: " << paths.size() << std::endl;
    for (auto& path : paths) {
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
    test(get_sample_graph_3());
    test(get_sample_graph_4());
}