#include "minimize.h"

#include "map"

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
    long max = 0;
    long curr = 0;
    std::map<std::string, bool> nodesInMemory;
    for (auto riter = nodes.rbegin(); riter != nodes.rend(); ++riter) {
        Node currNode = *riter;
        if (!nodesInMemory[currNode.getName()]) {
            curr += currNode.getOutputMem();
            nodesInMemory[currNode.getName()] = true;
        }

        std::vector<Node> currNodeInputs = currNode.getInputs();
        for (auto iter2 = currNodeInputs.begin(); iter2 != currNodeInputs.end(); iter2++) {
            Node currInputNode = *iter2;

            if (!nodesInMemory[currInputNode.getName()]) {
                curr += currInputNode.getOutputMem();
                nodesInMemory[currInputNode.getName()] = true;
            }
        }

        if (curr > max) {
            max = curr;
        }
        curr -= currNode.getOutputMem();
        nodesInMemory[currNode.getName()] = false;
    }
    return max;
}