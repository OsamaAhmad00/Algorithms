#include <iostream>
#include <cstring>

const int n = 5;

const int MAX_VAL = 1'000'000;

int distances[n+1][n+1];
int answers[n+1][n+1];

int longest_path(int i, int j)
{
    return answers[i][j];
}

void calc_longest_paths()
{
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            answers[i][j] = distances[i][j];
        }
    }

    for (int k = 1; k <= n; k++) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                answers[i][j] = std::max(answers[i][j], answers[i][k] + answers[k][j]);
            }
        }
    }
}

void set_distances()
{
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            // since we're trying to maximize, put
            // small numbers for the invalid paths.
            distances[i][j] = -MAX_VAL;
        }
    }

    distances[1][2] = 8;
    distances[1][4] = 5;
    distances[1][5] = 50;
    distances[2][3] = 100;
    distances[3][5] = 3;
}

int main()
{
    set_distances();
    calc_longest_paths();
    std::cout << longest_path(1, 5) << std::endl;
}