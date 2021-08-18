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
    int V = graph.nodes_count;
    ShortestPathInfo info(V);
    auto &parent_of = info.parent_of;
    auto &minimum_distances = info.minimum_distances;

    minimum_distances[source] = 0;

    // Intuition: If a path is affected by a negative cycle, we can
    // relax the edges forever. We run the whole thing one more time
    // and if we managed to relax an edge in the second time, this
    // means that there exists a negative cycle, thus we set the the
    // value to -infinity (or -MAX_VAL in this case) so that the
    // currently being processed node gets marked as affected by a
    // negative cycle. The code here doesn't differentiate the nodes
    // directly involved in a negative cycle and the ones affected by
    // it.

    // NOTE: you can't just relax all of the edges one more time to
    // detect nodes affected by negative cycles. You have to relax
    // all edges for V - 1 times again for the same reason you can't
    // only relax for one time to compute the shortest distances.
    // You have to relax all edges for V - 1 times because you don't
    // relax the edges in order and one iteration can only guarantee
    // a relaxation of only one node (you might relax more, but it's
    // not guaranteed).

    for (int x = 1; x <= 2; x++) {
        for (int i = 0; i < V - 1; i++)
        {
            bool relaxed = false;

            for (auto &edge : graph.edges)
            {
                int old_weight = minimum_distances[edge.to];
                int new_weight = minimum_distances[edge.from] + edge.weight;

                if (new_weight < old_weight)
                {
                    minimum_distances[edge.to] =
                            (x == 1) ? new_weight : -MAX_VAL;

                    parent_of[edge.to] =
                            (x == 1) ? edge.from : -1;

                    relaxed = true;
                }
            }

            if (!relaxed) {
                break;
            }
        }
    }

    return info;
}

void print_path_info(const ShortestPathInfo &info, int source, int target)
{
    std::stack<int> stack;

    bool print = true;

    int current = target;
    while (current != source) {
        if (current == -1) {
            std::cout << "The given info doesn't represent a valid path from "
                      << source << " to " << target << " which indicates that the path doesn't exits." << std::endl;
            print = false;
            break;
        }
        stack.push(current);
        current = info.parent_of[current];
    }

    if (print)
    {
        std::cout << source;

        while (!stack.empty()) {
            std::cout << " -> " << stack.top();
            stack.pop();
        }

        std::cout << std::endl;
    }

    for (int i = 0; i < info.parent_of.size(); i++) {

        if (i == source) continue;

        int distance = info.minimum_distances[i];
        if (distance == MAX_VAL) {
            std::cout << "There is no path from "
                      << source << " to " << i << "." << std::endl;
        } else if (distance == -MAX_VAL) {
            std::cout << "The path from " << source << " to " << i
                << " is affected by a negative cycle, thus, there is no minimum distance." << std::endl;
        } else {
            std::cout << "Minimum distance from " << source
                      << " to " << i << " is " << distance << "." << std::endl;
        }
    }
}

Graph get_sample_graph()
{
    Graph graph {6};

    graph.add_edge(1, 2, 5);
    graph.add_edge(2, 1, -10);

    graph.add_edge(4, 5, 2);

    graph.add_edge(5, 2, 3);

    graph.add_edge(5, 3, 4);

    return graph;
}

int main()
{
    auto graph = get_sample_graph();
    auto info = bellman_ford(graph, 4);
    print_path_info(info, 4, 3);
}