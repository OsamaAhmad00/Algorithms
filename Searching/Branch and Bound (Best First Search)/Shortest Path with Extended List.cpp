#include <iostream>
#include <vector>
#include <queue>

struct Edge
{
    int to;
    int weight;
};

typedef std::vector<std::vector<Edge>> Graph;

class ShortestPath
{
    struct QueueNode
    {
        int node;
        int weight;

        bool operator>(const QueueNode& rhs) const {
            return weight > rhs.weight;
        }
    };

    const Graph& graph;

public:

    ShortestPath(const Graph& graph) : graph(graph) {}

    int compute_weight(int from, int to)
    {
        /*
         * https://www.youtube.com/watch?v=gGQ-vAmdAOI
         *
         * This function works well even with disconnected nodes.
         * The idea here is simple, get the node with the smallest
         *  weight so far, and extend it. Keep doing that until you
         *  reach the desired target node.
         * The name "Branch and Bound" comes from the fact that we
         *  take each node, and "branch" through all of its neighbours,
         *  and "bound" a current branch if its weight is bigger than
         *  the smallest weight so far, in other words, the branch is
         *  discarded until it's the branch with the smallest weight so far.
         */

        std::vector<bool> is_extended(graph.size(), false);
        std::priority_queue<QueueNode, std::vector<QueueNode>, std::greater<>> queue;

        queue.push({from, 0});

        while (!queue.empty())
        {
            QueueNode current = queue.top();
            queue.pop();

            // If this node has been extended before,
            //  then we've reached it with a smaller
            //  weight, no need to consider it again.
            if (is_extended[current.node])
                continue;
            is_extended[current.node] = true;

            if (current.node == to) {
                return current.weight;
            }

            for (const Edge& e : graph[current.node])
            {
                /*
                * Can we check if e.to is the target node?
                *  You can, but you still have to wait until
                *  all branches' weights exceed the current
                *  minimum weight. The reason is that other
                *  branches might be able to reach the target
                *  node with a smaller weight.
                *  To avoid this checks, even if e.to is the
                *  target node, we push it in the priority
                *  queue. If the top of the priority queue
                *  is the target node, this means that every
                *  other branch has a weight bigger than the
                *  weight to the target node, in which case
                *  we can return the weight.
                */
                queue.push({e.to, current.weight + e.weight});
            }
        }

        return -1;
    }
};

void add_child(Graph &graph, int parent, int child, int weight)
{
    graph[parent].push_back({child, weight});
    graph[child].push_back({parent, weight});
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

void test(const Graph& graph, int source)
{
    for (int to = 0; to < graph.size(); to++)
    {
        std::cout << "Weight of the shortest path from node " << source;
        std::cout << " to node " << to << " = ";
        std::cout << ShortestPath{graph}.compute_weight(source, to);
        std::cout << "." << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test(get_sample_graph(), 1);
}