#include "parser.h"
#include "node.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <cctype>
#include <vector>
#include <string>
#include <memory>

// ---------- helpers ----------
static bool isCommentOrBlank(const std::string& s) {
    bool seen_non_space = false;
    for (char ch : s) {
        if (ch == '#') return true;                    // treat as comment line
        if (!std::isspace(static_cast<unsigned char>(ch)))
            seen_non_space = true;
    }
    return !seen_non_space; // true if blank, false otherwise
}

static std::vector<std::string> splitTokens(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> toks;
    std::string t;
    while (iss >> t) toks.push_back(t);
    return toks;
}

// ---------- main parse ----------
ParseResult parseFile(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::string line;

    // 1) Read total memory (first non-blank, non-comment line)
    long totalMem = -1;
    while (std::getline(fin, line)) {
        if (isCommentOrBlank(line)) continue;
        auto toks = splitTokens(line);
        if (toks.empty()) continue;
        try {
            totalMem = std::stol(toks[0]);
        } catch (...) {
            throw std::runtime_error("First non-comment line must be the total memory limit");
        }
        break;
    }
    if (totalMem < 0) {
        throw std::runtime_error("Missing total memory line at top of file");
    }

    // 2) Single-pass: create nodes in order, link inputs immediately (topological order assumed)
    std::unordered_map<std::string, std::shared_ptr<Node>> byName;
    std::unordered_map<std::string, int> consumerCount; // counts how many consumers each node has
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::string> order;
    byName.reserve(1024);
    consumerCount.reserve(1024);

    while (std::getline(fin, line)) {
        if (isCommentOrBlank(line)) continue;
        auto toks = splitTokens(line);
        if (toks.size() < 4) {
            throw std::runtime_error("Each node line must be: name run_mem output_mem time_cost [inputs...]");
        }

        const std::string& name = toks[0];
        if (byName.count(name)) {
            throw std::runtime_error("Duplicate node definition for: " + name);
        }

        int runMem   = 0;
        int outMem   = 0;
        int timeCost = 0;
        try {
            runMem   = std::stoi(toks[1]);
            outMem   = std::stoi(toks[2]);
            timeCost = std::stoi(toks[3]);
        } catch (...) {
            throw std::runtime_error("Bad numeric field for node: " + name);
        }

        auto node = std::make_shared<Node>(name, runMem, outMem, timeCost);

        // Link inputs immediately (must already exist because of topological order)
        auto& in = node->inputs();
        for (size_t i = 4; i < toks.size(); ++i) {
            const std::string& dep = toks[i];
            auto it = byName.find(dep);
            if (it == byName.end()) {
                throw std::runtime_error(
                    "Forward reference not allowed (file must be in topological order): " + dep +
                    " (referenced by " + name + ")"
                );
            }
            in.push_back(it->second);
            consumerCount[dep] += 1; // record that 'dep' has one more consumer
        }

        // Insert node
        byName.emplace(name, node);
        if (!consumerCount.count(name)) consumerCount[name] = 0; // ensure present with default 0
        order.push_back(name);
        nodes.push_back(std::move(node));
    }

    if (nodes.empty()) {
        throw std::runtime_error("No nodes found in file after memory line");
    }

    // 3) Infer output node(s): nodes with zero consumers
    std::vector<std::shared_ptr<Node>> outputs;
    outputs.reserve(2);
    for (const auto& nname : order) {
        if (consumerCount[nname] == 0) {
            outputs.push_back(byName[nname]);
        }
    }
    if (outputs.empty()) {
        throw std::runtime_error("No output node found (cycle? or every node consumed by another?)");
    }

    // If multiple zero-consumer nodes, pick the last defined for determinism
    std::shared_ptr<Node> chosen_output = outputs.back();

    // 4) Build result
    ParseResult res;
    res.total_memory = totalMem;
    res.nodes        = std::move(nodes);
    res.output       = std::move(chosen_output);
    return res;
}
