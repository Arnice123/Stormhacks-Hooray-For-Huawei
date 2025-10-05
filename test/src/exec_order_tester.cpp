/**
 * This program tests that our ExecuteOrder function runs properly.
 * 
 * It will take parsed test cases from Parser(...), Run ExecuteOrder(...), and Display the results of the test
 */
#include "node.h"
#include "parser.h"
#include <iostream>

int exec_Test(const std::vector<Node> &result, const std::string &output_Name, long total_Memory, int total_Nodes);
int main() {
    const std::string sm_Test_1 = "test/test_cases/example1.txt";
    ParseResult sm_Test_Data = parse_file(sm_Test_1);
    
    std::vector<Node> small_Test_1_Solution = executeOrder(sm_Test_Data.nodes,/*outputName*/,sm_Test_Data.max_memory);

    printf("Total time small_test_1: %d", exec_Test(small_Test_1_Solution, /* outputName*/, sm_Test_Data.max_memory, sm_Test_Data.nodes.size()));
}
/**
 *  This function will track current memory usage, current executing node, and total execution time.
 *  It will also confirm if the execution of a node is legal and if the current memory usage stays below max memory.
 *  
 *  params:
 *  const std::vector<Node> &result - The optimized execution order
 *  const std::string &output_name - The final node that should be computed
 *  long total_memory - The maximum memory cap
 *  
 *  returns:
 *  int total_time - The time it takes to execute the DAG
 */
int exec_Test(const std::vector<Node> &result, const std::string &output_name, long total_memory, int total_Nodes) {
    int total_Time = 0;
    int current_Mem_Usage = 0;
    int *current_Mem_nodes[total_Nodes];
    /*
    I get the address of vector, i need to iterate through 
    */

    return total_time;
}
