#include <iostream>
#include <cstring>

const int n = 5;

const int MAX_VAL = 1'000'000;

int distances[n+1][n+1];
int answers[n+1][n+1];

int min_path(int i, int j)
{
    // This recurrence is cyclic and not valid.

    int &result = answers[i][j];

    // If it wasn't for this conditional, this
    // function will recurse forever.
    if (result != -1) {
        return result;
    }

    result = distances[i][j];

    for (int k = 1; k <= n; k++) {
        if (k == i || k == j) continue;
        result = std::min(result, min_path(i, k) + min_path(k, j));
    }

    return result;
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
    memset(answers, -1, sizeof answers);
    set_distances();
    std::cout << min_path(1, 2) << std::endl;
}