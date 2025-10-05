/**
 * !!! Change "recomp_algo.h" to your own algo (You will probably need to change the function name "executeOrder" aswell)
 * This program tests that our ExecuteOrder function runs properly.
 * 
 * It will take parsed test cases from Parser(...), Run ExecuteOrder(...), and Display the results of the test
 */
#include "recomp_algo.h"
#include <iostream>

int exec_Test(const std::vector<Node> &result, const std::string &output_Name);
bool exec_Has_Dependencies(const std::vector<Node> &inputs, const std::vector<Node> &nodes_In_Mem);

int main() {
    const std::string sm_Test_1 = "data/example1.txt";
    ParseResult sm_Test_Data = parse_file(sm_Test_1);
    
    std::vector<Node> small_Test_1_Solution = executeOrder(sm_Test_Data.nodes, sm_Test_Data.output_name, sm_Test_Data.max_memory);

    printf("Total time small_test_1: %d", exec_Test(small_Test_1_Solution, sm_Test_Data.output_name));
}


/**
 *  This function will track current executing node and total execution time.
 *  
 *  params:
 *  const std::vector<Node> &result - The optimized execution order
 *  const std::string &output_name - The final node that should be computed
 *  
 *  returns:
 *  int total_time - The time it takes to execute the DAG
 */
int exec_Test(const std::vector<Node> &result, const std::string &output_name) {
    int total_Time = 0;
    unsigned long index = 0;
    /*
    I get the address of vector, i need to iterate through 
    */
    while(result[index].getName() != output_name ) {
        std::cout << "Execute operator " << result[index].getName() << "Total execution time: " << (total_Time += result[index].getTimeCost());
        index++;
    }
    return total_Time;
}