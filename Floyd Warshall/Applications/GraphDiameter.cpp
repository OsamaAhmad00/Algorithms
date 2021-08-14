#include <iostream>
#include <cstring>

const int n = 5;

const int MAX_VAL = 1'000'000;

int distances[n+1][n+1];
int answers[n+1][n+1];

void print_graph_diameter()
{
    // The shortest path in the graph between
    // two nodes with the most distance.

    int max_val = 0;
    int from = -1;
    int to = -1;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (distances[i][j] > max_val && distances[i][j] != MAX_VAL) {
                max_val = distances[i][j];
                from = i;
                to = j;
            }
        }
    }

    std::cout << "Graph diameter is from node " << from << " to node "
        << to << " with a distance of " << max_val << "." << std::endl;
}

void calc_min_paths()
{
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            answers[i][j] = distances[i][j];
        }
    }

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
    print_graph_diameter();
}