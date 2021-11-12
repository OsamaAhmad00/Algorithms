#include <vector>
#include <iostream>
#include <functional>

typedef std::vector<std::vector<int>> Graph;

enum class EdgeType
{
    // For undirected graphs, there are no cross and forward edges.

    TreeEdge    = 0, // normal edge
    BackEdge    = 1, // an edge from a node to one of its parents (results in a cycle)
    ForwardEdge = 2, // an edge from a node to a grand child
    CrossEdge   = 3, // an edge that points to a node already set to be a child for another node (in a different sub-tree)
    Unknown     = 4
};

struct EdgeDescription
{
    int from;
    int to;
    EdgeType type;
};

std::vector<EdgeDescription> classify_edges(const Graph& graph, int starting_node = 1)
{
    // the result depends on the starting node and the children order.

    struct NodeInfo
    {
        int enter = -1;
        bool left = false;
    };

    std::vector<NodeInfo> info(graph.size());
    std::vector<EdgeDescription> result;

    auto is_tree_edge = [&](int child)
    {
        return info[child].enter == -1;
    };

    auto is_back_edge = [&](int child)
    {
        return info[child].left == false;
    };

    auto is_forward_edge = [&](int parent, int child)
    {
        return info[child].left == true &&
            info[parent].enter < info[child].enter;
    };

    auto is_cross_edge = [&](int parent, int child)
    {
        return info[child].left == true &&
               info[parent].enter > info[child].enter;
    };

    std::function<int(int, int)> dfs = [&](int node, int timer) {

        info[node].enter = timer;

        for (int child : graph[node])
        {
            // if anything other than a tree edge, then it's
            // already visited and no need to visit again.

            if (is_tree_edge(child)) {

                result.push_back({node, child, EdgeType::TreeEdge});

                // setting the timer to be the
                // last timer in this sub-tree
                timer = dfs(child, timer + 1);

            } else if (is_back_edge(child)) {
                result.push_back({node, child, EdgeType::BackEdge});
            } else if (is_forward_edge(node, child)) {
                result.push_back({node, child, EdgeType::ForwardEdge});
            } else if (is_cross_edge(node, child)) {
                result.push_back({node, child, EdgeType::CrossEdge});
            } else {
                result.push_back({node, child, EdgeType::Unknown});
            }
        }

        info[node].left = true;

        return timer;
    };

    dfs(starting_node, 1);

    return result;
}

Graph get_sample_graph()
{
    Graph graph(9);

    graph[1].push_back(2);
    graph[1].push_back(5);
    graph[1].push_back(8);

    graph[2].push_back(3);

    graph[3].push_back(4);

    graph[4].push_back(2);

    graph[5].push_back(6);

    graph[6].push_back(3);
    graph[6].push_back(7);
    graph[6].push_back(8);

    return graph;
}

void print_classifications(const std::vector<EdgeDescription> &descriptions)
{
    static std::string type_to_str[] = { "Tree", "Back", "Forward", "Cross", "Unknown" };

    for (auto description : descriptions) {
        std::cout << "Edge from " << description.from << " to " << description.to
            << " is a " << type_to_str[(int)description.type] << " edge." << std::endl;
    }
}

int main()
{
    auto graph = get_sample_graph();
    auto classifications = classify_edges(graph);
    print_classifications(classifications);
}
