#pragma once
#include <string>
#include <vector>
#include <istream>
#include "node.h"

// Container for the entire parsed file.
struct ParseResult {
    int max_memory = 0;          // From the first line of the file
    std::vector<Node> nodes;     // All operator nodes
    std::string output_name;
};

// Parse the file and return both the node list and memory cap.
ParseResult parse_file(const std::string& path);
ParseResult parse_stream(std::istream& is);