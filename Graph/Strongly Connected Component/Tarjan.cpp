#include <iostream>
#include <functional>
#include <vector>
#include <stack>

const int UNVISITED = -1;
typedef std::vector<std::vector<int>> Graph;

std::vector<std::vector<int>> get_SCC(const Graph& graph)
{
    std::vector<std::vector<int>> result;

    int n = graph.size();
    int last_id = 0;
    std::vector<int> id(n, UNVISITED);
    std::vector<int> low_link(n);
    std::vector<bool> on_stack(n, false);
    std::stack<int> stack;

    std::function<void(int)> dfs = [&](int x)
    {
        id[x] = low_link[x] = last_id++;
        stack.push(x);
        on_stack[x] = true;

        for (int neighbour : graph[x])
        {
            // self-loops shouldn't be a problem.

            if (id[neighbour] == UNVISITED) {
                dfs(neighbour);
                low_link[x] = std::min(low_link[x], low_link[neighbour]);
            } else if (on_stack[neighbour]) {
                low_link[x] = std::min(low_link[x], low_link[neighbour]);
                // this will also work.
                // low_link[x] = std::min(low_link[x], id[neighbour]);
            }
        }

        if (low_link[x] == id[x]) {
            result.emplace_back();
            while (stack.top() != x) {
                int node = stack.top();
                stack.pop();
                on_stack[node] = false;
                result.back().push_back(node);
            }
            // TODO remove code duplication.
            stack.pop();
            on_stack[x] = false;
            result.back().push_back(x);
        }
    };

    for (int i = 0; i < n; i++) {
        if (id[i] == UNVISITED) {
            dfs(i);
        }
    }

    return result;
}

Graph get_sample_graph_1()
{
    // one component
    Graph result(6);

    result[0].push_back(1);
    result[1].push_back(2);
    result[1].push_back(4);
    result[2].push_back(3);
    result[3].push_back(1);
    result[4].push_back(5);
    result[5].push_back(0);

    return result;
}

Graph get_sample_graph_2()
{
    Graph result(10);

    result[1].push_back(2);
    result[2].push_back(1);
    result[1].push_back(3);
    result[3].push_back(1);
    result[4].push_back(5);
    result[5].push_back(4);
    result[6].push_back(2);
    result[2].push_back(4);
    result[6].push_back(5);
    result[6].push_back(8);
    result[7].push_back(6);
    result[8].push_back(7);
    result[9].push_back(7);
    result[9].push_back(8);
    result[9].push_back(9);

    return result;
}

void test(const std::vector<std::vector<int>> &components)
{
    for (int i = 0; i < components.size(); i++) {
        std::cout << "Component " << i+1 << ": ";
        for (int node : components[i]) {
            std::cout << node << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test(get_SCC(get_sample_graph_1()));
    test(get_SCC(get_sample_graph_2()));
}