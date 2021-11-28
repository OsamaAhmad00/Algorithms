#include <iostream>
#include <vector>
#include <queue>

const int NOT_A_WORD = -1;
const int WORD_NO_INDEX = -2;
const int CHARACTERS_COUNT = 26;

int get_letter_index(char c) { return c - 'a'; }

struct Node
{
    const int index;
    int word_index;
    std::vector<Node*> children;
    std::vector<int> children_indices;

    explicit Node(int index, int word_index)
        : index(index), word_index(word_index), children(CHARACTERS_COUNT, nullptr) {}

    Node* get_child(int i) { return children[i]; }

    void set_child(int i, Node *child)
    {
        if (children[i] == nullptr) {
            children_indices.push_back(i);
        }
        children[i] = child;
    }

    const std::vector<int>& get_children_indices() const { return children_indices; }

    bool is_word() const { return word_index != NOT_A_WORD; }
};

class Trie
{
    std::vector<Node*> nodes;

public:

    Node* const root;

    explicit Trie(const std::vector<std::string> &words = {}) : root(create_node())
    {
        for (int i = 0; i < words.size(); i++) {
            add_word(words[i], i);
        }
    }

    ~Trie()
    {
        for (Node* node : nodes) {
            delete node;
        }
    }

    Node* create_node(int word_index = NOT_A_WORD)
    {
        nodes.push_back(new Node(nodes.size(), word_index));
        return nodes.back();
    }

    Node* get_node(int id) { return nodes[id]; }

    void add_word(const std::string& word, int word_index)
    {
        Node* current = root;
        for (int i = 0; i < word.size(); i++)
        {
            int letter_index = get_letter_index(word[i]);
            Node* child = current->get_child(letter_index);

            if (child == nullptr) {
                child = create_node();
                current->set_child(letter_index, child);
            }

            if (i == word.size() - 1) {
                child->word_index = word_index;
            }

            current = child;
        }
    }

    int size() const { return nodes.size(); }
};

class AhoCorasick
{
    Trie trie;
    Node* root;
    int current_node;
    std::queue<int> queue;
    std::vector<int> failure_links;
    std::vector<std::vector<int>> result;

    // TODO this assumes that each node
    //  needs only one dictionary link
    //  verify that.
    std::vector<int> dictionary_links;

    Node* get_failure_node(int id) { return trie.get_node(failure_links[id]); }

    void push_first_level_nodes()
    {
        // Base case: nodes of the first level will
        //  all have a failure link to the root.
        //  this way, nodes in the following levels
        //  start matching starting from the root.
        // A dictionary link to the root means that
        //  there is no link to any word.
        for (int i : root->get_children_indices()) {
            Node* child = root->get_child(i);
            failure_links[child->index] = root->index;
            dictionary_links[child->index] = root->index;
            queue.push(child->index);
        }
    }

    void set_failure_link(Node* parent, int letter_index)
    {
        // Because a BFS is used, we know that all
        //  failure links of the nodes in the previous
        //  levels are already set.
        // If the node the parent fails at has a child
        //  node for the desired letter, then that's
        //  the failure node of the child.
        Node* child = parent->get_child(letter_index);
        Node* parent_failure = get_failure_node(parent->index);
        Node* failure_child = parent_failure->get_child(letter_index);
        failure_links[child->index] = failure_child != nullptr ? failure_child->index : root->index;
    }

    void set_dictionary_link(Node* node)
    {
        // This assumes that only one dictionary
        //  link is enough for each node.
        // Because a BFS is used, we know that all
        //  dictionary links of the nodes in the
        //  previous levels are already set.
        // If the failure node is a word itself,
        //  then it's our dictionary node, other
        //  than that, since the dictionary link
        //  of the failure node is already set
        //  (because it's in a previous level),
        //  we can utilize this fact and set it
        //  to be our dictionary link.
        int index = node->index;
        Node* failure_node = get_failure_node(index);
        dictionary_links[index] = failure_node->is_word() ?
                                  failure_node->index : dictionary_links[failure_node->index];
    }

    void clear_queue() { while (!queue.empty()) queue.pop(); }

    void compute_links()
    {
        // The idea here is to use BFS to set
        // all previous levels and keep setting
        // links incrementally.

        clear_queue();
        push_first_level_nodes();

        while (!queue.empty())
        {
            Node* node = trie.get_node(queue.front());
            queue.pop();

            for (int i : node->get_children_indices()) {
                set_failure_link(node, i);
                Node* child = node->get_child(i);
                set_dictionary_link(child);
                queue.push(child->index);
            }
        }
    }

    void add_matches(int text_index)
    {
        // https://www.youtube.com/watch?v=O7_w001f58c
        // https://www.youtube.com/watch?v=OFKxWFew_L0

        // Dictionary links should be forming
        // a chain with root = current node
        // The chain can be empty (in case
        // current node is not a word), or
        // consisting of a single node (in case
        // this node is a word, and it has no
        // dictionary links), or consisting of
        // multiple nodes.

        int current = current_node;

        if (!trie.get_node(current)->is_word()) {
            return;
        }

        while (current != root->index) {
            Node* node = trie.get_node(current);
            result[node->word_index].push_back(text_index);
            current = dictionary_links[current];
        }
    }

    void move_to(int letter_index)
    {
        // While we can't move from the current
        //  node to the desired letter, and we're
        //  not yet at the root node, fail.

        Node* child;

        while (true) {
            Node* node = trie.get_node(current_node);
            child = node->get_child(letter_index);
            if (child == nullptr && current_node != root->index) {
                current_node = failure_links[current_node];
            } else {
                break;
            }
        }

        current_node = (child != nullptr) ? child->index : failure_links[current_node];
    }

public:

    // This is similar to the KMP algorithm, uses the
    //  same idea, the failure links.
    // For KMP, the failure link points to the end of
    //  the longest prefix match in the same pattern
    // For Aho-Corasick, the failure link points to the
    //  end of the longest prefix match in all patterns
    // A trie data structure is used to be able to combine
    //  all patterns into a single object to get the longest
    //  common prefix in all patterns combined.

    explicit AhoCorasick(const std::vector<std::string>& patterns)
        : trie(patterns), result(patterns.size())
    {
        root = trie.root;
        failure_links.resize(trie.size());
        dictionary_links.resize(trie.size());
        compute_links();
    }

    std::vector<std::vector<int>> get_matches(const std::string& text)
    {
        // For a pattern with index i,
        // result[i] = a vector of the indices
        // at which the pattern ends.

        for (auto& v : result) v.clear();

        current_node = root->index;

        for (int i = 0; i < text.size(); i++) {
            int letter_index = get_letter_index(text[i]);
            move_to(letter_index);
            add_matches(i);
        }

        return result;
    }
};

void test(const std::string& text, const std::vector<std::string>& patterns)
{
    AhoCorasick ahoCorasick{patterns};
    auto result = ahoCorasick.get_matches(text);

    std::cout << "Text: " << text << std::endl;
    for (int i = 0; i < patterns.size(); i++) {
        std::cout << "  Pattern \"" << patterns[i] << "\" matches at indices: ";
        for (int index : result[i]) {
            std::cout << index - patterns[i].size() + 1 << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test("abababaa", {"aba", "a"});
    test("abababaa", {"aba", "a", "abab", "xyz", "aa"});
    test("ahishers", {"he", "she", "hers", "his"});
}