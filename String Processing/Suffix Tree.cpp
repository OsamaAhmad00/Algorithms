#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>

const int NOT_SUFFIX = -1;

struct CyclicShift
{
    int start_index;
    int rank;

    // Comparisons and indexing are done
    //  by the rank of the cyclic shift.
    operator int() const { return rank; }
};

std::vector<CyclicShift> stable_sort(const std::vector<CyclicShift> &shifts)
{
    // Count Sort - O(N + M) where N is the size of
    // the string and M is the size of count array.

    int max_val = 0;
    for (const CyclicShift& shift : shifts)
        max_val = std::max(max_val, shift.rank);

    std::vector<int> count(max_val + 1);

    for (const CyclicShift& shift : shifts)
        count[shift.rank]++;

    for (int i = 1; i < count.size(); i++)
        count[i] += count[i-1];

    std::vector<CyclicShift> result(shifts.size());
    for (int i = shifts.size() - 1; i >= 0; i--) {
        int new_index = --count[shifts[i].rank];
        result[new_index] = shifts[i];
    }

    return result;
}

class SuffixArray
{

    // Suffix Array is a compressed form of "Suffix Tree",
    // which in turn is a compressed form of "Suffix Trie".

    int n;
    int current_shift_length;
    std::vector<int> old_orders;
    std::vector<CyclicShift> shifts;
    std::vector<int> suffix_array;

    void initialize_shifts(const std::string& string)
    {
        shifts.back() = {n - 1, 0};

        for (int i = 0; i < n - 1; i++) {
            shifts[i].start_index = i;
            shifts[i].rank = string[i];
        }

        shifts = stable_sort(shifts);

        shifts[0].rank = 0;
        for (int i = 1; i < n; i++) {
            int curr = shifts[i  ].start_index;
            int prev = shifts[i-1].start_index;
            bool not_equal = string[curr] != string[prev];
            shifts[i].rank = shifts[i-1].rank + not_equal;
        }
    }

    int shifted_index(int index, int steps = 1) const {
        return (index - current_shift_length * steps + n) % n;
    }

    void set_new_orders()
    {
        shifts[0].rank = 0;
        for (int i = 1; i < n; i++)
        {
            shifts[i].rank = shifts[i-1].rank;

            int curr = shifts[i  ].start_index;
            int prev = shifts[i-1].start_index;

            bool different_first_half = old_orders[curr] != old_orders[prev];
            bool different_second_half = false;

            if (!different_first_half) {
                int curr_shifted = shifted_index(curr, -1);
                int prev_shifted = shifted_index(prev, -1);
                int a = old_orders[curr_shifted];
                int b = old_orders[prev_shifted];
                different_second_half = (a != b);
            }

            if (different_first_half || different_second_half) {
                shifts[i].rank++;
            }
        }
    }

    void set_old_orders()
    {
        for (CyclicShift& shift : shifts)
            old_orders[shift.start_index] = shift.rank;
    }

    void shift()
    {
        for (CyclicShift& shift: shifts) {
            shift.start_index = shifted_index(shift.start_index);
            shift.rank = old_orders[shift.start_index];
        }

        shifts = stable_sort(shifts);

        set_new_orders();
    }

    void double_shift_length()
    {
        set_old_orders();
        shift();
        current_shift_length *= 2;
    }

    void compute_suffix_array(const std::string& string)
    {
        initialize_shifts(string);

        while (current_shift_length < n)
            double_shift_length();

        for (int i = 1; i < n; i++)
            suffix_array[i - 1] = shifts[i].start_index;
    }

public:

    explicit SuffixArray(const std::string& string) : n(string.size() + 1),
        current_shift_length(1), shifts(n), old_orders(n), suffix_array(n - 1)
    {
        compute_suffix_array(string);
    }

    const std::vector<int>& get_suffix_array() { return suffix_array; }
};

class LCP
{
    std::vector<int> ranks;
    std::vector<int> LCP_array;

    // These are just references, not going to be
    // used, and not guaranteed to be valid references
    // once the computation of the suffix array is done.
    const std::string& string;
    const std::vector<int>& suffix_array;

    void set_ranks()
    {
        // ranks[i] = rank of suffix that starts
        //  at index i in the original string.
        // rank = the position of the suffix in
        //  the suffix array.
        for (int rank = 0; rank < suffix_array.size(); rank++) {
            int start_index = suffix_array[rank];
            ranks[start_index] = rank;
        }
    }

    bool has_preceding_suffix(int i) const {
        // has preceding suffix in the suffix array.
        return ranks[i] > 0;
    }

    void compute_LCP()
    {
        // Kasai's algorithm
        // Great explanation: https://stackoverflow.com/a/63104083/9140652
        // O(n). If the computation of the suffix array takes O(n * log(n))
        //  the overall complexity will be O(n * log(n)).
        // Why is this linear and not quadratic (since we have two nested loops)?
        //  - matched_characters increases as long as the inner loop is running.
        //  - matched_characters can't exceed string.size().
        //  - on each iteration, matched_characters decreases by 1 at most.
        //  Combining these 3 points, we see that these two nested loops are O(n).

        set_ranks();

        int matched_characters = 0;
        for (int start_index = 0; start_index < string.size(); start_index++) {
            if (has_preceding_suffix(start_index))
            {
                // index in the suffix array of
                // the suffix that starts at start_index
                int rank = ranks[start_index];

                int i = suffix_array[rank    ] + matched_characters;
                int j = suffix_array[rank - 1] + matched_characters;
                while (i < string.size() && j < string.size() && string[i++] == string[j++])
                    matched_characters++;

                LCP_array[rank - 1] = matched_characters;

                if (matched_characters > 0)
                    matched_characters--;
            }
        }
    }

public:

    // LCP_array[i] = the longest common prefix
    // of suffix_array[i] and suffix_array[i + 1]

    LCP(const std::string& string, const std::vector<int>& suffix_array)
            : ranks(string.size()), LCP_array(string.size() - 1), string(string), suffix_array(suffix_array)
    {
        compute_LCP();
    }

    const std::vector<int>& get_LCP() const { return LCP_array; }
};

class SuffixTree
{

public:

    struct Node;

    struct Edge
    {
        // Label of the edge = string.substr(start_index, length)
        int start_index;
        int length;

        Node* node;

        std::string get_label(const std::string& string) {
            return string.substr(start_index, length);
        }

        Edge() = default;
        Edge(Node* node)
            : start_index(-1), length(-1), node(node) {}
        Edge(int start_index, int length)
            : start_index(start_index), length(length), node(new Node) {}

    };

    struct Node
    {
        std::vector<Edge> children;
        int suffix_start_index;

        Node() : children({}), suffix_start_index(NOT_SUFFIX) {}

        Node(int suffix_start_index) : suffix_start_index(suffix_start_index) {}

        bool is_suffix() const { return suffix_start_index != NOT_SUFFIX; }

        bool is_leaf() const { return children.empty(); }

        void destroy()
        {
            // Once the root node is
            // destroyed, all its children
            // will be destroyed recursively.
            for (Edge &e : children) {
                delete e.node;
            }
            delete this;
        }
    };

private:

    // These are just references, not going to be
    // used, and not guaranteed to be valid references
    // once the computation of the suffix tree is done.
    const std::vector<int>& suffix_array;
    const std::vector<int>& LCP_array;

    Node *root;
    Edge current;
    std::stack<Edge> parents;

    // Number of characters from
    //  the root to the current node,
    //  not the number of edges from
    //  the root to the current node.
    int current_depth;

    Edge pop_last_parent()
    {
        Edge parent = parents.top();
        parents.pop();
        return parent;
    }

    void move_to_parent() {
        current_depth -= current.length;
        current = pop_last_parent();
    }

    void move_to_last_child() {
        parents.push(current);
        current = current.node->children.back();
        current_depth += current.length;
    }

    void add_child(const Edge& child) { current.node->children.push_back(child); }

    Edge get_last_child() { return current.node->children.back(); }

    void unlink_last_child() { current.node->children.pop_back(); }

    void add_and_move(const Edge& child) {
        add_child(child);
        move_to_last_child();
    }

    void split_and_move(int split_depth)
    {
        Edge bottom_part = get_last_child();
        unlink_last_child();

        int new_len = split_depth - current_depth;
        Edge top_part { bottom_part.start_index, bottom_part.length - new_len };
        bottom_part.length = new_len;
        bottom_part.start_index += new_len;

        add_and_move(top_part);
        add_child(bottom_part);
    }

    Edge get_suffix_edge(int i, int depth)
    {
        int string_length = suffix_array.size();

        Edge e{};
        e.start_index = suffix_array[i] + depth;
        e.length = string_length - e.start_index;
        e.node = new Node{suffix_array[i]};

        return e;
    }

    void compute_suffix_tree()
    {
        // The smallest suffix lexicographically doesn't have
        //  a preceding suffix in the suffix array, thus, we
        //  add it to the tree directly, then for the following
        //  suffices, we get the longest common prefix (LCP) of
        //  the current suffix and the lastly added suffix.
        // The LCP of 2 suffices is the number of common characters
        //  between two suffices.
        // For 2 suffices, a and b, with lengths aL and bL respectively,
        //  and an LCP of lcp, we would want a single edge representing
        //  the first lcp characters of the two suffices, then two edges
        //  representing the following aL - lcp characters in a and the
        //  following bL - lcp characters in b.
        // Note that we go in the LCP array in order, and LCP_array[i]
        //  describes the LCP of the suffices i and i + 1. This means
        //  that we're adding the suffix i + 1, we don't need to compare
        //  it with any suffices other than the suffix i, which we've put
        //  at the previous iteration. This is why the functions deal with
        //  the "last" child. We don't need to lookup for any other children.
        // We maintain a pointer to the current node. The current node is
        //  always at the end of the suffix we've just added in the previous
        //  iteration. We also maintain a current_depth variable, which
        //  stores the number of characters from the root to the current node
        //  (not the number of edges from the root to the current node).
        // The idea here is that for each suffix, we go to a depth = the LCP
        //  of the suffix with the previously added suffix, then add the
        //  difference (in the above example, bL - lcp) as a new edge at that
        //  position (the position with depth = the LCP).
        // If we're at a depth > the desired depth, then obviously we need to
        //  go up in the tree to some parent to make the depth smaller.
        // Since we don't go up a character by character, after going up, we
        //  might end up in a node with depth < the desired depth. This means
        //  that we have to "split" the lastly used edge into two parts such
        //  that the node that's splitting the edge have a depth = the desired
        //  depth. Note that this case can't happen unless we've gone up to
        //  the parent nodes, which means that there exists at least one child
        //  to the current node.
        // After getting to the node with depth = the desired depth, we add
        //  the new node representing the suffix.


        current_depth = 0;
        add_and_move(get_suffix_edge(0, 0));

        for (int i = 0; i < LCP_array.size(); i++)
        {
            // Number of characters from
            //  the root to the node with,
            //  not the number of edges from
            //  the root to the current node.
            int desired_depth = LCP_array[i];

            while (current_depth > desired_depth)
                move_to_parent();

            if (current_depth < desired_depth)
                split_and_move(desired_depth);

            add_and_move(get_suffix_edge(i+1, desired_depth));
        }
    }

public:

    SuffixTree(const std::vector<int>& suffix_array, const std::vector<int>& LCP_array)
        : suffix_array(suffix_array), LCP_array(LCP_array), root(new Node {}), current(root)
    {
        compute_suffix_tree();
    }

    Node* get_suffix_tree() const { return root; }
};

void print_suffix_tree(SuffixTree::Node* node, const std::string& string, int width = 0)
{
    for (SuffixTree::Edge& e : node->children)
    {
        for (int i = 0; i < width; i++)
            std::cout << ' ';

        std::cout << e.get_label(string);
        if (e.node->is_suffix())
            std::cout << "   <--- " << e.node->suffix_start_index;
        std::cout << std::endl;

        print_suffix_tree(e.node, string, width + 2);
    }
}

void test(const std::string& string)
{
    SuffixArray suffix_array(string);
    LCP lcp(string, suffix_array.get_suffix_array());
    SuffixTree tree(suffix_array.get_suffix_array(), lcp.get_LCP());

    std::cout << "String: " << string << std::endl;
    print_suffix_tree(tree.get_suffix_tree(), string);

    std::cout << std::endl << std::endl;
}

int main()
{
    test("GATAGACA");
    test("MISSISSIPPI");
    test("ABCD");
    test("AABBCD");
}