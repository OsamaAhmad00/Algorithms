#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

const int UNMATCHED = -1;
const int NO_PARENT = -1;
const int SOURCE = -2;

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
    std::vector<Edge> edges;
    std::vector<int> matched_with;
    std::vector<int> parent_of;
    std::vector<int> sources;
    Graph graph;

    void construct_graph()
    {
        for (Edge &edge : edges) {
            graph[edge.a].push_back(edge.b);
            graph[edge.b].push_back(edge.a);
        }
    }

    void augment_path(int child)
    {
        bool match = true;
        int parent = parent_of[child];
        while (parent != SOURCE) {
            // This loop can be optimized to iterate half of
            // the iterations, but makes the code less readable.
            if (match) {
                matched_with[child] = parent;
                matched_with[parent] = child;
            }
            child = parent;
            parent = parent_of[parent];
            match = !match;
        }
    }

    bool augment_paths()
    {
        std::fill(parent_of.begin(), parent_of.end(), NO_PARENT);

        std::queue<int> queue;
        for (int source : sources) {
            if (matched_with[source] == UNMATCHED) {
                queue.push(source);
                parent_of[source] = SOURCE;
            }
        }

        bool found_augmenting_path = false;

        while (!queue.empty())
        {
            int node = queue.front();
            queue.pop();

            for (int child : graph[node])
            {
                if (parent_of[child] != NO_PARENT) continue;
                parent_of[child] = node;

                if (matched_with[child] == UNMATCHED) {
                    augment_path(child);
                    found_augmenting_path = true;
                    break;
                } else {
                    queue.push(child);
                }
            }
        }

        return found_augmenting_path;
    }

public:

    MaximumBipartiteMatchingCalculator(int n, std::vector<Edge> edges)
            : edges(std::move(edges)), matched_with(n), parent_of(n), graph(n)
    {
        construct_graph();
    }

    std::vector<Edge> solve()
    {
        //  - Free Node or Vertex: Given a matching M, a node that is not part of
        //    matching is called free node. Initially all vertices as free
        //  - Matching and Not-Matching edges: Given a matching M, edges that are part
        //    of matching are called Matching edges and edges that are not part of M
        //    (or connect free nodes) are called Not-Matching edges.
        //  - An alternating path is a path that alternates between matching and
        //    not-matching edges.
        // Given an alternating path that starts and ends at a free node means that
        // the length of the path is odd, and that the number of the not-matching
        // edges along the path = the number of matching edges + 1.
        // Changing the matching edges in the path to not-matching edges, and changing
        // the not-matching edges to matching edges will keep all nodes that were matched
        // before remain matched, in addition to matching one extra free node. This is because
        // now, the number of the matching edges = the number of not-matching edges + 1.
        // Thus, if an alternating path that starts and ends at a free node is found, we can
        // increase the number of matched nodes by 1. Such path is called an "Augmenting Path".

        // We'll find all possible augmenting paths using BFS. This results in finding the
        // shortest augmenting paths first. In subsequent iterations, the shortest paths
        // found must be longer.
        // Since the edges alternate, the next paths will be at least 2 edges longer.
        // After sqrt(|V|) iterations, the minimum path length would therefore be 2 * sqrt(|V|).
        // Since an augmenting path should be vertex-disjoint, and there are |V| nodes in the
        // graph, there can only be |V| / (2 * sqrt(|V|)) paths left.
        // Since each iteration adds at least one new path (else, the loop will terminate), the
        // maximum number of iterations left = |V| / (2 * sqrt(|V|)).
        // Therefore, after sqrt(|V|) iterations, at most |V| / (2 * sqrt(|V|)) more iterations
        // will be done, thus, the loop will iterate at most 3/2 * sqrt(|V|) times.
        // Since each iteration performs a BFS, and each edge will be visited at most once,
        // the runtime complexity of this algorithm = O(E * sqrt(|V|)).

        std::vector<Edge> result;

        sources.clear();
        std::vector<bool> visited(graph.size(), false);
        for (const Edge& edge : edges) {
            if (!visited[edge.a]) {
                sources.push_back(edge.a);
                visited[edge.a] = true;
            }
        }

        std::fill(matched_with.begin(), matched_with.end(), UNMATCHED);

        while (augment_paths());

        for (int source : sources) {
            if (matched_with[source] != UNMATCHED) {
                result.push_back({source, matched_with[source]});
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