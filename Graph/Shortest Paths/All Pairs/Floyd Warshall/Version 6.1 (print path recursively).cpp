#include <iostream>
#include <cstring>

const int n = 5;

const int MAX_VAL = 1'000'000;

int distances[n+1][n+1];
int answers[n+1][n+1];
int in_between[n+1][n+1];

int min_path(int i, int j)
{
    return answers[i][j];
}

void calc_min_paths()
{
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            answers[i][j] = distances[i][j];
            in_between[i][j] = -1;
        }
    }

    // k is a sequential parameter.
    // It MUST be the outer loop.
    for (int k = 1; k <= n; k++) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {

                int include_k_in_the_path = answers[i][k] + answers[k][j];
                int exclude_k_from_the_path = answers[i][j];

                if (include_k_in_the_path < exclude_k_from_the_path) {
                    answers[i][j] = include_k_in_the_path;
                    in_between[i][j] = k;
                } else {
                    answers[i][j] = exclude_k_from_the_path;
                }
            }
        }
    }
}

void print_path(int i, int j, bool first_call = true)
{
    if (first_call) {
        std::cout << i;
    }

    if (in_between[i][j] == -1) {
        std::cout << " -> " << j;
    } else {
        int k = in_between[i][j];
        print_path(i, k, false);
        print_path(k, j, false);
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
    std::cout << min_path(2, 3) << std::endl;
    print_path(2, 3); std::cout << std::endl;
}