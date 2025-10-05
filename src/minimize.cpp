#include "minimize.h"
#include <algorithm>

std::vector<Node> MinimizeRecomputation::ExecuteOrder(const std::vector<Node> &all_nodes, const std::string &output_name,
                                                      long total_memory) {
    // Use unused parameter
    (void) output_name;

    std::vector<Node> nodes = all_nodes;
    long memoryUsage = MinimizeRecomputation::maxMemorySpike(nodes);
    std::map<std::string, long> outputMemories = getNodeOutputMemories(nodes);
    std::map<std::string, Node> nameToNode = getNameToNode(nodes);

    for (auto maxMemUsageNameItr = outputMemories.rbegin();
         memoryUsage > total_memory && maxMemUsageNameItr != outputMemories.rend();
         maxMemUsageNameItr++) {
            
        std::string maxMemUsageName = maxMemUsageNameItr->first;
        Node maxMemUsageNode = nameToNode.at(maxMemUsageName);

        for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
            Node curr = *itr;
            std::vector<Node> currInputs = curr.getInputs();

            auto nodeIsMaxMem = [&] (Node a) {
                return a.getName() == maxMemUsageName;
            };

            if (std::find_if(currInputs.begin(), currInputs.end(), nodeIsMaxMem) != currInputs.end()) {
                nodes.insert(itr, maxMemUsageNode);
            }
        }

        memoryUsage = MinimizeRecomputation::maxMemorySpike(nodes);
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

std::map<std::string, long> MinimizeRecomputation::getNodeOutputMemories(const std::vector<Node> &nodes) {
    std::map<std::string, long> memories;

    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
        Node node = *itr;
        memories[node.getName()] = node.getOutputMem();
    }

    return memories;
}

std::map<std::string, Node> MinimizeRecomputation::getNameToNode(const std::vector<Node> &nodes) {
    std::map<std::string, Node> nameToNode;

    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
        Node node = *itr;
        nameToNode[node.getName()] = node;
    }

    return nameToNode;
}