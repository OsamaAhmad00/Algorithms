#include <functional>
#include <iostream>
#include <cstring>
#include <vector>

// NOTE this code is not practical.
//  Yet, can be used to explain the idea.

const int MAX_VAL = 1'000'000;

const int V = 6;

int distances[V][V];

// second argument is the edges_count
// which has a maximum of V - 1.
int answers[V][V-1];

struct ShortestPathInfo
{
    std::vector<int> parent_of;
    std::vector<int> minimum_distances;

    ShortestPathInfo(int n)
        : parent_of(n, -1), minimum_distances(n, MAX_VAL) {}
};

ShortestPathInfo not_bellman_ford(int source, int target = -1)
{
    // TODO Can this function be modified so that you can
    //  get the minimum distance from the source to every node
    //  instead of only getting the minimum distance from the
    //  source to the nodes along the path to the target?

    // The runtime complexity of this function is O(N^3),
    //  two arguments (O(V^2)) and one loop (O(V)).
    // The memory complexity of this function is O(V^2).

    // Intuition: Any shortest path will be a simple path
    // (does not repeat nodes) and will consist of at most V nodes
    // (involves every single node in the graph once). This means
    // that the maximum number of edges in a simple shortest path
    // is V - 1. The function here tries to construct every single
    // path from the source to the target with at most V - 1 edges
    // and minimizes the result value.

    // Since any sub-path of a shortest path is a shortest path
    // as well, if the path from the source s to the target t is
    // s -> a -> b -> c -> t, then this function will not only
    // compute the minimum distance from s to t, it'll also compute
    // the minimum distance from s to a, b, and c.

    // This function returns the minimum distances to each node
    // and the parent (the previous node) to each node in the
    // path from the source to the target. If you want to construct
    // the path, start from the target and go backwards. As of for
    // now, this function only guarantees the values of the nodes
    // along the path to be correct. You shouldn't use the minimum
    // distances a node that is not in the result path.

    ShortestPathInfo info(V);
    auto &parent_of = info.parent_of;
    auto &minimum_distances = info.minimum_distances;

    // We start from the target and keep calculating the minimum distance
    // backwards. This is done to have the distances calculated with respect
    // to the source and not with respect to the target. This results in the
    // result minimum distances array containing the distances from the source
    // to the other nodes and not from the target.
    std::function<int(int, int)> min_path = [&](int current, int edges_count) {

        if (current == source) {
            return 0;
        }

        // if edge count is zero and yet
        // didn't reach the desired node.
        if (edges_count == 0) {
            return MAX_VAL;
        }

        int &result = answers[current][edges_count];
        if (result != -1) {
            return result;
        }

        // Calculate the minimum distance from the current
        //  node to the source, but with edges_count - 1 edges.
        // You can get rid of this line if you can guarantee
        //  that the distance from a node to itself costs 0.
        //  in this case, you can remove this line, and remove
        //  the conditional which checks if i == current in the
        //  loop below.
        result = min_path(current, edges_count - 1);

        for (int i = 0; i < V; i++) {

            if (i == current) {
                continue;
            }

            // include the edge from i to current in the path and add
            // its distance to the distance of the rest of the path.
            int include = min_path(i, edges_count - 1) + distances[i][current];
            if (include < result) {
                result = include;
                parent_of[current] = i;
            }
        }

        // This will be assigned in each call with
        // different edge counts, but the last
        // call that will set this value is the
        // call with edge_count being equal to V - 1.
        minimum_distances[current] = result;

        return result;
    };

    min_path(target, V - 1);

    return info;
}

void set_distances()
{
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            distances[i][j] = MAX_VAL;
        }
    }

    distances[2][1] = 8;

    distances[1][4] = 5;
    distances[4][1] = 5;

    distances[2][3] = 100;

    distances[5][3] = 3;

    distances[4][5] = 2;
    distances[5][4] = 2;
}

void print_path_info(const ShortestPathInfo &info, int source, int target)
{
    std::vector<int> stack;

    int current = target;
    while (current != source) {
        if (current == -1) {
            std::cout << "The given info doesn't represent a valid path from "
                << source << " to " << target << " which indicates that the path doesn't exits." << std::endl;
            return;
        }
        stack.push_back(current);
        current = info.parent_of[current];
    }

    std::cout << source;
    for (int i = stack.size() - 1; i >= 0; i--) {
        std::cout << " -> " << stack[i];
    }
    std::cout << std::endl;

    for (int i = stack.size() - 1; i >= 0; i--) {
        int node = stack[i];
        std::cout << "Minimum distance from " << source << " to " << node
            << " is " << info.minimum_distances[node] << "." << std::endl;
    }
}

int main()
{
    memset(answers, -1, sizeof answers);
    set_distances();
    auto info = not_bellman_ford(2, 3);
    print_path_info(info, 2, 3);
}