#include <iostream>
#include <vector>
#include <queue>

struct Edge
{
    int to;
    int weight;
};

struct BiEdge
{
    int from;
    int to;
    int weight;

    bool operator>(const BiEdge& edge) const {
        return weight > edge.weight;
    }
};

struct Graph
{
    std::vector<std::vector<Edge>> adjList;

    Graph(int size) : adjList(size) {}

    void add_edge(int from, int to, int weight) {
        adjList[from].push_back({to, weight});
        adjList[to].push_back({from, weight});
    }

    void add_edge(const BiEdge& edge) {
        add_edge(edge.from, edge.to, edge.weight);
    }

    int size() const { return adjList.size(); }
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
    Graph result(graph.size());

    std::priority_queue<BiEdge, std::vector<BiEdge>, std::greater<>> queue;
    std::vector<bool> visited(graph.size(), false);

    int root = 1;
    visited[root] = true;
    for (const Edge& edge : graph.adjList[root]) {
        queue.push({root, edge.to, edge.weight});
    }

    // nodes start from 1 and not 0.
    int size = graph.size() - 1;

    // a tree contains only n-1 edges.
    for (int i = 0; i < size - 1; i++)
    {
        while (!queue.empty() && visited[queue.top().to]) queue.pop();

        // the graph is disconnected.
        if (queue.empty()) {
            break;
        }

        BiEdge curEdge = queue.top();
        int curNode = curEdge.to;
        visited[curNode] = true;
        queue.pop();

        result.add_edge(curEdge);

        for (const Edge& edge : graph.adjList[curNode]) {
            if (!visited[edge.to]) {
                queue.push({curNode, edge.to, edge.weight});
            }
        }
    }

    return result;
}

void print_graph_edges(const Graph& graph)
{
    for (int i = 0; i < graph.size(); i++) {
        for (const Edge& edge : graph.adjList[i]) {
            if (edge.to < i) {
                // already visited
                continue;
            }

            std::cout << "From node " << i << " to node " << edge.to
                << " with weight " << edge.weight << std::endl;
        }
    }
}

int main()
{
    auto graph = get_graph();
    auto mst = get_MST(graph);
    print_graph_edges(mst);
}