#pragma once
#include <string>
#include <vector>

class Node {
private:
    int id_;
    std::string name_;
    std::vector<int> input_ids_;
    long long run_mem_;
    long long output_mem_;
    long long time_cost_;

public:
    Node()
        : id_(-1), name_(), input_ids_(), run_mem_(0), output_mem_(0), time_cost_(0) {}

    Node(int id,
         std::string name,
         std::vector<int> input_ids,
         long long run_mem,
         long long output_mem,
         long long time_cost)
        : id_(id),
          name_(std::move(name)),
          input_ids_(std::move(input_ids)),
          run_mem_(run_mem),
          output_mem_(output_mem),
          time_cost_(time_cost) {}

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::vector<int>& getInputIds() const { return input_ids_; }
    long long getRunMem() const { return run_mem_; }
    long long getOutputMem() const { return output_mem_; }
    long long getTimeCost() const { return time_cost_; }

    void setId(int id) { id_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setInputIds(const std::vector<int>& inputs) { input_ids_ = inputs; }
    void setInputIds(std::vector<int>&& inputs) { input_ids_ = std::move(inputs); }
    void setRunMem(long long run_mem) { run_mem_ = run_mem; }
    void setOutputMem(long long output_mem) { output_mem_ = output_mem; }
    void setTimeCost(long long time_cost) { time_cost_ = time_cost; }
};
