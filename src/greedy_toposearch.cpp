#include "greedy_toposearch.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <queue>

std::vector<Node> TopoSearch::ExecuteOrder(const std::vector<Node> &all_nodes, const std::string &output_name, long total_memory)
{
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