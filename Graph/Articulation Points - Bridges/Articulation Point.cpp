#include <iostream>
#include <functional>
#include <vector>

// - Tarjan's algorithm
// - For undirected graphs

const int NO_PARENT = -1;
const int UNVISITED = -1;
typedef std::vector<std::vector<int>> Graph;

std::vector<int> get_APs(const Graph& graph)
{
    std::vector<int> result;

    int n = graph.size();
    int last_id = 0;
    std::vector<int> ids(n, UNVISITED);
    std::vector<int> low_link(n);

    std::function<void(int, int)> dfs = [&](int x, int parent)
    {
        ids[x] = low_link[x] = last_id++;

        // Also known as "Biconnected Components"
        int child_components_count = 0;
        bool is_articulation_point = false;

        for (int child : graph[x])
        {
            if (child == parent) {
                // THIS ASSUMES THAT THE PARENT IS CONNECTED
                // TO ITS CHILD WITH A SINGLE EDGE.
                continue;
            }

            if (ids[child] == UNVISITED) {
                dfs(child, x);
                low_link[x] = std::min(low_link[x], low_link[child]);

                child_components_count++;

                // In the case of bridges, if the child node can reach this
                //  node, the edge from x to child is not a bridge. This is
                //  because a bridge is never a part of a cycle, but articulation
                //  points can be. For this reason, if the child node can't
                //  reach any of the ancestors of the current node, then this
                //  node is an articulation point.
                // The root (has no parent) has the lowest low_link, thus this
                //  condition will always be satisfied. For this reason, we
                //  handle the case of the root separately.
                is_articulation_point |= (parent != NO_PARENT && low_link[child] >= ids[x]);
            } else {
                // here, we're using ids[child] instead of
                //  low_link[child] because a bridge is a single
                //  edge, and can't be composed of multiple edges.
                //  We want to know the node with the smallest
                //  "low link" that is connected to the node x
                //  directly by an edge.
                low_link[x] = std::min(low_link[x], ids[child]);
            }
        }

        // if this node is the root, and it connects multiple components that can't
        //  reach each other except through this root node, also known as "Biconnected
        //  Components", then removing this node will cause the graph to be separated
        //  into multiple components, which is the definition of the articulation point.
        is_articulation_point |= (parent == NO_PARENT && child_components_count > 1);

        if (is_articulation_point) {
            result.emplace_back(x);
        }
    };

    for (int i = 0; i < n; i++) {
        if (ids[i] == UNVISITED) {
            dfs(i, NO_PARENT);
        }
    }

    return result;
}

Graph get_sample_graph_1()
{
    Graph result(8);

    result[0].push_back(1);
    result[1].push_back(0);

    result[0].push_back(5);
    result[5].push_back(0);

    result[1].push_back(2);
    result[2].push_back(1);

    result[1].push_back(3);
    result[3].push_back(1);

    result[1].push_back(4);
    result[4].push_back(1);

    result[2].push_back(3);
    result[3].push_back(2);

    result[4].push_back(5);
    result[5].push_back(4);

    result[4].push_back(6);
    result[6].push_back(4);

    result[6].push_back(7);
    result[7].push_back(6);

    return result;
}

void test(const Graph& graph)
{
    auto articulation_points = get_APs(graph);
    std::cout << "Articulation Points: ";
    for (int node : articulation_points) {
        std::cout << node << ' ';
    }
    std::cout << std::endl << std::endl;
}

int main()
{
    test(get_sample_graph_1());
}