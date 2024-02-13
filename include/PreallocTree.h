#pragma once

#include <cstdio>
#include <array>

struct ChildrenCollection
{
    unsigned int StartIndex;
    unsigned int ChildrenCount;
};

template <typename T, size_t BRANCH_WIDTH, size_t BRANCH_DEPTH>
class PreallocTree
{
private:
    std::array<unsigned int, BRANCH_WIDTH * BRANCH_DEPTH> ChildrenCount;

public:
    std::array<T, BRANCH_WIDTH * BRANCH_DEPTH> Nodes;

    TreeNode<T> SetNode(unsigned int index, T const & value)
    {
        // copy
        Nodes[index] = value;
    }

    ChildrenCollection AddChildren(unsigned int index)
    {
        return ChildrenCollection{.StartIndex = 0, .ChildrenCount = ++ChildrenCount[index]};
    }

    ChildrenCollection GetChildren(unsigned int index);
};