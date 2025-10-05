#pragma once
#include <string>
#include <vector>

class Node {
private:
    int id_;                      // Original node id from the input graph
    std::string name_;             // Operator name (e.g., Add, Mul)
    std::vector<Node> inputs_;     // Direct input nodes (copied for now)
    long long run_mem_;            // Memory required during execution
    long long output_mem_;         // Memory occupied by the output tensor
    long long time_cost_;          // Execution time cost

public:
    Node()
        : id_(-1), name_(), inputs_(), run_mem_(0), output_mem_(0), time_cost_(0) {}

    Node(int id,
         const std::string& name,
         const std::vector<Node>& inputs,
         long long run_mem,
         long long output_mem,
         long long time_cost)
        : id_(id),
          name_(name),
          inputs_(inputs),
          run_mem_(run_mem),
          output_mem_(output_mem),
          time_cost_(time_cost) {}

    // Accessors
    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::vector<Node>& getInputs() const { return inputs_; }
    long long getRunMem() const { return run_mem_; }
    long long getOutputMem() const { return output_mem_; }
    long long getTimeCost() const { return time_cost_; }

    // Mutators
    void setId(int id) { id_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setInputs(const std::vector<Node>& inputs) { inputs_ = inputs; }
    void setRunMem(long long run_mem) { run_mem_ = run_mem; }
    void setOutputMem(long long output_mem) { output_mem_ = output_mem; }
    void setTimeCost(long long time_cost) { time_cost_ = time_cost; }
};
