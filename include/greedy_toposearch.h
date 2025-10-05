#include "node.h"

class TopoSearch {
public:
    std::vector<Node> ExecuteOrder(const std::vector<Node> &all_nodes, const std::string &output_name, long total_memory);

    struct topologicalInfo {
        Node node;
        std::vector<int> adj;
        int indegrees = 0;
        int future_use_count = 0;
    };

    struct ReadyNodeInfo {
        int index;
        double ratio;        
        bool operator>(const ReadyNodeInfo &other) const {
            return ratio > other.ratio;
        }
    };

private:
    std::vector<TopoSearch::topologicalInfo> preProccess(const std::vector<Node> &all_nodes);
    std::vector<Node> kahnAlgorithm(std::vector<TopoSearch::topologicalInfo> info);
};