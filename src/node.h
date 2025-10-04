#include "string"
#include "vector"

class Node {
   private:
    std::string name_;          // The name of the operator node, such as Add, Mul, etc.
    std::vector<Node> inputs_;  // All input nodes of the operator node
    int run_mem_;               // Memory required for the operator's computation process
    int output_mem_;            // Memory occupied by the operator's computation result
    int time_cost_;             // Time taken for the operator's computation
};