#include <iostream>
#include <cstring>

const int n = 5;

const int MAX_VAL = 1'000'000;

int distances[n+1][n+1];
int answers[n+1][n+1];

int min_path(int i, int j)
{
    return answers[i][j];
}

void calc_min_paths()
{
    // Since this is a minimization problem, it's
    // ok to mix the values from the current iteration
    // and the previous iteration. This is not doable
    // for other kinds of problems, but is fine here.
    //
    // For example, when calculating the distance from
    // node 1 to node 2 with k = 5, you'll use the
    // old distances[1][5]. After that you'll calculate
    // the distance from 1 to 5 with k = 5 (which updates
    // distances[1][5]). Then when calculating from 1 to 7
    // with k = 5 again, you'll use the new distances[1][5].
    //
    // This is fine. Remember, the value from the previous
    // iteration is either the same as the current value or
    // MORE. this MORE value is definitely wrong and whatever
    // calculations that involved this wrong value will be
    // calculated again using the new (current) value. so,
    // in this case, mixing the old and the new values is fine.
    //
    // You can think of this solution in another way
    // other than being an optimization for the recursive
    // solution. You can think of it as taking each node
    // k, and make a direct path from every pair of the
    // nodes connected to k (i and j) if the distance from
    // i to k + from k to j is less than the current direct
    // length. This is called "Edge Relaxation". The thing is,
    // by relaxing the edges, you add new edges that connects
    // (or reduces the distance) between two new nodes. and
    // in the further iterations, you'll relax again using
    // this new edge, which means that you've constructed a
    // new edge that represents a path with multiple nodes
    // representing it. The order of the of the relaxations
    // (the node k that will be processed) doesn't matter.
    // you just have to do this for every single node. if you
    // do this, you'll have some wrong answers in the middle
    // operations (some edges that need to be relaxed more),
    // but it's guaranteed that at the end, you'll end up
    // with a correct result for every pair in the graph.

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
    std::cout << min_path(2, 3) << std::endl;
}