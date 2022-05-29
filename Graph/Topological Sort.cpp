#include <iostream>
#include <vector>

typedef std::vector<std::vector<int>> Graph;

// start_index is used in case the 0th index is not used or
//  something, in this case, set it to 1.
std::vector<int> topological_sort(const Graph& graph, int start_index = 0)
{
    // If order.size() != graph.size(), there exists at least one cycle.
    std::vector<int> order;

    std::vector<int> in_degree(graph.size(), 0);
    for (const auto& node : graph)
        for (auto& neighbour : node)
            in_degree[neighbour]++;

    for (int i = start_index; i < in_degree.size(); i++)
        if (in_degree[i] == 0)
            order.push_back(i);

    for (int i = start_index; i < order.size(); i++)
        for (int neighbour : graph[order[i]])
            if (--in_degree[neighbour] == 0)
                order.push_back(neighbour);

    return order;
}

Graph graph_1()
{
    // Topological sort: 4 5 2 0 3 1

    Graph graph(6);

    graph[5].push_back(2);
    graph[5].push_back(0);
    graph[4].push_back(0);
    graph[4].push_back(1);
    graph[2].push_back(3);
    graph[3].push_back(1);

    return graph;
}

Graph cyclic_graph()
{
    Graph graph(5);

    graph[4].push_back(0);
    graph[0].push_back(1);
    graph[0].push_back(3);
    graph[1].push_back(2);
    graph[2].push_back(0);

    return graph;
}

void test(const Graph& graph)
{
    auto top_sort = topological_sort(graph);
    if (top_sort.size() != graph.size()) {
        std::cout << "The graph contains at least one cycle.";
    } else {
        std::cout << "Topological sort: ";
        for (int i : top_sort)
            std::cout << i << ' ';
    }
    std::cout << std::endl;
}

int main()
{
    test(graph_1());
    test(cyclic_graph());
}