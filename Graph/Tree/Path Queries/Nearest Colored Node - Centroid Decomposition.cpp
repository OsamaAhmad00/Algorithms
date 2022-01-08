#include <iostream>
#include <vector>
#include <queue>

const int MAX_VALUE = 1'000'000;

typedef std::vector<std::vector<int>> Tree;

int log2_floor(int x)
{
    int result = 0;
    while (x >>= 1) result++;
    return result;
}

struct RootedTree
{
    int root;
    Tree tree;
};

void add_edge(Tree& tree, int parent, int child)
{
    tree[parent].push_back(child);
    tree[child].push_back(parent);
}

class CentroidDecomposition
{
    const Tree& tree;

    int root;
    Tree centroid_decomposition;
    std::vector<bool> is_blocked;
    std::vector<int> subtree_size;
    std::queue<int> current_centroids;

    void calc_subtree_size(int node, int parent = -1)
    {
        subtree_size[node] = 1;
        for (int child : tree[node]) {
            if (!is_blocked[child] && child != parent) {
                calc_subtree_size(child, node);
                subtree_size[node] += subtree_size[child];
            }
        }
    }

    int find_centroid(int node, int tree_size, int parent = -1)
    {
        for (int child : tree[node])
        {
            if (is_blocked[child] || child == parent)
                continue;

            if (subtree_size[child] > tree_size / 2)
                return find_centroid(child, tree_size, node);
        }

        return node;
    }

    void init_root_centroid()
    {
        std::fill(is_blocked.begin(), is_blocked.end(), false);

        const int initial_root = 0;
        calc_subtree_size(initial_root);
        root = find_centroid(initial_root, tree.size());
        current_centroids.push(root);
        is_blocked[root] = true;
    }

    void process_next_centroid()
    {
        int centroid = current_centroids.front();
        current_centroids.pop();

        for (int child : tree[centroid]) {
            if (!is_blocked[child])
            {
                calc_subtree_size(child);

                int child_centroid = find_centroid(child, subtree_size[child]);
                add_edge(centroid_decomposition, centroid, child_centroid);
                current_centroids.push(child_centroid);
                is_blocked[child_centroid] = true;
            }
        }
    }

    void construct_centroid_decomposition()
    {
        init_root_centroid();

        while (!current_centroids.empty())
            process_next_centroid();
    }

public:

    CentroidDecomposition(const Tree& tree) :
            tree(tree),
            subtree_size(tree.size()),
            centroid_decomposition(tree.size()),
            is_blocked(tree.size())
    {}

    RootedTree get_centroid_decomposition()
    {
        construct_centroid_decomposition();
        return {root, std::move(centroid_decomposition)};
    }
};

class LowestCommonAncestor
{
    const Tree& tree;

    std::vector<int> depths;

    // ancestors[k][i] = the
    //  (2^k)th ancestor of the node i.
    std::vector<std::vector<int>> ancestors;

    int i, j;

    void dfs(int node, int depth, int parent = -1)
    {
        depths[node] = depth;
        for (int child : tree[node]) {
            if (child == parent) continue;
            ancestors[0][child] = node;
            dfs(child, depth + 1, node);
        }
    }

    void init_ancestors()
    {
        for (int k = 1; k < ancestors.size(); k++) {
            for (int node = 0; node < tree.size(); node++) {
                int ancestor = ancestors[k - 1][node];
                if (ancestor == -1) {
                    ancestors[k][node] = -1;
                } else {
                    ancestors[k][node] = ancestors[k - 1][ancestor];
                }
            }
        }
    }

    void init(int root)
    {
        int n = tree.size();
        int max_power = log2_floor(n);

        depths.resize(n);
        ancestors.resize(max_power + 1, std::vector<int>(n));

        ancestors[0][root] = -1;
        dfs(root, 0);

        init_ancestors();
    }

    int parent_of(int x) {
        return ancestors[0][x];
    }

    void set_at_same_depth()
    {
        // This assumes that i <= j.
        int diff = depths[j] - depths[i];

        while (diff)
        {
            int biggest_power_of_2 = log2_floor(diff);
            j = ancestors[biggest_power_of_2][j];
            diff -= (1 << biggest_power_of_2);
        }
    }

public:

    int distance(int node1, int node2) {
        int lca = get_LCA(node1, node2);
        return (depths[node1] - depths[lca]) + (depths[node2] - depths[lca]);
    }

    int get_LCA(int node1, int node2)
    {
        i = node1;
        j = node2;
        if (depths[i] > depths[j])
            std::swap(i, j);

        set_at_same_depth();

        if (i == j)
            return i;

        int k = log2_floor(depths[i]);

        for (; k >= 0; k--) {
            if (ancestors[k][i] != ancestors[k][j]) {
                i = ancestors[k][i];
                j = ancestors[k][j];
            }
        }

        return parent_of(i);
    }

    LowestCommonAncestor(const Tree& tree, int root) : tree(tree)
    {
        init(root);
    }
};

class NearestColoredNode
{

public:

    struct NearNode
    {
        int node;
        int distance;
    };

private:

    const Tree& tree;
    RootedTree centroid_decomposition;
    // Binary Lifting is being used here to
    // compute the LCA, but you can use any
    // other method for finding the LCA.
    LowestCommonAncestor lca;
    std::vector<int> parent_of;
    std::vector<NearNode> nearest_nodes;

    int distance(int node1, int node2)
    {
        // A quick hack. since the LCA class contains
        // an array of depths, why not use it.
        return lca.distance(node1, node2);
    }

    void minimize_nearest_node(int node, int near_node)
    {
        int dist = distance(node, near_node);
        if (dist < nearest_nodes[node].distance) {
            nearest_nodes[node].distance = dist;
            nearest_nodes[node].node = near_node;
        }
    }

    void set_centroid_decomposition_parents(int node, int parent = -1)
    {
        parent_of[node] = parent;
        for (int child : centroid_decomposition.tree[node])
            if (child != parent)
                set_centroid_decomposition_parents(child, node);
    }

    void init()
    {
        set_centroid_decomposition_parents(centroid_decomposition.root);
        for (int i = 0; i < tree.size(); i++)
            nearest_nodes[i].distance = MAX_VALUE;
    }

public:

    // TODO figure out how to uncolor a node.

    // TODO review the explanation below.

    // Algorithm:
    //  For each node in the tree, store the nearest colored node in ITS SUBTREE.
    //  Initially, all nodes are not colored, thus all nodes won't have a nearest
    //   node in their subtree
    //  When coloring a node, go to the ancestors of this node, and update their
    //   values (nearest node = nearest(current nearest node, newly colored node)).
    //   This will maintain the invariant, that every node stores the nearest
    //   colored node in its subtree. This takes O(n).
    //  When looking for the nearest colored node, it can be one of two cases:
    //   - In the node's subtree, in which case, you can get the nearest node in O(1).
    //   - Outside the subtree, in which case, the nearest node will be in the subtree
    //     of one of the ancestors, which means that it's the nearest node to one of
    //     the node's ancestors. The distance from the current node to the nearest
    //     node = the distance from the current node to the ancestor + the distance
    //     from the ancestor to its nearest node.
    //     We can compute the distance from the current node to the ancestor in O(1).
    //     Also, we can get the distance from the ancestor to its nearest node in O(1)
    //     since we store this value for all nodes. But we still need to go up to all
    //     ancestors, which takes O(n). This is because the tree might be a chain, and
    //     we might go from the bottom of the chain up to the root.
    //  Since we don't know which is the case, we'll get the result for both cases, and
    //   return the nearest.
    //  So, both the updates and the lookups will take O(n). This is not desired. Can we
    //   do better than this?
    //  We can restructure the tree so that its height is O(log(n)), and still have this
    //   algorithm working on it.
    //  We'll use a centroid decomposition tree. Its height is O(log(n)), which means that
    //   the updates and the lookups should be O(log(n)), assuming that the distance between
    //   two nodes can be computed in O(1). Here, we'll use a binary lifting, which takes
    //   O(log(n)) as well, which makes both updates and queries take O(log(n)^2).
    //  Since the centroid decomposition tree won't have the same set of edges as in the
    //   original tree, an ancestor in the centroid decomposition tree doesn't have to be
    //   an ancestor in the original tree. Moreover, even if all ancestors in the centroid
    //   decomposition tree were ancestors in the original tree, they don't have to have
    //   the same order, thus, we'll have to compute the distance every time we go up a node
    //   in the centroid decomposition tree.
    //  But, why would it work? Notice that the algorithm works in the original tree because
    //   going up the ancestors in the original tree ensures that we explore every single node
    //   in the tree. If it's not in our subtree, then it must be in the subtree of one
    //   of our ancestors, up to the root. If we can ensure that it's the same for the
    //   centroid decomposition tree, and that we won't miss some nodes in the tree while
    //   going up the ancestors in the centroid decomposition tree, then we can use the
    //   centroid decomposition tree.
    //  One property of a centroid decomposition tree is that any subtree in it will
    //   represent a connected component in the original tree. Furthermore, for any adjacent
    //   node to the connected component in the original tree, if it's not a child in the
    //   subtree in the centroid decomposition tree, it will be an ancestor of the subtree.
    //   This means that for a given node, in the centroid decomposition tree, if we search
    //   in its subtree (the stored value), and search in the subtrees of the ancestors, we
    //   would be searching for every node in the tree, which means that  we can use a centroid
    //   decomposition tree and get a correct result.

    void color_node(int node)
    {
        int current_node = node;
        while (current_node != -1) {
            minimize_nearest_node(current_node, node);
            current_node = parent_of[current_node];
        }
    }

    NearNode get_closest_colored_node(int node)
    {
        NearNode result = nearest_nodes[node];
        int current_node = parent_of[node];
        while (current_node != -1)
        {
            int current_distance = nearest_nodes[current_node].distance;
            int nearest_node = nearest_nodes[current_node].node;

            int total_distance = current_distance + distance(node, current_node);

            if (total_distance < result.distance) {
                result.node = nearest_node;
                result.distance = total_distance;
            }

            current_node = parent_of[current_node];
        }
        return result;
    }

    // We query the centroid decomposition tree, but we
    // use the original tree to compute the distance.
    NearestColoredNode(const Tree& tree, int root = 0) :
        tree(tree),
        centroid_decomposition(CentroidDecomposition(tree).get_centroid_decomposition()),
        lca(tree, root),
        parent_of(tree.size()),
        nearest_nodes(tree.size())
    {
        init();
    }
};

struct Query
{
    enum Type {query, color, uncolor};
    Type type;
    int node;
};

void test(const Tree& tree, const std::vector<Query>& queries)
{
    NearestColoredNode ncn(tree);
    for (const Query& query : queries)
    {
        if (query.type == Query::color) {
            std::cout << "Coloring the node " << query.node + 1 << std::endl;
            ncn.color_node(query.node);
//        } else if (query.type == Query::uncolor) {
//            std::cout << "Uncoloring the node " << query.node + 1 << std::endl;
//            ncn.uncolor_node(query.node);
        } else if (query.type == Query::query) {
            auto result = ncn.get_closest_colored_node(query.node);
            if (result.distance == MAX_VALUE) {
                std::cout << "There is no colored node in the tree yet." << std::endl;
            } else {
                std::cout << "Nearest colored node to node " << query.node + 1
                          << " is node " << result.node + 1 << " with a distance of "
                          << result.distance << std::endl;
            }
        }
    }
}

Tree get_sample_tree_1()
{
    Tree tree(7);

    add_edge(tree, 0, 1);
    add_edge(tree, 0, 2);

    add_edge(tree, 1, 5);
    add_edge(tree, 1, 6);

    add_edge(tree, 2, 3);
    add_edge(tree, 2, 4);

    return tree;
}

Tree get_sample_tree_2()
{
    Tree tree(16);

    add_edge(tree, 3, 0);
    add_edge(tree, 3, 1);
    add_edge(tree, 3, 2);

    add_edge(tree, 4, 3);

    add_edge(tree, 5, 4);
    add_edge(tree, 5, 6);
    add_edge(tree, 5, 9);

    add_edge(tree, 6, 7);
    add_edge(tree, 6, 8);

    add_edge(tree, 9, 10);

    add_edge(tree, 10, 11);
    add_edge(tree, 10, 12);

    add_edge(tree, 11, 13);

    add_edge(tree, 12, 14);
    add_edge(tree, 12, 15);

    return tree;
}

std::vector<Query> get_sample_queries_1()
{
    return {
        {Query::query, 1},
        {Query::color, 1},
        {Query::query, 0},
        {Query::query, 4},
        {Query::color, 5},
        {Query::query, 5},
    };
}

std::vector<Query> get_sample_queries_2()
{
    return {
        {Query::query, 1},
        {Query::color, 1},
        {Query::query, 0},
        {Query::query, 4},
        {Query::query, 13},
        {Query::color, 15},
        {Query::query, 13},
        {Query::query, 7},
        {Query::color, 5},
        {Query::query, 5},
        {Query::query, 13},
        {Query::query, 7},
    };
}


int main()
{
//    test(get_sample_tree_1(), get_sample_queries_1());
    test(get_sample_tree_2(), get_sample_queries_2());
}