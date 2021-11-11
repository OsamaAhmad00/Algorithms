#include <iostream>
#include <functional>
#include <vector>

// - Tarjan's algorithm
// - For undirected graphs

const int UNVISITED = -1;
typedef std::vector<std::vector<int>> Graph;

std::vector<std::pair<int, int>> get_bridges(const Graph& graph)
{
    std::vector<std::pair<int, int>> result;

    int n = graph.size();
    int last_id = 0;
    std::vector<int> ids(n, UNVISITED);
    std::vector<int> low_link(n);

    std::function<void(int, int)> dfs = [&](int x, int parent)
    {
        ids[x] = low_link[x] = last_id++;

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

                // ids[child] is greater than ids[x]. If low_link[child]
                //  equals ids[child], this means that this child has
                //  no other way to get to its ancestors except from
                //  this edge, which is the definition of a bridge.
                if (low_link[child] == ids[child]) {
                    result.emplace_back(x, child);
                }
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
    };

    for (int i = 0; i < n; i++) {
        if (ids[i] == UNVISITED) {
            dfs(i, -1);
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
    auto bridges = get_bridges(graph);
    for (auto& bridge : bridges) {
        std::cout << "There exists a bridge from "
            << bridge.first << " to " << bridge.second << "." << std::endl;
    }
}

int main()
{
    test(get_sample_graph_1());
}