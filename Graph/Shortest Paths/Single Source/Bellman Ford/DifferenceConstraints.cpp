#include <functional>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>

const int MAX_VAL = 1'000'000;

struct Edge
{
    int from;
    int to;
    int weight;
};

struct Graph
{
    int nodes_count = 0;
    std::vector<Edge> edges;

    void add_edge(int from, int to, int weight) {
        edges.push_back({from, to, weight});
    }
};

std::vector<int> bellman_ford(const Graph &graph, int source)
{
    int V = graph.nodes_count;
    std::vector<int> minimum_distances(V);

    minimum_distances[source] = 0;

    for (int x = 1; x <= 2; x++) {
        for (int i = 0; i < V - 1; i++)
        {
            bool relaxed = false;

            for (auto &edge : graph.edges)
            {
                int old_weight = minimum_distances[edge.to];
                int new_weight = minimum_distances[edge.from] + edge.weight;

                if (new_weight < old_weight)
                {
                    minimum_distances[edge.to] =
                            (x == 1) ? new_weight : -MAX_VAL;

                    relaxed = true;
                }
            }

            if (!relaxed) {
                break;
            }
        }
    }

    return minimum_distances;
}

class DifferenceConstraintsSolver
{
    Graph graph;
    int last_id = 1;
    std::map<std::string, int> map;

    int get_id(const std::string &x)
    {
        auto it = map.find(x);

        if (it == map.end())
        {
            map[x] = last_id;
            graph.add_edge(0, last_id, 0);
            graph.nodes_count++;

            return last_id++;
        }

        return it->second;
    }

public:

    void add_constraint(const std::string &x1, const std::string &x2, int diff)
    {
        int id1 = get_id(x1);
        int id2 = get_id(x2);
        graph.add_edge(id2, id1, diff);
    }

    std::map<std::string, int> get_solution() const
    {
        auto solution = bellman_ford(graph, 0);

        std::map<std::string, int> result;

        for (auto &x : map) {
            result[x.first] = solution[x.second];
        }

        return result;
    }
};

DifferenceConstraintsSolver get_solvable_system()
{
    DifferenceConstraintsSolver solver;

    solver.add_constraint("x1", "x2", 3);
    solver.add_constraint("x2", "x3", -5);
    solver.add_constraint("x1", "x3", 2);

    return solver;
}

DifferenceConstraintsSolver get_unsolvable_system()
{
    DifferenceConstraintsSolver solver;

    solver.add_constraint("x1", "x2", -2);
    solver.add_constraint("x2", "x3", -5);
    solver.add_constraint("x3", "x1", -3);

    return solver;
}

void print_solution(const DifferenceConstraintsSolver &solver)
{
    auto result = solver.get_solution();

    for (auto &solution : result) {
        if (solution.second == -MAX_VAL) {
            std::cout << "The system is not solvable." << std::endl;
            return;
        }
    }

    for (auto &solution : result) {
        std::cout << solution.first << ": " << solution.second << std::endl;
    }
}

int main()
{
    auto solvable = get_solvable_system();
    print_solution(solvable);

    std::cout << std::endl;

    auto unsolvable = get_unsolvable_system();
    print_solution(unsolvable);
}