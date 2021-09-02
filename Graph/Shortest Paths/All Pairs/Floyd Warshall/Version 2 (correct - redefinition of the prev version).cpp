#include <iostream>
#include <cstring>

const int n = 5;

const int MAX_VAL = 1'000'000;

int distances[n+1][n+1];
int answers[n+1][n+1][n+1];

int min_path(int i, int j, int k)
{
    if (k == 0) {
        // In case of not having any node in between the two,
        // just return the direct path's distance between the two.
        return distances[i][j];
    }

    int &result = answers[i][j][k];

    if (result != -1) {
        return result;
    }

    // if the node k is in the shortest path, we just know that
    // it's in between the two nodes (i and j), but we don't know
    // its position in the path. We can just get the distance from
    // i to k and from k to j then combine them. This way, we don't
    // have to know the actual position of k in the path. Theses
    // recursive calls will figure out the shortest path from i to k
    // (and from k to j) by trying to include other nodes (and exclude
    // some other) that are not included or excluded yet and will take care
    // of determining the nodes between i and k, and the nodes between k and j.
    int include_k_in_the_path = min_path(i, k, k-1) + min_path(k, j, k-1);
    int exclude_k_from_the_path = min_path(i, j, k-1);

    result = std::min(include_k_in_the_path, exclude_k_from_the_path);

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
    std::cout << min_path(2, 3, n) << std::endl;
}