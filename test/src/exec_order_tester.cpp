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

