#include <iostream>
#include <vector>
#include <queue>
#include <stack>

// NOTE: THIS CODE IS WRONG AND WILL GIVE WRONG RESULTS IN SOME CASES.
//  THIS CODE IS ONLY TO FOR A BETTER UNDERSTANDING OF THE ALGORITHM.

const int MAX_VAL = 1'000'000;

struct Edge
{
    int from;
    int to;
    int weight;
};

typedef std::vector<std::vector<Edge>> Graph;

struct ShortestPathInfo
{
    std::vector<int> path_len;
    std::vector<int> parent_of;
    std::vector<int> minimum_distances;

    ShortestPathInfo(int n) : path_len(n, -1),
        parent_of(n, -1), minimum_distances(n, MAX_VAL) {}
};

ShortestPathInfo bellman_ford(const Graph &graph, int source)
{
    // Intuition:
    //
    // The reason for relaxing all edges for V - 1 times is that the
    // order of the edges is not known. Each pass in the V - 1 passes
    // only guarantees one relaxation (might relax more, but not
    // guaranteed). The first relaxation ever will be a relaxation of
    // for an edge that goes out of the source node.
    //
    // Given a single path in an example graph a -> b -> c -> d -> e,
    // in the first pass in the V - 1 passes, if by luck, the first
    // edge to be processed is the edge from a -> b, and the second
    // edge to be processed is the edge from b -> c, then c -> d
    // then d -> e, in this case, the shortest paths from a to b, c,
    // d, and e are all calculated in the first pass only. But this
    // will happen only if you're lucky. Given an edge list, you can't
    // guarantee that you'll process nodes in order. But if you process
    // them by order, not only that you'll be able to determine the
    // shortest paths, you'll also be able to determine the number of
    // edges along the path (path length).
    //
    // This function uses BFS to process the nodes in order starting
    // from the source, thus does it in a single pass, resulting in
    // it running in O(E + V).

    int V = graph.size();
    ShortestPathInfo info(V);
    auto &path_len = info.path_len;
    auto &parent_of = info.parent_of;
    auto &minimum_distances = info.minimum_distances;

    minimum_distances[source] = 0;

    std::vector<std::vector<int>> visited_counts(
            V, std::vector<int>(V, 0)
    );

    std::queue<Edge> queue;
    queue.push({source, source, 0});

    int depth = 0;

    while (!queue.empty())
    {
        int size = queue.size();

        while (size--)
        {
            auto edge = queue.front();
            queue.pop();

            int current = edge.to;
            int parent = edge.from;
            int &visited_count = visited_counts[edge.from][edge.to];

            int current_weight = minimum_distances[current];
            int new_weight = minimum_distances[parent] + edge.weight;

            if (new_weight < current_weight)
            {
                if (visited_count)
                {
                    // If the node is visited from the same
                    // parent before and the new weight is
                    // less than the current weight, then
                    // there exists a cycle.
                    minimum_distances[current] = -MAX_VAL;
                    parent_of[current] = -1;
                    path_len[current] = -1;
                }
                else
                {
                    minimum_distances[current] = new_weight;
                    parent_of[current] = parent;
                    path_len[current] = depth;
                }
            }

            // We only need to process an edge for 2 times.
            // once to process its distance to the current
            // node through the current edge, and a second
            // time to detect if we've managed to reach
            // again through the same parent with a shorter
            // distance.
            if (visited_count < 2) {
                for (auto &e : graph[current]) {
                    queue.push(e);
                }
                visited_count++;
            }

        }

        depth++;
    }

    return info;

    // ===========================================
    // || The reason why this function is wrong ||
    // ===========================================
    // Try executing this function with the graph
    // below that results in wrong results. The
    // node 3 isn't affected by a cycle, yet this
    // function identified it as being affected.
    // this is because the parent of the node 3
    // got updated for 3 times through 3 different
    // parents, resulting in the value of node 3
    // being updated for 3 times (and this function
    // assumes that if the value of a node is
    // updated through the same parent for more than
    // two times, then there exists a cycle. This
    // is not true. You should allow its values to
    // be updated for V - 1 times (is it V - 1?)
    // and not only for two times. If you've done
    // so, the order of this function will be O(N^2)
    // which is worse (and more complex) than the
    // original algorithm.
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
        int path_len = info.path_len[i];

        if (distance == MAX_VAL) {
            std::cout << "There is no path from "
                      << source << " to " << i << "." << std::endl;
        } else if (distance == -MAX_VAL) {
            std::cout << "The path from " << source << " to " << i
                      << " is affected by a negative cycle, thus, there is no minimum distance." << std::endl;
        } else {
            std::cout << "Minimum distance from " << source
                      << " to " << i << " is " << distance << " with a length of " << path_len << "." << std::endl;
        }
    }
}

Graph get_sample_graph()
{
    Graph graph {7};

    graph[1].push_back({1, 2, 5});

    graph[2].push_back({2, 1, -10});
    graph[2].push_back({2, 6, 2});

    graph[4].push_back({4, 5, 2});

    graph[5].push_back({5, 2, 3});

    graph[5].push_back({5, 3, 4});

    return graph;
}

Graph get_graph_that_gives_wrong_results()
{
    Graph graph {9};

    graph[1].push_back({1, 2, 100});
    graph[1].push_back({1, 7, 10});
    graph[1].push_back({1, 4, 1});

    graph[2].push_back({2, 3, 5});

    graph[4].push_back({4, 5, 2});

    graph[5].push_back({5, 6, 3});

    graph[6].push_back({6, 2, 4});

    graph[7].push_back({7, 8, 20});

    graph[8].push_back({8, 2, 30});

    return graph;
}

int main()
{
    {
        auto graph = get_sample_graph();
        auto info = bellman_ford(graph, 4);
        print_path_info(info, 4, 3);
    }

    std::cout << std::endl << std::endl;

    {
        auto graph = get_graph_that_gives_wrong_results();
        auto info = bellman_ford(graph, 1);
        print_path_info(info, 1, 3);
    }
}