#include <iostream>
#include <cstring>

const int n = 5;

int distances[n+1][n+1];
int answers[n+1][n+1];

const int MAX_VAL = 1'000'000;

bool is_node_in_a_negative_cycle(int x)
{
    return distances[x][x] < 0;
}

bool there_exists_a_path(int i, int j)
{
    return distances[i][j] != MAX_VAL;
}

bool does_graph_contain_negative_cycles()
{
    for (int x = 1; x <= n; x++) {
        if (is_node_in_a_negative_cycle(x)) {
            return true;
        }
    }

    return false;
}

bool is_path_affected_by_negative_cycle(int i, int j)
{
    for (int k = 1; k <= n; k++) {
        bool is_affected = is_node_in_a_negative_cycle(k) &&
                           there_exists_a_path(i, k) &&
                           there_exists_a_path(k, j);
        if (is_affected) {
            return true;
        }
    }

    return false;
}

void calc_min_paths()
{
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            answers[i][j] = distances[i][j];
        }
    }

    // k is a sequential parameter.
    // It MUST be the outer loop.
    for (int k = 1; k <= n; k++) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {

                int include_k_in_the_path = answers[i][k] + answers[k][j];
                int exclude_k_from_the_path = answers[i][j];

                answers[i][j] = std::min(include_k_in_the_path, exclude_k_from_the_path);
            }
        }
    }
}

void set_distances()
{
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            distances[i][j] = MAX_VAL;
        }
    }

    distances[1][2] = 8;
    distances[2][1] = 8;

    distances[1][4] = 5;
    distances[4][1] = 5;

    distances[2][3] = 100;
    distances[3][2] = 100;

    distances[3][5] = 3;
    distances[5][3] = 3;

    distances[4][5] = 2;
    distances[5][4] = 2;
}

int main()
{
    set_distances();
    calc_min_paths();
    std::cout << is_path_affected_by_a_negative_cycle(2, 3) << std::endl;
    std::cout << does_graph_contain_negative_cycles() << std::endl;
}