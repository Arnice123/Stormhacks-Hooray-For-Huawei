#include <iostream>
#include <string>
#include <vector>
#include "parser.h"
#include "node.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    try {
        ParseResult r = parse_file(argv[1]);

        std::cout << "Max memory: " << r.max_memory << "\n";
        std::cout << "Graph output: " 
                  << (r.output_name.empty() ? "(unset)" : r.output_name) << "\n";
        std::cout << "Parsed " << r.nodes.size() << " nodes\n\n";

        for (size_t i = 0; i < r.nodes.size(); ++i) {
            const Node& n = r.nodes[i];
            std::cout << i << ": " << n.getName() << "\n  Inputs: ";
            const auto& ins = n.getInputs();
            if (ins.empty()) {
                std::cout << "(none)";
            } else {
                for (size_t j = 0; j < ins.size(); ++j) {
                    std::cout << ins[j].getName();
                    if (j + 1 < ins.size()) std::cout << ", ";
                }
            }
            std::cout << "\n  Run Mem: "    << n.getRunMem()
                      << "\n  Output Mem: " << n.getOutputMem()
                      << "\n  Time Cost: "  << n.getTimeCost()
                      << "\n----------------------------------\n";
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
