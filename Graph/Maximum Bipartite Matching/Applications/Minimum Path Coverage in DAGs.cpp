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
        // Hopcroft-Karp

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

    std::vector<Edge> result = MaximumBipartiteMatchingCalculator(n * 2, edges).solve();

    std::vector<int> parent_of(n, -1);
    std::vector<int> child_of(n, -1);
    for (Edge& edge : result) {
        int to = edge.b - n;
        parent_of[to] = edge.a;
        child_of[edge.a] = to;
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

    graph[0].push_back(1);
    graph[0].push_back(2);
    graph[1].push_back(3);
    graph[2].push_back(3);

    return graph;
}

Graph get_sample_graph_2()
{
    Graph graph(6);

    graph[0].push_back(1);
    graph[0].push_back(4);
    graph[1].push_back(4);
    graph[2].push_back(3);
    graph[4].push_back(5);
    graph[5].push_back(2);
    graph[5].push_back(3);

    return graph;
}

Graph get_sample_graph_3()
{
    Graph graph(6);

    graph[0].push_back(1);
    graph[0].push_back(4);
    graph[1].push_back(4);
    graph[2].push_back(3);
    graph[5].push_back(2);
    graph[5].push_back(3);

    return graph;
}

Graph get_sample_graph_4()
{
    Graph graph(6);

    graph[0].push_back(4);
    graph[1].push_back(4);
    graph[4].push_back(5);
    graph[5].push_back(2);
    graph[5].push_back(3);

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