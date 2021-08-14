#include <iostream>
#include <cstring>

const int n = 5;

const int MAX_VAL = 1'000'000;

int distances[n+1][n+1];
int answers[2][n+1][n+1];

int min_path(int i, int j)
{
    return answers[n][i][j];
}

void calc_min_paths()
{
    // you can use one bool and negate the
    // variable at each iteration.
    int prev = 0;
    int current = 1;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            answers[prev][i][j] = distances[i][j];
        }
    }

    // k is a sequential parameter.
    // It MUST be the outer loop.
    for (int k = 1; k <= n; k++) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {

                int include_k_in_the_path = answers[prev][i][k] + answers[prev][k][j];
                int exclude_k_from_the_path = answers[prev][i][j];

                answers[current][i][j] = std::min(include_k_in_the_path, exclude_k_from_the_path);
            }
        }

        std::swap(prev, current);
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
}