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

bool is_disconnected(const Graph& graph)
{
    int n = graph.size();
    std::vector<bool> visited(n, false);

    std::function<void(int)> visit = [&](int x) {
        if (visited[x]) return;
        visited[x] = true;
        for (int neighbour : graph[x]) {
            visit(neighbour);
        }
    };

    // starting with node 1 to handle both cases where
    //  nodes start from 0 and when they start from 1.
    visit(1);

    // if a node is disconnected, but has no edges connected
    // to it, then the disconnected node doesn't affect.
    for (int i = 0; i < n; i++) {
        if (!visited[i] && !graph[i].empty()) {
            return true;
        }
    }

    return false;
}

EulerianPathType compute_path_type(const Graph& graph)
{
    // odd degree count and
    // even degree count.
    int count[2] = {0, 0};

    for (int i = 0; i < graph.size(); i++) {
        count[graph[i].size() % 2]++;
    }

    if (count[0] == graph.size()) return EulerianPathType::Circuit;
    if (count[1] == 2) return EulerianPathType::Path;
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

    // adjMatrix[i][j] = number of edges from i to j (or j to i).
    std::vector<std::vector<int>> adjMatrix(n, std::vector<int>(n, 0));

    int edge_count = 0;

    for (int i = 0; i < n; i++) {
        edge_count += graph[i].size();
        for (int neighbour : graph[i]) {
            // adjMatrix[neighbour][i] will
            // be incremented when i = neighbour.
            adjMatrix[i][neighbour]++;
        }
    }

    // every edge will be counted
    // twice, once from each end.
    edge_count /= 2;

    int start_node = -1;
    for (int i = 0; i < n; i++) {
        if (!graph[i].empty()) {
            if ((start_node == -1 && !graph[i].empty()) || graph[i].size() % 2 == 1) {
                start_node = i;
            }
        }
    }

    std::vector<int> last_index(n, 0);
    std::function<void(int)> find_path = [&](int x) {
        while (last_index[x] < graph[x].size()) {
            int neighbour = graph[x][last_index[x]++];
            if (adjMatrix[x][neighbour]) {
                adjMatrix[x][neighbour]--;
                adjMatrix[neighbour][x]--;
                find_path(neighbour);
            }
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

    result[1].push_back(2);
    result[2].push_back(1);

    result[1].push_back(5);
    result[5].push_back(1);

    result[2].push_back(3);
    result[3].push_back(2);

    result[2].push_back(5);
    result[5].push_back(2);

    result[2].push_back(6);
    result[6].push_back(2);

    result[3].push_back(4);
    result[4].push_back(3);

    result[3].push_back(5);
    result[5].push_back(3);

    result[3].push_back(6);
    result[6].push_back(3);

    result[4].push_back(6);
    result[6].push_back(4);

    result[5].push_back(6);
    result[6].push_back(5);

    return result;
}

Graph get_graph_with_circuit_2()
{
    Graph result(5);

    result[1].push_back(2);
    result[1].push_back(2);
    result[1].push_back(3);
    result[1].push_back(3);
    result[1].push_back(4);
    result[1].push_back(4);

    result[2].push_back(1);
    result[2].push_back(1);

    result[3].push_back(1);
    result[3].push_back(1);

    result[4].push_back(1);
    result[4].push_back(1);

    return result;
}

Graph get_graph_with_path()
{
    auto result = get_graph_with_circuit_1();
    result[5].pop_back();
    result[6].pop_back();
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

    result[0].push_back(1);
    result[1].push_back(0);

    result[1].push_back(4);
    result[4].push_back(1);

    return result;
}

void test(const Graph& graph)
{
    const char* types[] = {"None", "Path", "Circuit"};
    int type = (int) check_for_path_type(graph);
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