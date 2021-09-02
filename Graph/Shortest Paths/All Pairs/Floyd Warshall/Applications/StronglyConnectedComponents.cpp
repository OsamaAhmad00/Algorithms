#include <iostream>
#include <cstring>
#include <vector>

const int n = 6;

bool connected[n+1][n+1];
bool answers[n+1][n+1];

bool is_connected(int i, int j)
{
    return answers[i][j];
}

void calc_connected_nodes()
{
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            answers[i][j] = connected[i][j];
        }
    }

    for (int k = 1; k <= n; k++) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {

                bool include_k_in_the_path = answers[i][k] && answers[k][j];
                answers[i][j] |= include_k_in_the_path;

            }
        }
    }
}

std::pair<int, std::vector<int>> get_nodes_components()
{
    int components_count = 0;
    std::vector<int> components(n + 1, -1);

    for (int i = 1; i <= n; i++) {

        if (components[i] != -1) continue;

        components[i] = components_count;

        for (int j = i + 1; j <= n; j++) {
            if (is_connected(i, j)) {
                components[j] = components_count;
            }
        }

        components_count++;
    }

    return {components_count, components};
}

std::vector<std::vector<bool>> get_component_graph()
{
    // This can be done in O(E + V). this is just
    // another way.

    // Component graph is a DAG.

    auto nodes_components = get_nodes_components();

    int components_count = nodes_components.first;
    auto &components = nodes_components.second;

    std::vector<std::vector<bool>> component_graph (
        components_count,
        std::vector<bool>(components_count, false)
    );

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (is_connected(i, j)) {
                int i_component = components[i];
                int j_component = components[j];
                component_graph[i_component][j_component] = true;
            }
        }
    }

    return component_graph;
}

void set_connections()
{
    memset(connected, false, sizeof connected);

    connected[1][2] = true;
    connected[2][1] = true;

    connected[1][6] = true;
    connected[4][6] = true;

    connected[4][3] = true;
    connected[3][5] = true;
    connected[5][4] = true;
}

void print_component_graph()
{
    auto component_graph = get_component_graph();
    int count = component_graph.size();

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            std::cout << component_graph[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}

int main()
{
    set_connections();
    calc_connected_nodes();
    print_component_graph();
}