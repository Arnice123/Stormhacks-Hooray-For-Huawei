#include "minimize.h"

std::vector<Node> MinimizeRecomputation::ExecuteOrder(const std::vector<Node> &all_nodes, const std::string &output_name,
                               long total_memory) {
    // Use unused parameter
    (void) output_name;

    std::vector<Node> nodes = all_nodes;
    long memoryUsage = MinimizeRecomputation::maxMemorySpike(nodes);
    while (memoryUsage > total_memory) {
        
    }
    
    return nodes;
}

long MinimizeRecomputation::maxMemorySpike(const std::vector<Node> &nodes) {
    return 0;
}