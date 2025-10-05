#include "node.h"
#include <map>

class MinimizeRecomputation {
    public:
        /// @brief Takes in a list of Nodes in a valid order and a maximum amount of memory, and returns a list of Nodes in a
        ///        valid order that uses no more than @param total_memory while still being quick
        /// @param all_nodes List of Nodes in valid order
        /// @param output_name Unclear
        /// @param total_memory Maximum memory usage
        /// @return List of Nodes in a valid order that uses no more than @param total_memory
        static std::vector<Node> ExecuteOrder(const std::vector<Node> &all_nodes, const std::string &output_name,
                                    long total_memory);
    // private:
        /// @brief Returns the maximum amount of memory a given sequence of Nodes uses to compute
        /// @param nodes List of nodes
        /// @return Maximum amount of memory a given sequence of Nodes uses to compute
        static long maxMemorySpike(const std::vector<Node> &nodes);

        /// @brief Returns a map of node names to their output memories
        /// @param nodes The list of nodes to add to the map
        /// @return A map of node names to their output memories
        static std::map<std::string, long> getNodeOutputMemories(const std::vector<Node> &nodes);

        /// @brief Returns a map of node names to their Node objects
        /// @param nodes The list of nodes to add to the map
        /// @return A map of node names to their Node objects
        static std::map<std::string, Node> getNameToNode(const std::vector<Node> &nodes);
};