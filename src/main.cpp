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

    std::vector<Node> order = ExecuteOrder(r.nodes, r.output_name, r.max_memory);
    if (order.empty()) {
        std::cerr << "Schedule infeasible (empty order).\n";
        return 2;
    }

    std::cout << "First op: " << order.front().getName() << "\n";
    PrintOrderTimeAndPeak(order, std::cout);
    return 0;
}
