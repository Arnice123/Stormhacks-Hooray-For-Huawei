#include "greedy_toposearch.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <queue>

std::vector<Node> TopoSearch::ExecuteOrder(const std::vector<Node> &all_nodes, const std::string &output_name, long total_memory)
{
    std::vector<TopoSearch::topologicalInfo> graph_info = preProccess(all_nodes);
    std::vector<Node> topoResult = kahnAlgorithm(graph_info);

    std::vector<Node> result;
    int memory_in_use = 0;

    int n = graph_info.size();

    std::priority_queue<ReadyNodeInfo, std::vector<ReadyNodeInfo>, std::greater<ReadyNodeInfo>> ready_queue;

    std::unordered_map<int, TopoSearch::topologicalInfo> active_outputs;

    for (int i = 0; i < n; ++i)
    {
        if (graph_info[i].indegrees == 0)
        {
            const Node &node = graph_info[i].node;
            double ratio = (double)node.timeCost() / node.outputMem();

            ready_queue.push({i, node, ratio});
        }
    }

    while (!ready_queue.empty())
    {
        ReadyNodeInfo current = ready_queue.top();
        ready_queue.pop();

        Node node = current.node;

        long required_mem = node.runMem() + node.outputMem();

        if (memory_in_use + required_mem > total_memory)
        {
            std::vector<std::pair<int, double>> eviction_candidates;
            for (auto &kv : active_outputs)
            {
                const auto &info = kv.second;
                const Node &no = info.node;
                double score = (double)info.future_use_count / ((double)no.outputMem() * (double)no.timeCost());
                eviction_candidates.push_back({kv.first, score});
            }

            std::sort(eviction_candidates.begin(), eviction_candidates.end(), [](auto &a, auto &b)
                      { return a.second < b.second; });

            for (auto &pair : eviction_candidates)
            {
                int idx = pair.first;
                const Node &evicted = active_outputs[idx].node;
                memory_in_use -= evicted.outputMem();
                active_outputs.erase(idx);
                if (memory_in_use + required_mem <= total_memory)
                    break;
            }

            if (memory_in_use + required_mem > total_memory)
            {
                // find the smallest subtree using topograph
                continue;
            }

            memory_in_use+= required_mem;
            active_outputs[current.index] = graph_info[current.index];

            result.push_back(node);

            for (int dep: graph_info[current.index].adj)
            {
                --graph_info[dep].indegrees;
                if (graph_info[dep].indegrees == 0)
                {
                    const Node &node = graph_info[dep].node;
                    double ratio = (double)node.timeCost() / node.outputMem();

                    ready_queue.push({dep, node, ratio});
                }
            }

            for (auto it = active_outputs.begin(); it != active_outputs.end();)
            {
                it->second.future_use_count--;
                if (it->second.future_use_count <= 0)
                {
                    memory_in_use -= it->second.node.outputMem();
                    it = active_outputs.erase(it);
                }
                else
                    ++it;
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
        nameToIndex[all_nodes[i].name()] = i;
    }

    for (int i = 0; i < n; ++i)
    {
        for (const Node &input : info[i].node.inputs())
        {
            auto it = nameToIndex.find(input.name());
            if (it != nameToIndex.end())
            {
                info[it->second].adj.push_back(i);
                ++info[i].indegrees;
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

/*
std::vector<std::vector<int>> TopoSearch::createAdjList(const std::vector<Node> &all_nodes)
{
    int n = all_nodes.size();
    std::unordered_map<std::string, int> nameToIndex;

    for (int i = 0; i < n; ++i)
    {
        nameToIndex[all_nodes[i].name()] = i;
    }

    std::vector<std::vector<int>> adjList(n);

    for (int i = 0; i < n; ++i)
    {
        const Node &node = all_nodes[i];
        for (const Node &input : node.inputs())
        {
            auto it = nameToIndex.find(input.name());
            if (it != nameToIndex.end())
            {
                adjList[it->second].push_back(i);
            }
        }
    }
    return adjList;
}

std::vector<int> TopoSearch::computeIndegrees(const std::vector<Node> &all_nodes)
{
    int n = all_nodes.size();
    std::vector<int> indegrees(n, 0);
    std::unordered_map<std::string, int> nameToIndex;

    for (int i = 0; i < n; ++i)
    {
        nameToIndex[all_nodes[i].name()] = i;
    }

    for (int i = 0; i < n; ++i)
    {
        const Node &node = all_nodes[i];
        for (const Node &input : node.inputs())
        {
            auto it = nameToIndex.find(input.name());
            if (it != nameToIndex.end())
            {
                ++indegrees[i];
            }
        }
    }

    return indegrees;
}

std::vector<Node> TopoSearch::kahnAlgorithm(const std::vector<Node> &all_nodes, std::vector<std::vector<int>> adjList, std::vector<int> indegrees)
{
    int n = indegrees.size();

    std::unordered_map<std::string, int> nameToIndex;
    for (int i = 0; i < n; ++i)
    {
        nameToIndex[all_nodes[i].name()] = i;
    }

    std::queue<Node> q;
    for (int i = 0; i < n; ++i)
    {
        if (indegrees[i] == 0)
        {
            q.push(all_nodes[i]);
        }
    }

    std::vector<Node> topological_order;
    while (!q.empty())
    {
        Node u = q.front();
        q.pop();
        topological_order.push_back(u);

        int u_index = nameToIndex.at(u.name());

        for (int v_index : adjList[u_index])
        {
            --indegrees[v_index];

            if (indegrees[v_index] == 0)
            {
                q.push(all_nodes[v_index]);
            }
        }
    }

    if (topological_order.size() != n)
    {
        return {}; // cycle found
    }

    return topological_order;
}


std::vector<int> TopoSearch::computeCountOfDependeces(const std::vector<std::vector<int>>& adjList)
{
    std::vector<int> future_use_count;

    for (std::vector<int> u : adjList)
    {
        future_use_count.push_back(u.size());
    }

    return future_use_count;
}*/