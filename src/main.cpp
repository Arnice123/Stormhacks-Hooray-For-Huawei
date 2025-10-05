#include <iostream>
#include "greedy_toposearch.h"
#include "parser.h"

int main(int argc, char** argv){
    ParseResult r = parse_file(argv[1]);

    TopoSearch t;
    std::vector<Node> result = t.ExecuteOrder(r.nodes, r.output_name, r.max_memory);

    long totalTime = 0;
    for (int i = 0; i < result.size(); ++i)
    {
        totalTime += result[i].getTimeCost();
        //std::cout << "Ran: " << result[i].getName() << " Time: " << result[i].getTimeCost() << "\n";
    }
    std::cout << "Total time: " << totalTime << " Total Count: " << result.size() <<  "\n";
}