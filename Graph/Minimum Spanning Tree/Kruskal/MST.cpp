#include <iostream>
#include <algorithm>
#include <vector>

struct Edge
{
    int from;
    int to;
    int weight;

    bool operator<(const Edge& edge) const {
        return weight < edge.weight;
    }
};

struct Graph
{
    int nodes_count;
    std::vector<Edge> edges;

    Graph(int nodes_count) : nodes_count(nodes_count) {}

    void add_edge(int from, int to, int weight) {
        add_edge({from, to, weight});
    }

    void add_edge(const Edge& edge) {
        edges.push_back(edge);
    }
};

struct UnionFind
{
    // Optimization: sizes will be stored in the parents array.
    //  if node x has a parent of -5, this means that x is a
    //  root and the size of the component (forest) is 5.

    // not using ranks here.

    int node_count;
    int components; // (forests)
    std::vector<int> parent;

    UnionFind(int size) : node_count(size), components(size), parent(size, -1) {}

    int parent_of(int x)
    {
        if (parent[x] < 0) return x;
        parent[x] = parent_of(parent[x]);
        return parent[x];
    }

    int component_size(int x) {
        return -parent[parent_of(x)];
    }

    bool are_connected(int a, int b) {
        return parent_of(a) == parent_of(b);
    }

    void connect(int a, int b)
    {
        if (are_connected(a, b)) return;

        components--;
        parent[parent_of(a)] += -component_size(b);
        parent[parent_of(b)] = parent_of(a);
    }
};

Graph get_graph()
{
    Graph graph {6};

    graph.add_edge(2, 1, 8);
    graph.add_edge(4, 1, 5);
    graph.add_edge(2, 3, 100);
    graph.add_edge(5, 3, 3);
    graph.add_edge(4, 5, 2);

    return graph;
}

Graph get_MST(const Graph& graph)
{
    Graph result(graph.nodes_count);

    UnionFind unionFind(graph.nodes_count);

    std::vector<Edge> edges = graph.edges;
    std::sort(edges.begin(), edges.end());

    for (const Edge& edge : edges) {
        if (unionFind.are_connected(edge.from, edge.to)) {
            continue;
        }
        result.add_edge(edge);
        unionFind.connect(edge.from, edge.to);
    }

    if (result.edges.size() != graph.nodes_count - 1) {
        // the graph is disconnected.
    }

    return result;
}

void print_graph_edges(const Graph& graph)
{
    for (const Edge& edge : graph.edges) {
        std::cout << "From node " << edge.from << " to node "
            << edge.to << " with weight " << edge.weight << std::endl;
    }
}

int main()
{
    auto graph = get_graph();
    auto mst = get_MST(graph);
    print_graph_edges(mst);
}