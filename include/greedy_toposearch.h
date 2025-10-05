#include "node.h"

class TopoSearch {
public:
    std::vector<Node> ExecuteOrder(const std::vector<Node> &all_nodes, const std::string &output_name, long total_memory);

private:
    struct topologicalInfo {
        Node node;
        std::vector<int> adj;
        int indegrees = 0;
        int future_use_count = 0;
    };

    struct ReadyNodeInfo {
        int index;
        const Node& node;
        double ratio;
        bool operator>(const ReadyNodeInfo &other) const {
            return ratio > other.ratio;
        }
    };

    std::vector<topologicalInfo> preProccess(const std::vector<Node> &all_nodes);
    std::vector<Node> TopoSearch::kahnAlgorithm(std::vector<topologicalInfo> info);

    /*
    std::vector<std::vector<int>> createAdjList(const std::vector<Node> &all_nodes);
    std::vector<int> computeIndegrees(const std::vector<Node> &all_nodes);
    std::vector<Node> kahnAlgorithm(const std::vector<Node> &all_nodes, std::vector<std::vector<int>> adjList, std::vector<int> indegrees);
    std::vector<int> computeCountOfDependeces(const std::vector<std::vector<int>> & adjList);*/
};


