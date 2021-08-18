#include <functional>
#include <iostream>
#include <cstring>
#include <vector>
#include <stack>

const int MAX_VAL = 1'000'000;

struct Edge
{
    int from;
    int to;
    int weight;
};

struct Graph
{
    int nodes_count;
    std::vector<Edge> edges;

    void add_edge(int from, int to, int weight) {
        edges.push_back({from, to, weight});
    }
};

struct ShortestPathInfo
{
    std::vector<int> parent_of;
    std::vector<int> minimum_distances;

    ShortestPathInfo(int n)
            : parent_of(n, -1), minimum_distances(n, MAX_VAL) {}
};

ShortestPathInfo bellman_ford(const Graph &graph, int source)
{
    // Note: any sub-path of a shortest path is a shortest path as well
    // Example: if the shortest path from x to y is x -> a -> b -> c -> y,
    // the shortest path from a to c will be a -> b -> c. This is because
    // if there were a shorter path from a to c, it should've been taken
    // in the path from x to y to make it shorter.

    // Intuition:
    //
    // Any shortest path will be a simple path
    // (does not repeat nodes) and will consist of at most V nodes
    // (involves every single node in the graph once). This means
    // that the maximum number of edges in a simple shortest path
    // is V - 1.
    //
    // To calculate the minimum distance from the source to all other nodes
    // and be able to get them in one step, we will create an store the
    // minimum distances in an array. You can think of every index in the
    // array as an edge to the source. To construct these edges, we'll have
    // to "relax" all edges for V - 1 times.
    //
    // For a given path, if we know the order of the nodes in the path, we can
    // compute the distance from the source to every node in the path in a single
    // pass (instead of in V - 1 passes). This is done by relaxing starting from
    // the source and ending at the end of the path, each time using the information
    // about the previous node. In a single pass, the worst case is that we relax
    // only a single edge. Best case is that we relax all edges in one pass.
    //
    // Since we don't have any information about the shortest paths, and since any
    // shortest path consists of at most V - 1 edges, and since worst case for a
    // single pass guarantees the relaxation of at least a single edge, if we perform
    // V - 1 passes, then it's guaranteed that at the end, all edges will be relaxed
    // with respect to the source.
    //
    // We set the distance to the source to 0 and all of the other distances to infinity
    // (or MAX_VAL in this case) so that the relaxation happens with respect to the source.
    //
    // This algorithm is not a greedy algorithm (like dijkstra) and will work with negative
    // edge value with no problem, given that no negative cycles exist.

    int V = graph.nodes_count;
    ShortestPathInfo info(V);
    auto &parent_of = info.parent_of;
    auto &minimum_distances = info.minimum_distances;

    minimum_distances[source] = 0;

    for (int i = 0; i < V - 1; i++) {
        for (auto &edge : graph.edges) {
            int old_weight = minimum_distances[edge.to];
            int new_weight = minimum_distances[edge.from] + edge.weight;
            if (new_weight < old_weight) {
                minimum_distances[edge.to] = new_weight;
                parent_of[edge.to] = edge.from;
            }
        }
    }

    return info;
}

void print_path_info(const ShortestPathInfo &info, int source, int target)
{
    std::stack<int> stack;

    int current = target;
    while (current != source) {
        if (current == -1) {
            std::cout << "The given info doesn't represent a valid path from "
                      << source << " to " << target << " which indicates that the path doesn't exits." << std::endl;
            return;
        }
        stack.push(current);
        current = info.parent_of[current];
    }

    std::cout << source;
    while (!stack.empty()) {
        std::cout << " -> " << stack.top();
        stack.pop();
    }

    std::cout << std::endl;

    for (int i = 0; i < info.parent_of.size(); i++) {

        if (i == source) continue;

        int distance = info.minimum_distances[i];
        if (distance != MAX_VAL) {
            std::cout << "Minimum distance from " << source
                << " to " << i << " is " << distance << "." << std::endl;
        } else {
            std::cout << "There is no path from "
                << source << " to " << i << "." << std::endl;
        }
    }
}

Graph get_sample_graph()
{
    Graph graph {6};

    graph.add_edge(2, 1, 8);

    graph.add_edge(4, 1, 5);
    graph.add_edge(1, 4, 5);

    graph.add_edge(2, 3, 100);

    graph.add_edge(5, 3, 3);

    graph.add_edge(4, 5, 2);
    graph.add_edge(5, 4, 2);

    return graph;
}

int main()
{
    auto graph = get_sample_graph();
    auto info = bellman_ford(graph, 2);
    print_path_info(info, 2, 3);
}