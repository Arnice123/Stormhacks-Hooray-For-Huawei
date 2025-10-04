#pragma once
#include <memory>
#include <string>
#include <vector>

class Node;

struct ParseResult {
    long total_memory;
    std::vector<std::shared_ptr<Node>> nodes;
    std::shared_ptr<Node> output;
};

ParseResult parseFile(const std::string& filename);
