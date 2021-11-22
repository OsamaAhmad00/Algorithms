#include <iostream>
#include <vector>
#include <algorithm>

struct Edge
{
    // Let A and B be the two sets of the bipartite graph.
    // a is always the node in set A and b is always the
    // node in set B.
    int a;
    int b;
};

typedef std::vector<std::vector<int>> Graph;

class MaximumBipartiteMatchingCalculator
{
    std::vector<bool> visited;
    std::vector<Edge> edges;
    std::vector<int> matched_with;
    Graph graph;

    void construct_graph()
    {
        for (Edge &edge : edges) {
            graph[edge.a].push_back(edge.b);
            graph[edge.b].push_back(edge.a);
        }
    }

    bool try_matching(int node)
    {
        if (visited[node]) {
            return false;
        }
        visited[node] = true;

        for (int neighbour : graph[node]) {
            int matched_node = matched_with[neighbour];
            if (matched_node == -1 || try_matching(matched_node)) {
                // When the node neighbour tries to
                // match with this node, it'll see that
                // it's visited, so it won't consider it
                // and will try to match with other nodes.
                // If the neighbour can match with another
                // node, then we'll match with this neighbour.
                // This will increase the number of matchings
                // by 1. If we couldn't match with any of the
                // neighbours, the number of matchings will
                // remain the same.
                matched_with[node] = neighbour;
                matched_with[neighbour] = node;
                return true;
            }
        }

        return false;
    }

public:

    MaximumBipartiteMatchingCalculator(int n, std::vector<Edge> edges)
        : edges(std::move(edges)), visited(n), matched_with(n), graph(n)
    {
        construct_graph();
    }

    std::vector<Edge> solve()
    {
        std::vector<Edge> result;

        std::fill(visited.begin(), visited.end(), false);
        std::vector<int> A; // set A
        for (Edge& edge : edges) {
            if (!visited[edge.a]) {
                A.push_back(edge.a);
                visited[edge.a] = true;
            }
        }

        std::fill(matched_with.begin(), matched_with.end(), -1);

        // This loop will iterate at most |V| times.
        // Each iteration takes O(|E|), so, the overall
        // runtime complexity is O(|E| * |V|).
        for (int a : A) {
            std::fill(visited.begin(), visited.end(), false);
            if (matched_with[a] == -1) {
                try_matching(a);
            }
        }

        for (int a : A) {
            if (matched_with[a] != -1) {
                result.push_back({a, matched_with[a]});
            }
        }

        return result;
    }
};

std::vector<Edge> get_sample_graph_1()
{
    std::vector<Edge> edges;

    edges.push_back({0, 7});
    edges.push_back({0, 8});
    edges.push_back({2, 6});
    edges.push_back({2, 9});
    edges.push_back({3, 8});
    edges.push_back({4, 8});
    edges.push_back({4, 9});
    edges.push_back({5, 11});

    return edges;
}

std::vector<Edge> get_sample_graph_2()
{
    std::vector<Edge> edges;

    edges.push_back({0, 4});
    edges.push_back({0, 5});
    edges.push_back({1, 5});
    edges.push_back({1, 6});
    edges.push_back({2, 6});
    edges.push_back({2, 7});
    edges.push_back({3, 4});

    return edges;
}

void test(const std::vector<Edge>& edges)
{
    int n = 0;
    for (const Edge& edge : edges) {
        n = std::max(n, std::max(edge.a, edge.b) + 1);
    }

    auto result = MaximumBipartiteMatchingCalculator(n, edges).solve();

    for (const Edge& edge : result) {
        std::cout << edge.a << " --- " << edge.b << std::endl;
    }

    std::cout << "Number of matches: " << result.size() << std::endl << std::endl;
}

int main()
{
    test(get_sample_graph_1());
    test(get_sample_graph_2());
}