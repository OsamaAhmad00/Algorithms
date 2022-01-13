#include <iostream>
#include <vector>
#include <queue>
#include <stack>

const int MAX_VALUE = 1'000'000;

struct NegativeCycleException : public std::exception
{
    int first_affected_node;

    NegativeCycleException(int node = -1) :
        first_affected_node(node), std::exception() {}

    const char* what() const noexcept {
        return "The graph contains a negative cycle.";
    }
};

struct Edge
{
    int to;
    int weight;
};

typedef std::vector<std::vector<Edge>> Graph;

struct ShortestPathsInfo
{
    std::vector<int> shortest_distances;

    // Dijkstra's algorithm constructs a
    // shortest path tree with the root being
    // the source node. to construct any path
    // (or the entire tree). Thus, you only need
    // to know the parent of each node in the tree
    // to be able to construct the tree.
    std::vector<int> parent_of;

    ShortestPathsInfo(int n) {
        shortest_distances.resize(n, MAX_VALUE);
        parent_of.resize(n, -1);
    }
};

struct QueueFrame
{
    int node;
    int parent;
    int weight;

    bool operator>(const QueueFrame &other) const {
        return weight > other.weight;
    }
};

ShortestPathsInfo dijkstra(const Graph &graph, int source, int target)
{
    // This only works given that the weights of the edges are
    // non-negative.

    // Note: any sub-path of the shortest path is a shortest path
    // as well. Example: if shortest path from a -> e = abcde,
    // then the shortest path from b -> d is bcd and the shortest
    // path from c -> e is cde and the shortest path from a -> c
    // is abc. If for example, the shortest path from b -> d is
    // bcfd, then the path abcfde is shorter than a  abcde (the
    // shortest path from a -> e) which is not correct.

    // The intuition of this algorithm: given that there are no
    // negative edges, given all distances to the neighbours of
    // the node i, it's very obvious that the neighbour with the
    // minimum distance, call it j, that direct path from i to j
    // is the shortest path from i to j. any other path from i
    // to j will have a bigger distance. Given this fact along
    // with the note above, this implies this algorithm should
    // get the shortest path from i to j:
    //  1 - create a node closest = source
    //  2 - while closest is not j:
    //  3 -   closest = get the closest neighbour to i
    //  4 -   for each neighbour of closest, called n:
    //  5 -     if dist[i][closest] + dist[closest][n] < dist[i][n]:
    //  6 -       dist[i][n] = dist[i][closest] + dist[closest][n]
    //  7 -       parent[n] = closest
    //  8 - return dist[i][j]
    // This is essentially getting the closest node to i (shortest
    // path), relaxing its edges, kinda like removing the node and
    // replacing it with new edges that keeps paths lengths not
    // changed in the graph, then getting the next closest node,
    // relaxing it again... until you reach the desired node.

    // If you want to get the shortest path to all nodes, you can
    // get rid of the part where it matches the closest node to
    // the target, or pass the target as a node that doesn't exits.

    // This runs in O((E + V) * log(V)) because we're using priority
    // queues. If an adjacency matrix is used instead, the order
    // of this function will be O(V^2).
    // Using priority queues doesn't only make it faster, but also
    // allows for dynamically updating the graph (which the adjacency
    // matrix version doesn't allow). With that being said, the
    // adjacency matrix version is simpler.

    ShortestPathsInfo result(graph.size());
    auto &shortest_distances = result.shortest_distances;
    auto &parent_of = result.parent_of;

    // This is just a priority_queue that prioritizes smaller values.
    // you can also set the operator< in QueueFrame to "return weight > other.weight;"
    //  and use a normal priority queue, but this is cleaner.
    std::priority_queue<QueueFrame, std::vector<QueueFrame>, std::greater<QueueFrame>> queue;

    // Node with parent of -1 is the root of the shortest path tree.
    queue.push({.node=source, .parent=-1, .weight=0});

    // This is used to detect negative cycles. If we encounter a node
    // in the queue that is visited before AND we can reach it with
    // less weight, then this node is in a cycle and the results won't
    // be correct.
    std::vector<bool> is_visited(graph.size(), false);

    // At each iteration, one shortest path will be calculated.
    // Since the result is a tree, the maximum number of paths
    // from the root is n - 1. will iterate n - 1 times or until
    // the queue gets empty (in case not all nodes are connected).
    // i is not being updated here, will be updated when a shortest
    // path gets updated.
    for (int i = 0; i < graph.size() - 1 && !queue.empty();)
    {
        auto closest = queue.top();
        queue.pop();

        int node = closest.node;

        if (is_visited[node]) {
            if (closest.weight >= shortest_distances[node]) {
                continue;
            } else {
                throw NegativeCycleException(node);
            }
        }

        shortest_distances[node] = closest.weight;
        parent_of[node] = closest.parent;
        is_visited[node] = true;
        i++;

        for (auto& edge : graph[node]) {
            if (closest.weight + edge.weight < shortest_distances[edge.to]) {
                queue.push({
                    .node = edge.to,
                    .parent = node,
                    .weight = closest.weight + edge.weight,
                });
            }
        }
    }

    return result;
}

void add_child(Graph &graph, int parent, int child, int weight)
{
    graph[parent].push_back({child, weight});
    graph[child].push_back({parent, weight});
}

Graph construct_shortest_path_tree(const ShortestPathsInfo &info)
{
    int n = info.shortest_distances.size();
    Graph tree(n);

    for (int i = 0; i < n; i++) {

        int parent = info.parent_of[i];
        if (parent == -1) continue;

        int weight = info.shortest_distances[i] -
                     info.shortest_distances[parent];

        add_child(tree, parent, i, weight);
    }

    return tree;
}

Graph get_sample_graph()
{
    Graph graph(6);

    add_child(graph, 1, 2, 8);
    add_child(graph, 2, 3, 100);
    add_child(graph, 3, 5, 3);
    add_child(graph, 4, 1, 5);
    add_child(graph, 5, 4, 2);

    return graph;
}

Graph get_negative_cycle_graph()
{
    Graph graph(6);

    add_child(graph, 1, 2, 5);
    add_child(graph, 2, 3, 6);
    add_child(graph, 3, 4, 4);
    add_child(graph, 4, 5, 2);
    add_child(graph, 5, 3, -11);

    return graph;
}

void print_shortest_paths_info(const Graph& graph, int source)
{
    auto info = dijkstra(graph, source, -1);
    auto tree = construct_shortest_path_tree(info);

    for (int i = 0; i < info.shortest_distances.size(); i++)
    {
        if (i == source) {
            continue;
        }

        int dist = info.shortest_distances[i];

        if (dist == MAX_VALUE)
        {
            std::cout << "The nodes " << source << " and " << i
                      << " are not connected." << std::endl;
        }
        else
        {
            std::cout << "Shortest path from " << source << " to " << i
                      << " costs " << info.shortest_distances[i] << "." << std::endl;
        }
    }

    std::cout << std::endl;

    std::cout << "Shortest path tree:" << std::endl;

    for (int i = 0; i < tree.size(); i++)
    {
        std::cout << "Edges of node " << i << ":" << std::endl;

        for (int j = 0; j < tree[i].size(); j++)
        {
            std::cout << "\tTo " << tree[i][j].to << " with a weight of "
                << tree[i][j].weight << "." << std::endl;
        }
    }
}

void print_shortest_path_info(const Graph& graph, int source, int target)
{
    auto info = dijkstra(graph, source, target);

    struct StackFrame
    {
        int node;
        int weight;
    };

    std::stack<StackFrame> stack;

    int current = target;
    while (current != source) {
        int parent = info.parent_of[current];
        int weight = info.shortest_distances[current] -
                     info.shortest_distances[parent];
        stack.push({.node=current, .weight=weight});
        current = parent;
    }

    std::cout << "The total distance from " << source << " to " << target
              << " is " << info.shortest_distances[target] << "." << std::endl;

    std::cout << "Path: (" << source << ")";
    while (!stack.empty()) {
        auto frame = stack.top(); stack.pop();
        std::cout << " --" << frame.weight << "--> (" << frame.node << ")";
    }
    std::cout << std::endl;
}

void test_with_negative_cycle()
{
    std::cout << std::endl << std::endl;
    std::cout << "Negative cycle example: " << std::endl;

    auto negative_cycle_graph = get_negative_cycle_graph();
    try {
        print_shortest_paths_info(negative_cycle_graph, 1);
    } catch (NegativeCycleException &e) {
        std::cout << "\tNode " << e.first_affected_node
            << " is affected by a negative cycle." << std::endl;
    }
}

int main()
{
    auto graph = get_sample_graph();
    print_shortest_paths_info(graph, 1);

    test_with_negative_cycle();
}