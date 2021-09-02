#include <iostream>
#include <cstring>

const int n = 5;

const int MAX_VAL = 1'000'000;

int values[n+1][n+1];
int answers[n+1][n+1];

int mini_max_value(int i, int j)
{
    return answers[i][j];
}

void calc_mini_max()
{
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            answers[i][j] = values[i][j];
        }
    }

    for (int k = 1; k <= n; k++) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {

                // You can change the following two lines to get:
                //  - Mini-min: get a path with a minimum minimum value in the path.
                //  - Mini-max: get a path with a minimum maximum value in the path.
                //  - Maxi-min: get a path with a maximum minimum value in the path.
                //  - Maxi-max: get a path with a maximum maximum value in the path.

                // Don't forget to change the initial values if you're gonna maximize.

                // In "Mini-max", the first line here corresponds to the second half
                // ("max") and the second line corresponds to the first half (Mini).
                int maximum_value = std::max(answers[i][k], answers[k][j]);
                answers[i][j] = std::min(answers[i][j], maximum_value);

            }
        }
    }
}

void set_values()
{
    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            // since we're trying to minimize, put
            // large numbers for the invalid paths.
            values[i][j] = MAX_VAL;
        }
    }

    values[1][2] = 8;
    values[2][1] = 8;

    values[1][4] = 5;
    values[4][1] = 5;

    values[2][3] = 100;
    values[3][2] = 100;

    values[3][5] = 3;
    values[5][3] = 3;

    values[4][5] = 2;
    values[5][4] = 2;
}

int main()
{
    set_values();
    calc_mini_max();
    std::cout << mini_max_value(2, 3) << std::endl;
}