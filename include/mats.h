#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "node.h"

struct Topology {
    std::vector<int> inDeg;
    std::vector<int> consumerCount;
    std::vector<std::vector<int>> inputs_idx;
    std::vector<std::vector<int>> consumers;
};

Topology build_topology(const std::vector<Node>& nodes);

std::vector<int> ExecuteOrder(const std::vector<Node>& all_nodes,
                              const std::string& output_name,
                              long long total_memory);

void PrintOrderTimeAndPeak(const std::vector<Node>& nodes,
                           const std::vector<int>& order,
                           std::ostream& os,
                           bool show_peak);
