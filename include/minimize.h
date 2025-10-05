#include "node.h"

class MinimizeRecomputation {
    public:
        /// @brief Takes in a list of Nodes in a valid order and a maximum amount of memory, and returns a list of Nodes in a
        ///        valid order that uses no more than @param total_memory while still being quick
        /// @param all_nodes List of Nodes in valid order
        /// @param output_name no clue
        /// @param total_memory Maximum memory usage
        /// @return List of Nodes in a valid order that uses no more than @param total_memory
        std::vector<Node> ExecuteOrder(const std::vector<Node> &all_nodes, const std::string &output_name,
                                    long total_memory);
    private:
        /// @brief Returns the maximum amount of memory a given sequence of Nodes uses to compute.
        /// @param nodes List of nodes.
        /// @return Maximum amount of memory a given sequence of Nodes uses to compute.
        long maxMemorySpike(const std::vector<Node> &nodes);
};