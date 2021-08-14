#include <iostream>
#include <cstring>

const int n = 5;

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

void set_connections()
{
    memset(connected, false, sizeof connected);

    connected[1][2] = true;
    connected[2][1] = true;

    connected[1][4] = true;
    connected[4][1] = true;

    connected[3][5] = true;
    connected[5][3] = true;

    connected[4][5] = true;
    connected[5][4] = true;
}

int main()
{
    set_connections();
    calc_connected_nodes();
    std::cout << is_connected(2, 3) << std::endl;
}