#include <iostream>
#include <cstring>

const int n = 5;

int count[n+1][n+1];
int answers[n+1][n+1];

int paths_count(int i, int j)
{
    return answers[i][j];
}

void calc_paths_count()
{
    // You might think that this shouldn't work
    // because this is not like the minimization
    // problem where you can mix the values from
    // this iteration and the previous iteration.
    // For example, when calculating the count from
    // node 1 to node 2 with k = 5, you'll use the
    // old count[1][5]. After that you'll calculate
    // the count from 1 to 5 with k = 5 (which updates
    // count[1][5]). Then when calculating from 1 to 7
    //  with k = 5 again, you'll use the new count[1][5].

    // There are two cases:
    //
    //  - The graph is a DAG:
    //      In this case, no cycles exist. thus count[i][i] = 0.
    //      Thus, in the previous case, when updating count[1][5],
    //      count[1][5] += count[1][5] * count[5][5] which equals
    //      count[1][5] += count[1][5] * 0 which equals the old
    //      count[1][5]. Note that this is the only case that
    //      count[1][5] will be updated. Other than that, it'll
    //      just be used. The problem described above only happens
    //      when trying accessing count[i][k] or count[k][j]. These
    //      are the only two nodes that will be used and updated
    //      in the same iteration. But luckily, count[i][k] and
    //      count[k][j] in this iteration will not be affected
    //      because their updates will be multiplied with count[k][k]
    //      which equals 0.
    //
    //  - The graph is NOT a DAG:
    //      It'll still work for this case. It just won't work for the
    //      path from i to j where i can reach j and j can reach i (there
    //      exists a cycle). The concept of paths count doesn't have a
    //      meaning here since you can have as many paths as you want and
    //      all of them will be valid. But given that no cycle is present
    //      in the paths from i to j, it should still work fine.

    // In case the graph wasn't a DAG, you can find out whether a node v can
    // reach itself (there exists a cycle) by checking whether count[v][v] is
    // zero or not. if v can reach itself, and a node i can reach v, and v can
    // reach a node j, then the path count from i to j is useless since you can
    // have as many valid paths as you want. In other words, the path count is
    // infinity.

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            answers[i][j] = count[i][j];
        }
    }

    for (int k = 1; k <= n; k++) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {

                int additional_paths = answers[i][k] * answers[k][j];
                answers[i][j] += additional_paths;

            }
        }
    }
}

void set_paths_count()
{
    // It's important that count[i][i] = 0
    // for it to work.

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= n; j++) {
            count[i][j] = 0;
        }
    }

    count[1][2] = 1;
    count[1][4] = 1;
    count[2][3] = 1;
    count[4][5] = 1;
    count[5][3] = 1;
}

int main()
{
    set_paths_count();
    calc_paths_count();
    std::cout << paths_count(1, 3) << std::endl;
}