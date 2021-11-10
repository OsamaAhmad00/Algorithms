#include <iostream>
#include <functional>
#include <vector>

// Hierholzer's algorithm

/*
 * How to know that there is an Eulerian circuit:
 *  if the graph is undirected, every node should have an even degree.
 *  if the graph is directed, every node should have equal indegree and outdegree.
 *
 * How to know that there is only an Eulerian path (no circuit):
 *  if the graph is undirected, exactly 2 nodes should have an odd degree.
 *  if the graph is directed, at most one node will have indegree - outdegree = 1
 *   and at most one node will have outdegree - indegree = 1.
 *
 * Note: any Eulerian circuit is an Eulerian path. If you have an algorithm for
 *  finding an Eulerian path, and your graph contains a circuit, it will find it.
 *
 * Note: the number of nodes with odd degree can't be odd.
 */

enum class EulerianPathType
{
    None=0, Path, Circuit
};

typedef std::vector<std::vector<int>> Graph;

struct UnionFind
{
    int components;
    std::vector<int> parent;

    UnionFind(int size) : parent(size, -1), components(size) {}

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

bool is_disconnected(const Graph& graph)
{
    int n = graph.size();

    if (n == 1) return false;

    UnionFind unionFind(n);

    for (int i = 0; i < n; i++) {
        for (int neighbour : graph[i]) {
            unionFind.connect(i, neighbour);
        }
    }

    // if a node is disconnected, but has no edges connected (including
    // self-loops) to it, then the disconnected node doesn't affect.
    int nodes_with_no_edges = 0;
    for (int i = 0; i < n; i++) {
        if (unionFind.component_size(i) == 1) {
            // self-loops.
            if (!graph[i].empty()) return true;
            nodes_with_no_edges++;
        }
    }

    return nodes_with_no_edges != unionFind.components - 1;
}

std::vector<int> get_indegree(const Graph& graph)
{
    std::vector<int> indegree(graph.size(), 0);

    for (const auto &node : graph) {
        for (int neighbour : node) {
            indegree[neighbour]++;
        }
    }

    return indegree;
}

EulerianPathType compute_path_type(const Graph& graph)
{
    int n = graph.size();

    auto indegree = get_indegree(graph);

    // {in-out = -1, in-out = 0, in-out = 1}
    int count[3] = {0, 0, 0};
    for (int i = 0; i < n; i++) {
        int diff = indegree[i] - graph[i].size();
        if (abs(diff) > 1) return EulerianPathType::None;
        count[diff + 1]++;
    }

    if (count[1] == n) return EulerianPathType::Circuit;
    if (count[0] <= 1 && count[2] <= 1) return EulerianPathType::Path;
    return EulerianPathType::None;
}

EulerianPathType check_for_path_type(const Graph& graph)
{
    if (is_disconnected(graph)) {
        return EulerianPathType::None;
    }
    return compute_path_type(graph);
}

std::vector<int> get_eulerian_path(const Graph& graph)
{
    if (compute_path_type(graph) == EulerianPathType::None) {
        return {};
    }

    int n = graph.size();
    std::vector<int> result;

    int edge_count = 0;
    for (int i = 0; i < n; i++) {
        edge_count += graph[i].size();
    }

    // TODO this is being computed twice.
    auto indegree = get_indegree(graph);

    int start_node = -1;
    for (int i = 0; i < n; i++) {
        if (!graph[i].empty()) {
            if ((start_node == -1 && !graph[i].empty()) || indegree[i] - graph[i].size() == 1) {
                start_node = i;
            }
        }
    }

    std::vector<int> last_index(n, 0);
    std::function<void(int)> find_path = [&](int x) {
        while (last_index[x] < graph[x].size()) {
            int neighbour = graph[x][last_index[x]++];
            find_path(neighbour);
        }
        result.push_back(x);
    };

    find_path(start_node);

    if (result.size() != edge_count + 1) {
        // disconnected
        return {};
    }

    return result;
}

Graph get_graph_with_circuit_1()
{
    // node 0 is disconnected, but
    // has no edges connected to it.
    Graph result(7);

    result[2].push_back(1);
    result[1].push_back(5);
    result[3].push_back(2);
    result[5].push_back(2);
    result[2].push_back(6);
    result[4].push_back(3);
    result[3].push_back(5);
    result[6].push_back(3);
    result[6].push_back(4);
    result[5].push_back(6);

    return result;
}

Graph get_graph_with_circuit_2()
{
    Graph result(5);

    result[1].push_back(2);
    result[1].push_back(3);
    result[1].push_back(4);
    result[2].push_back(1);
    result[3].push_back(1);
    result[4].push_back(1);

    return result;
}

Graph get_graph_with_path()
{
    auto result = get_graph_with_circuit_1();
    result[5].pop_back();
    return result;
}

Graph get_graph_with_none_1()
{
    // disconnected.
    Graph result(5);

    result[1].push_back(2);
    result[2].push_back(1);

    result[3].push_back(4);
    result[4].push_back(3);

    return result;
}

Graph get_graph_with_none_2()
{
    auto result = get_graph_with_none_1();

    result[1].push_back(0);
    result[1].push_back(4);

    return result;
}

void test(const Graph& graph)
{
    const char* types[] = {"None", "Path", "Circuit"};
    int type = (int)check_for_path_type(graph);
    std::cout << types[type] << std::endl;
    std::cout << "Nodes: ";
    for (int node : get_eulerian_path(graph)) {
        std::cout << node << ' ';
    }
    std::cout << std::endl << std::endl;
}

int main()
{
    test(get_graph_with_circuit_1());
    test(get_graph_with_circuit_2());
    test(get_graph_with_path());
    test(get_graph_with_none_1());
    test(get_graph_with_none_2());
}