#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "parser.h"
#include "mats.h"

int main(int argc, char** argv){
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [--summary] [--peak]\n";
        return 1;
    }

    bool summary_mode = false;
    bool show_peak = false;
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--summary") {
            summary_mode = true;
        } else if (arg == "--peak") {
            show_peak = true;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            return 1;
        }
    }

    ParseResult r = parse_file(argv[1]);

    std::vector<int> order = ExecuteOrder(r.nodes, r.output_name, r.max_memory);
    if (order.empty()) {
        std::cerr << "Schedule infeasible (empty order).\n";
        return 2;
    }

    std::string output_path = std::string(argv[1]) + ".order.txt";
    std::ofstream ofs(output_path);
    if (!ofs) {
        std::cerr << "Failed to open output file: " << output_path << "\n";
        return 1;
    }

    for (std::size_t i = 0; i < order.size(); ++i) {
        int idx = order[i];
        const Node& n = r.nodes[idx];
        ofs << i << ": id=" << n.getId() << " name=" << n.getName() << "\n";
    }

    std::cout << "First op: " << r.nodes[order.front()].getName() << "\n";
    std::cout << "Schedule order (" << order.size() << " nodes) written to " << output_path << "\n";

    if (!summary_mode) {
        const std::size_t kPrintLimit = 20;
        const std::size_t to_print = std::min(kPrintLimit, order.size());
        for (std::size_t i = 0; i < to_print; ++i) {
            int idx = order[i];
            const Node& n = r.nodes[idx];
            std::cout << i << ": id=" << n.getId() << " name=" << n.getName() << "\n";
        }
        if (order.size() > kPrintLimit) {
            std::cout << "... (" << order.size() - kPrintLimit << " more entries written to file)\n";
        }
    } else {
        std::cout << "(node listing suppressed in summary mode; see file)\n";
    }

    std::cout << "Optimized schedule metrics:\n";
    PrintOrderTimeAndPeak(r.nodes, order, std::cout, show_peak);

    if (!summary_mode) {
        std::vector<int> default_order(r.nodes.size());
        for (std::size_t i = 0; i < r.nodes.size(); ++i) default_order[i] = static_cast<int>(i);
        std::cout << "Default order metrics:\n";
        PrintOrderTimeAndPeak(r.nodes, default_order, std::cout, show_peak);
    } else {
        std::cout << "Default order metrics: (skipped in summary mode)\n";
    }

    return 0;
}
