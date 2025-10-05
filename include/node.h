#pragma once
#include <string>
#include <vector>
#include <memory>

class Node {
public:
    // Constructor
    Node(std::string name, int runMem, int outputMem, int timeCost)
        : name_(std::move(name)),
          run_mem_(runMem),
          output_mem_(outputMem),
          time_cost_(timeCost) {}

    // getter
    const std::string& name() const { return name_; }
    int runMem() const { return run_mem_; }
    int outputMem() const { return output_mem_; }
    int timeCost() const { return time_cost_; }
    std::vector<std::shared_ptr<Node>>& inputs() { return inputs_; }

    // setters
    void setRunMem(int runMem) { run_mem_ = runMem; }
    void setOutputMem(int outputMem) { output_mem_ = outputMem; }
    void setTimeCost(int cost) { time_cost_ = cost; }
    const std::vector<std::shared_ptr<Node>>& inputs() const { return inputs_; }
    
    

private:
    std::string name_;
    std::vector<std::shared_ptr<Node>> inputs_;
    int run_mem_;
    int output_mem_;
    int time_cost_;
};