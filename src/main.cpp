#include <iostream>
#include <vector>
#include "parser.h"
#include "mats.h"

int main(int argc, char** argv){
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    ParseResult r = parse_file(argv[1]);

    std::vector<int> order = ExecuteOrder(r.nodes, r.output_name, r.max_memory);
    if (order.empty()) {
        std::cerr << "Schedule infeasible (empty order).\n";
        return 2;
    }

    std::cout << "First op: " << r.nodes[order.front()].getName() << "\n";
    std::cout << "Schedule order (" << order.size() << " nodes):\n";

    const std::size_t kPrintLimit = 20;
    const std::size_t to_print = std::min(kPrintLimit, order.size());
    for (std::size_t i = 0; i < to_print; ++i) {
        int idx = order[i];
        const Node& n = r.nodes[idx];
        std::cout << i << ": id=" << n.getId() << " name=" << n.getName() << "\n";
    }
    if (order.size() > kPrintLimit) {
        std::cout << "... (" << order.size() - kPrintLimit << " more entries)\n";
    }

    PrintOrderTimeAndPeak(r.nodes, order, std::cout);
    return 0;
}
