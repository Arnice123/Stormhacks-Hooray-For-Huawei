#include "mats.h"
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <limits>

namespace {

long long g_last_peak_mem = 0;
long long g_last_total_time = 0;
bool g_have_last_stats = false;

struct Scheduler {
    Scheduler(const std::vector<Node>& nodes,
              const Topology& topo,
              long long total_memory)
        : nodes_(nodes),
          topo_(topo),
          total_memory_(total_memory),
          consumer_remaining_(topo.consumerCount),
          materialized_(nodes.size(), false),
          logical_executed_(nodes.size(), false),
          in_progress_(nodes.size(), false) {}

    bool ensure(int v) { return ensure_node(v); }

    std::vector<Node> take_order() { return std::move(order_); }

    long long peak_mem() const { return peak_mem_; }
    long long total_time() const { return total_time_; }

private:
    bool ensure_node(int v) {
        if (materialized_[v]) {
            return true;
        }
        if (in_progress_[v]) {
            throw std::runtime_error("Cycle detected while scheduling");
        }

        in_progress_[v] = true;

        for (int u : topo_.inputs_idx[v]) {
            if (!ensure_node(u)) {
                in_progress_[v] = false;
                return false;
            }
        }

        std::vector<char> protected_flags(nodes_.size(), false);
        protected_flags[v] = true;
        for (int u : topo_.inputs_idx[v]) {
            protected_flags[u] = true;
        }
        for (std::size_t i = 0; i < in_progress_.size(); ++i) {
            if (in_progress_[i]) {
                protected_flags[i] = true;
            }
        }

        const bool first_time = !logical_executed_[v];
        long long freed = 0;
        if (first_time) {
            for (int u : topo_.inputs_idx[v]) {
                if (consumer_remaining_[u] <= 0) {
                    in_progress_[v] = false;
                    return false;
                }
                if (consumer_remaining_[u] == 1 && materialized_[u]) {
                    freed += nodes_[u].getOutputMem();
                }
            }
        }

        if (!ensure_capacity(v, freed, protected_flags)) {
            in_progress_[v] = false;
            return false;
        }

        if (first_time) {
            for (int u : topo_.inputs_idx[v]) {
                consumer_remaining_[u] -= 1;
                if (consumer_remaining_[u] == 0 && materialized_[u]) {
                    live_mem_ -= nodes_[u].getOutputMem();
                    materialized_[u] = false;
                }
            }
            logical_executed_[v] = true;
        }

        live_mem_ += nodes_[v].getOutputMem();
        materialized_[v] = true;
        peak_mem_ = std::max(peak_mem_, live_mem_);
        total_time_ += nodes_[v].getTimeCost();
        order_.push_back(nodes_[v]);

        in_progress_[v] = false;
        return true;
    }

    bool ensure_capacity(int v, long long freed, std::vector<char>& protected_flags) {
        const long long run_mem = nodes_[v].getRunMem();
        const long long out_mem = nodes_[v].getOutputMem();

        while (true) {
            const long long during = live_mem_ + run_mem;
            const long long after = live_mem_ + out_mem - freed;
            if (during <= total_memory_ && after <= total_memory_) {
                peak_mem_ = std::max(peak_mem_, std::max(during, after));
                return true;
            }

            int victim = select_evict(protected_flags);
            if (victim == -1) {
                return false;
            }

            live_mem_ -= nodes_[victim].getOutputMem();
            materialized_[victim] = false;
        }
    }

    int select_evict(const std::vector<char>& protected_flags) {
        int best_idx = -1;
        long long best_mem = -1;
        bool best_is_dead = false;

        for (int i = 0; i < static_cast<int>(nodes_.size()); ++i) {
            if (!materialized_[i] || protected_flags[i]) {
                continue;
            }
            const long long out_mem = nodes_[i].getOutputMem();
            if (out_mem <= 0) {
                continue;
            }
            const bool is_dead = consumer_remaining_[i] == 0;
            if (!best_is_dead && is_dead) {
                best_is_dead = true;
                best_idx = i;
                best_mem = out_mem;
            } else if (is_dead == best_is_dead && out_mem > best_mem) {
                best_idx = i;
                best_mem = out_mem;
            }
        }
        return best_idx;
    }

    const std::vector<Node>& nodes_;
    const Topology& topo_;
    long long total_memory_;
    std::vector<int> consumer_remaining_;
    std::vector<bool> materialized_;
    std::vector<bool> logical_executed_;
    std::vector<bool> in_progress_;
    long long live_mem_ = 0;
    long long peak_mem_ = 0;
    long long total_time_ = 0;
    std::vector<Node> order_;
};

} // namespace

Topology build_topology(const std::vector<Node>& nodes) {
    const int N = static_cast<int>(nodes.size());
    Topology T;
    T.inDeg.assign(N, 0);
    T.consumerCount.assign(N, 0);
    T.inputs_idx.assign(N, {});
    T.consumers.assign(N, {});

    std::unordered_map<int, int> id_to_index;
    id_to_index.reserve(N);
    for (int i = 0; i < N; ++i) {
        id_to_index[nodes[i].getId()] = i;
    }

    for (int v = 0; v < N; ++v) {
        const auto& inNodes = nodes[v].getInputs();
        T.inDeg[v] = static_cast<int>(inNodes.size());
        for (const auto& inNode : inNodes) {
            auto it = id_to_index.find(inNode.getId());
            if (it == id_to_index.end()) {
                throw std::runtime_error("Unknown input id: " + std::to_string(inNode.getId()));
            }
            int u = it->second;
            T.consumerCount[u] += 1;
            T.inputs_idx[v].push_back(u);
            T.consumers[u].push_back(v);
        }
    }
    return T;
}

std::vector<Node> ExecuteOrder(const std::vector<Node>& all_nodes,
                               const std::string& /*output_name*/,
                               long long total_memory) {
    g_have_last_stats = false;

    const int N = static_cast<int>(all_nodes.size());
    if (N == 0) return {};

    Topology T = build_topology(all_nodes);
    Scheduler scheduler(all_nodes, T, total_memory);

    std::vector<int> roots;
    roots.reserve(N);
    for (int i = 0; i < N; ++i) {
        if (T.consumers[i].empty()) {
            roots.push_back(i);
        }
    }
    if (roots.empty()) {
        roots.push_back(N - 1);
    }

    for (int root : roots) {
        if (!scheduler.ensure(root)) {
            return {};
        }
    }

    g_last_peak_mem = scheduler.peak_mem();
    g_last_total_time = scheduler.total_time();
    g_have_last_stats = true;

    return scheduler.take_order();
}

void PrintOrderTimeAndPeak(const std::vector<Node>& order, std::ostream& os)
{
    if (g_have_last_stats) {
        os << "Running time: " << g_last_total_time << "\n"
           << "Peak memory:  " << g_last_peak_mem << "\n";
        g_have_last_stats = false;
        return;
    }

    long long total_time = 0;
    long long live_mem = 0;
    long long peak_mem = 0;

    std::unordered_map<int, int> consumerCount;
    std::unordered_map<int, long long> outMem;
    consumerCount.reserve(order.size() * 2);
    outMem.reserve(order.size() * 2);

    for (const auto& n : order) {
        outMem[n.getId()] = n.getOutputMem();
        for (const auto& in : n.getInputs()) {
            consumerCount[in.getId()] += 1;
        }
    }

    for (const auto& v : order) {
        long long freed = 0;
        for (const auto& in : v.getInputs()) {
            auto it = consumerCount.find(in.getId());
            if (it != consumerCount.end() && --(it->second) == 0) {
                if (auto jt = outMem.find(in.getId()); jt != outMem.end()) {
                    freed += jt->second;
                }
            }
        }

        const long long during = live_mem + v.getRunMem();
        const long long after = live_mem + v.getOutputMem() - freed;

        peak_mem = std::max(peak_mem, std::max(during, after));
        live_mem = after;
        total_time += v.getTimeCost();
    }

    os << "Running time: " << total_time << "\n"
       << "Peak memory:  " << peak_mem << "\n";
}
