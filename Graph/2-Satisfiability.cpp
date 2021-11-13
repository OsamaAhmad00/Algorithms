#include <iostream>
#include <functional>
#include <vector>
#include <stack>

#define V(x) SAT2::index_of(x)
#define nV(x) SAT2::compliment(V(x))

const int UNKNOWN = -1;
const int UNVISITED = -1;
typedef std::vector<std::vector<int>> Graph;

// The returned component_of are sorted in a reverse topological order.
std::vector<int> get_SCCs(const Graph& graph)
{
    int n = graph.size();

    std::vector<int> result(n);

    int last_id = 0;
    int last_component_number = 0;
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
            while (stack.top() != x) {
                int node = stack.top();
                stack.pop();
                on_stack[node] = false;
                result[node] = last_component_number;
            }
            // TODO remove code duplication.
            stack.pop();
            on_stack[x] = false;
            result[x] = last_component_number;
            last_component_number++;
        }
    };

    for (int i = 0; i < n; i++) {
        if (id[i] == UNVISITED) {
            dfs(i);
        }
    }

    return result;
}

class SAT2
{
    // Solves a CNF (Conjunctive Normal Form) with
    //  each clause containing at most 2 variables.
    // Anything that can be represented in terms of implication can be
    //  solved. Examples: or, xor, and bidirectional implication (equality)

    // a variable and its complement are stored next to each other.
    //  x0 -> 0, !x0 -> 1. In general, xi -> 2 * i, !xi -> 2 * i + 1.

    int n;
    Graph graph;

    int components_count;
    Graph components_graph;
    Graph components_graph_transpose;

    std::vector<int> component_of;
    std::vector<int> component_val;  // this is unnecessary
    std::vector<std::vector<int>> components;

    std::vector<bool> var_values;

    void set_components()
    {
        // TODO this looks like a lot of
        //  work. optimize this function.

        components.clear();
        components_graph.clear();
        components_graph_transpose.clear();

        component_of = get_SCCs(graph);

        // TODO optimize this.
        components_count = 0;
        for (int i = 0; i < graph.size(); i++) {
            components_count = std::max(components_count, component_of[i]);
        }
        components_count++;

        component_val.resize(components_count, UNKNOWN);
        components.resize(components_count);
        components_graph.resize(components_count);
        components_graph_transpose.resize(components_count);

        for (int i = 0; i < graph.size(); i++) {
            components[component_of[i]].push_back(i);
        }

        for (int i = 0; i < graph.size(); i++) {
            for (int neighbour : graph[i]) {
                if (component_of[i] != component_of[neighbour]) {
                    int a = component_of[i];
                    int b = component_of[neighbour];
                    components_graph[a].push_back(b);
                    components_graph_transpose[b].push_back(a);
                }
            }
        }
    }

    int component_compliment(int component)
    {
        int x = components[component].front();
        return component_of[compliment(x)];
    }

    void set_var_value(int var, int value)
    {
        int index = reverse_index_of(var);
        var_values[index] = value ^ is_compliment(var);
    }

    void set_component_value(int i, bool value)
    {
        component_val[i] = value;

        // for every cycle in the graph (a component), there will be
        //  an exact same cycle with the variables being negated. this
        //  means that each component has a complement component.
        int j = component_compliment(i);
        if (component_val[j] == UNKNOWN)
        {
            // set the variables only if they're
            //  not set from the complement component.
            for (int var : components[i]) {
                set_var_value(var, value);
            }

            set_component_value(j, !value);
        }

        // For a logical implication p -> q:
        //  - if p is true , then q must be true
        //  - if q is false, then p must be false
        // If a -> b -> c -> d:
        //  if b is true, then we're certain that both c and d
        //  (and anything after b in the chain) are true, but
        //  we still don't know the value of a (and anything
        //  before b in the chain).
        //  similarly, if c is false, then we're certain that both
        //  b and a (and anything before c in the chain) are false,
        //  but we still don't know the value of d (and anything
        //  after c in the chain).
        const Graph& g = value ? components_graph : components_graph_transpose;
        for (int component : g[i]) {
            set_component_value(component, value);
        }
    }

public:

    explicit SAT2(int n) : n(n), graph(n * 2) {}

    static inline int index_of(int x) {
        // TODO find a clean way to
        //  get rid of this function.
        return x * 2;
    }

    static inline int reverse_index_of(int x) {
        return x / 2;
    }

    static inline int compliment(int x) {
        // 4 -> 5
        // 5 -> 4
        return x ^ 1;
    }

    static inline bool is_compliment(int x) {
        return x % 2;
    }

    void add_implication(int p, int q) {
        graph[p].push_back(q);
    }

    void force_true(int x) {
        // equivalent to (x or x)
        add_implication(compliment(x), x);
    }

    void force_false(int x) {
        force_true(compliment(x));
    }

    void add_or(int p, int q) {
        // if p is false, then q must be true.
        //  otherwise, the expression will be
        //  false. so, p is false implies q.
        // same for q and p.
        add_implication(compliment(p), q);
        add_implication(compliment(q), p);
    }

    void add_xor(int p, int q) {
        add_or(p, q);
        add_or(compliment(p), compliment(q));
    }

    void add_bidirectional_implication(int p, int q) {
        add_xor(compliment(p), q);
    }

    bool has_no_solution()
    {
        for (int i = 0; i < n; i++) {
            // !x ->  x = x is true.
            //  x -> !x = x is false.
            // !x -> x and x -> !x means
            //  x is both true and false.
            // Thus, if x and !x are in a
            // cycle, there is no solution.
            int x = index_of(i);
            if (component_of[x] == component_of[compliment(x)]) {
                return true;
            }
        }
        return false;
    }

    std::vector<bool> solve()
    {
        // For a logical implication p -> q:
        //  - if p is true , then q must be true
        //  - if q is false, then p must be false
        // If a -> b -> c -> a (a, b, and c are in a cycle),
        //  it means that all of them must have the same value.
        // Since all variables in a cycle have the same value,
        //  it makes sense work with the strongly connected components.
        // Computing the SCCs doesn't only reduce the number of nodes
        //  to work with, but also converts the graph to a DAG which
        //  is needed for the following code to work properly.

        // TODO if there is no solution, some unnecessary
        //  computations are done (anything other than the
        //  computation of component_of).
        set_components();

        if (has_no_solution()) {
            return {};
        }

        var_values.resize(n);

        // For a logical implication p -> q:
        //  - if q is true , then p can be true or false.
        //  using this fact, if a component doesn't imply other
        //  components, we'll set it to be true and set all the other
        //  relating components to their corresponding values, and
        //  consider it removed from the graph, and repeat until
        //  we're done.
        // Luckily, Tarjan's algorithm returns the components sorted
        //  in a reverse topological order. which means that we can
        //  just iterate over the components without trying to compute
        //  the reverse topological order.
        for (int i = 0; i < components_count; i++) {
            if (component_val[i] == UNKNOWN) {
                set_component_value(i, true);
            }
        }

        return var_values;
    }
};

SAT2 get_sample_1()
{
    SAT2 sat(6);

    sat.add_or(V(0), nV(1));
    sat.add_or(V(1), nV(4));
    sat.add_xor(V(1), nV(2));
    sat.add_xor(V(3), nV(2));
    sat.add_xor(V(3), V(4));
    sat.force_false(V(5));

    return sat;
}

SAT2 get_sample_2()
{
    SAT2 sat(6);

    sat.add_xor(V(0), V(1));
    sat.add_xor(V(1), V(2));
    sat.add_xor(V(2), V(3));
    sat.add_xor(V(3), V(4));
    sat.add_xor(V(4), V(5));
    sat.force_true(V(5));

    return sat;
}

SAT2 get_sample_3()
{
    SAT2 sat(5);

    sat.add_or(V(0), nV(1));
    sat.add_or(V(1), nV(4));
    sat.add_xor(V(1), nV(2));
    sat.add_xor(V(3), nV(2));
    sat.add_xor(V(3), V(4));
    sat.force_true(V(4));

    return sat;
}

SAT2 get_sample_4()
{
    SAT2 sat(6);

    sat.add_xor(V(0), V(1));
    sat.add_xor(V(1), V(2));
    sat.add_xor(V(2), V(3));
    sat.add_xor(V(3), V(4));
    sat.add_xor(V(4), V(5));
    sat.add_bidirectional_implication(V(0), V(1));

    return sat;
}

void test(const SAT2& sat)
{
    auto copy = sat;  // because the parameter is const.
    auto res = copy.solve();
    if (res.empty()) {
        std::cout << "No solution." << std::endl;
    } else {
        for (int i = 0; i < res.size(); i++) {
            std::cout << "x" << i << ": " << res[i] << std::endl;
        }
    }
    std::cout << std::endl;
}

int main()
{
    test(get_sample_1());
    test(get_sample_2());
    test(get_sample_3());
    test(get_sample_4());
}