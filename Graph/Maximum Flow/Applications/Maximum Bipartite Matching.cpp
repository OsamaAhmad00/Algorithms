#include <iostream>
#include <vector>
#include <algorithm>

struct Edge
{
    int from;
    int to;
    int weight;
};

class Graph : public std::vector<std::vector<int>>
{
    std::vector<std::vector<int>> weights;

public:

    explicit Graph(int n) : std::vector<std::vector<int>>(n),
        weights(n, std::vector<int>(n, 0)) {}

    void add_weight(int from, int to, int weight) {
        if (weights[from][to] == 0) {
            (*this)[from].push_back(to);
        }
        weights[from][to] += weight;
    }

    int get_weight(int from, int to) const { return weights[from][to]; }
};

struct SinkNodeDescriptor
{
    int index;
    // total number of nodes
    // this node can match with.
    int total_matches;
};

struct SourceNodeDescriptor : public SinkNodeDescriptor
{
    struct Edge
    {
        int to;
        // the count of the maximum matches
        // allowed with the node "to".
        int maximum_matches;
    };

    std::vector<Edge> matchable_nodes;
};

typedef std::vector<SourceNodeDescriptor> Sources;
typedef std::vector<SinkNodeDescriptor> Sinks;

class MaxFlowCalculator
{
    int n;
    int source, sink;
    Sources sources;
    Sinks sinks;
    Graph residual_graph;
    std::vector<bool> visited;

    int get_flow_value(int from, int to) const
    {
        return residual_graph.get_weight(to, from);
    }

    int add_augmenting_path(int from, int bottleneck = INT_MAX)
    {
        if (from == sink) {
            return bottleneck;
        }

        int result = 0;
        visited[from] = true;

        for (int to : residual_graph[from])
        {
            if (visited[to]) continue;
            int weight = residual_graph.get_weight(from, to);
            int value = add_augmenting_path(to, std::min(weight, bottleneck));
            if (value > 0) {
                residual_graph.add_weight(from, to, -value);
                residual_graph.add_weight(to, from,  value);
                result = value;
                break;
            }
        }

        visited[from] = false;
        return result;
    };

    std::vector<Edge> get_flow_edges()
    {
        std::vector<Edge> result;

        for (auto &s : sources) {
            for (auto &edge : s.matchable_nodes) {
                int flow = get_flow_value(s.index, edge.to);
                if (flow > 0) {
                    result.push_back({s.index, edge.to, flow});
                }
            }
        }

        return result;
    }

    void compute_residual_graph()
    {
        residual_graph = Graph(n + 2);

        for (auto &s : sources) {
            residual_graph.add_weight(source, s.index, s.total_matches);
            for (auto &edge : s.matchable_nodes) {
                residual_graph.add_weight(s.index, edge.to, edge.maximum_matches);
            }
        }

        for (auto &s : sinks) {
            residual_graph.add_weight(s.index, sink, s.total_matches);
        }

        while (add_augmenting_path(source) != 0);
    }

public:

    // The idea here is similar to the multi-source multi-sink problem.
    // we consider one of the sets in the bipartite graph to be the sources
    // and the other set to be the sinks.
    // Each edge weight has a meaning here:
    //  - Weight of an edge from the "super-source" to a "source node"
    //    is the maximum matches that this source node can make.
    //  - Weight of an edge from the "source node" to a "sink node" is
    //    the maximum matches this source node can make with this sink node.
    //  - Weight of an edge from the "super-sink" to a "sink node"
    //    node is the maximum matches that this sink node can make.

    MaxFlowCalculator(const Sources& sources, const Sinks& sinks) :
        sources(sources), sinks(sinks), residual_graph({})
    {
        n = sources.size() + sinks.size();
        source = n;
        sink = n + 1;
        visited.resize(n + 2, false);
    }

    std::vector<Edge> get_max_flow()
    {
        compute_residual_graph();
        return get_flow_edges();
    }
};

Sources get_sample_sources_1()
{
    Sources sources(5);

    sources[0].index = 0;
    sources[0].total_matches = 4;
    sources[0].matchable_nodes = {{5, 2}, {6, 2}};
    sources[1].index = 1;
    sources[1].total_matches = 3;
    sources[1].matchable_nodes = {{6, 1}, {7, 1}};
    sources[2].index = 2;
    sources[2].total_matches = 2;
    sources[2].matchable_nodes = {{7, 2}, {8, 2}};
    sources[3].index = 3;
    sources[3].total_matches = 3;
    sources[3].matchable_nodes = {{8, 1}, {9, 1}};
    sources[4].index = 4;
    sources[4].total_matches = 4;
    sources[4].matchable_nodes = {{9, 3}};

    return sources;
}

Sinks get_sample_sinks_1()
{
    Sinks sinks(5);
    sinks[0] = {5, 3};
    sinks[1] = {6, 3};
    sinks[2] = {7, 2};
    sinks[3] = {8, 3};
    sinks[4] = {9, 3};
    return sinks;
}

void test(const Sources &sources, const Sinks &sinks)
{
    int total_matches = 0;
    auto edges = MaxFlowCalculator(sources, sinks).get_max_flow();
    for (Edge& edge : edges)
    {
        std::cout << edge.from << " --" << edge.weight;
        if (edge.weight < 10) std::cout << ' ';
        std::cout << "--> " <<  edge.to << std::endl;

        for (auto &s : sinks) {
            if (s.index == edge.to) {
                total_matches += edge.weight;
            }
        }
    }
    std::cout << "Total Matches: " << total_matches << std::endl << std::endl;
}

int main()
{
    test(get_sample_sources_1(), get_sample_sinks_1());
}