#include "greedy_toposearch.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <queue>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <limits>
#include <sstream>
#include <cctype>
#include <limits>

void debugGraphInfo(const std::vector<TopoSearch::topologicalInfo> &graph_info)
{
    //std::cout << "\n=== GRAPH DEBUG INFO ===" << std::endl;
    for (int i = 0; i < graph_info.size(); ++i)
    {
        const auto &info = graph_info[i];
        /*std::cout << "Node " << i << ": " << info.node.getName()
                  << " indegree=" << info.indegrees
                  << " adj_size=" << info.adj.size()
                  << " inputs: ";*/

        for (const Node &input : info.node.getInputs())
        {
            //std::cout << input.getName() << " ";
        }
        //std::cout << std::endl;
    }
    //std::cout << "========================\n"<< std::endl;
              
}

std::vector<Node> TopoSearch::ExecuteOrder(const std::vector<Node> &all_nodes, const std::string &output_name, long total_memory)
{

    std::vector<Node> all_nodes_mut = all_nodes;
    int j = all_nodes_mut.size();

    std::vector<TopoSearch::topologicalInfo> graph_info = preProccess(all_nodes_mut);

    //debugGraphInfo(graph_info);

    std::vector<Node> result;
    long memory_in_use = 0;
    long max_memory_in_use = 0;

    int n = graph_info.size();

    std::priority_queue<ReadyNodeInfo, std::vector<ReadyNodeInfo>, std::greater<ReadyNodeInfo>> ready_queue;

    std::unordered_map<int, TopoSearch::topologicalInfo> active_outputs;
    std::unordered_set<int> computed_nodes;

    std::vector<bool> values(n ,0);

    std::vector<int> initial_indegrees(n);
    for (int i = 0; i < n; ++i)
    {
        initial_indegrees[i] = graph_info[i].indegrees;
    }

    for (int i = 0; i < n; ++i)
    {
        if (graph_info[i].indegrees == 0)
        {
            const Node &node = graph_info[i].node;
            double ratio = static_cast<double>(node.getTimeCost()) / node.getOutputMem();
            ready_queue.push({i, ratio});
        }
    }

    while (!ready_queue.empty())
    {
        ReadyNodeInfo current = ready_queue.top();
        ready_queue.pop();

        values[current.index] = true;

        const Node &node = graph_info[current.index].node;

        long required_mem = node.getRunMem() + node.getOutputMem();

        if (memory_in_use + required_mem > total_memory)
        {
            std::vector<std::pair<int, double>> eviction_candidates;

            for (auto &kv : active_outputs)
            {
                const auto &info = kv.second;
                const Node &output_node = info.node;

                double score = static_cast<double>(info.future_use_count) /
                               (static_cast<double>(output_node.getOutputMem()) * static_cast<double>(output_node.getTimeCost()));

                eviction_candidates.push_back({kv.first, score});
            }

            std::sort(eviction_candidates.begin(), eviction_candidates.end(),
                      [](const auto &a, const auto &b)
                      { return a.second < b.second; });

            for (const auto &pair : eviction_candidates)
            {
                int idx = pair.first;
                const Node &evicted = active_outputs[idx].node;
                memory_in_use -= evicted.getOutputMem();
                active_outputs.erase(idx);

                if (memory_in_use + required_mem <= total_memory)
                    break;
            }

            if (memory_in_use + required_mem > total_memory)
            {
                std::cerr << "Warning: Not enough memory to execute node " << node.getName() << std::endl;
                continue;
            }
        }

        memory_in_use += node.getOutputMem();

        if (memory_in_use > max_memory_in_use)
        {
            max_memory_in_use = memory_in_use;
        }

        int future_uses = 0;
        for (int dep : graph_info[current.index].adj)
        {
            if (computed_nodes.count(dep) == 0)
            {
                future_uses++;
            }
        }
        graph_info[current.index].future_use_count = future_uses;

        active_outputs[current.index] = graph_info[current.index];

        result.push_back(node);
        computed_nodes.insert(current.index);

        std::unordered_set<std::string> input_names;
        for (const Node &input : node.getInputs())
        {
            input_names.insert(input.getName());
        }

        for (auto it = active_outputs.begin(); it != active_outputs.end();)
        {
            if (input_names.count(it->second.node.getName()))
            {
                it->second.future_use_count--;

                if (it->second.future_use_count <= 0)
                {
                    memory_in_use -= it->second.node.getOutputMem();
                    it = active_outputs.erase(it);
                    continue;
                }
            }
            ++it;
        }

        for (int dep : graph_info[current.index].adj)
        {
            if (graph_info[dep].indegrees > 0)
            {
                graph_info[dep].indegrees--;
                if (graph_info[dep].indegrees == 0)
                {
                    const Node &dep_node = graph_info[dep].node;
                    double ratio = static_cast<double>(dep_node.getTimeCost()) / dep_node.getOutputMem();
                    ready_queue.push({dep, ratio});
                    values[dep] = true;

                }
            }
        }

        for (int i = 0; i < n; ++i)
        {
            if (computed_nodes.count(i) == 0 && graph_info[i].indegrees == 0 && !values[i])
            {
                const Node &dep_node = graph_info[i].node;
                double ratio = static_cast<double>(dep_node.getTimeCost()) / dep_node.getOutputMem();
                ready_queue.push({i, ratio});
                values[i] = true;
            }
        }
    }
    return result;
}

std::vector<TopoSearch::topologicalInfo> TopoSearch::preProccess(const std::vector<Node> &all_nodes)
{
    int n = all_nodes.size();
    std::vector<TopoSearch::topologicalInfo> info(n);

    std::unordered_map<std::string, int> nameToIndex;

    for (int i = 0; i < n; ++i)
    {
        info[i].node = all_nodes[i];
        nameToIndex[all_nodes[i].getName()] = i;
    }

    for (int i = 0; i < n; ++i)
    {
        for (const Node &input : info[i].node.getInputs())
        {
            auto it = nameToIndex.find(input.getName());
            if (it != nameToIndex.end())
            {
                info[it->second].adj.push_back(i);
                info[i].indegrees++;
            }
        }
    }

    for (int i = 0; i < info.size(); ++i)
    {
        info[i].future_use_count = info[i].adj.size();
    }

    return info;
}

std::vector<Node> TopoSearch::kahnAlgorithm(std::vector<TopoSearch::topologicalInfo> info)
{
    int n = info.size();

    std::queue<int> q;
    for (int i = 0; i < n; ++i)
    {
        if (info[i].indegrees == 0)
        {
            q.push(i);
        }
    }

    std::vector<Node> topological_order;
    while (!q.empty())
    {
        int u_index = q.front();
        q.pop();
        topological_order.push_back(info[u_index].node);

        for (int v_index : info[u_index].adj)
        {
            --info[v_index].indegrees;

            if (info[v_index].indegrees == 0)
            {
                q.push(v_index);
            }
        }
    }

    if (topological_order.size() != n)
    {
        return {}; // cycle found
    }

    return topological_order;
}
