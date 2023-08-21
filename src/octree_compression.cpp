#include <iostream>
#include "octree_compression.h"

OctreeCompression::OctreeCompression(std::vector<std::vector<std::vector<char>>>* Slices,
                                     std::unordered_map<char, std::string>* TagTable,
                                     Dimensions* Dimensions)
    : Compression(Slices, TagTable, Dimensions)
{}

void OctreeCompression::CompressBlock(int z_start, int y_start, int x_start) {

    std::queue<std::tuple<int, int, int, int, int, int>> q;
    q.push(std::make_tuple(z_start,
                           y_start,
                           x_start,
                           z_start + myDimensions->z_parent - 1,
                           y_start + myDimensions->y_parent - 1,
                           x_start + myDimensions->x_parent - 1));
    
    while (!q.empty()) {
        OctreeNode node(q.front(), mySlices);
        q.pop();
        aggregate           (node,    {0, 1, 2, 3, 4, 5, 6, 7})  || // if all sub-blocks are homogeneous with each other
        n_take_one_aggregate(node, q)                            ||     // else, if any of the 2*2*1 blocks can be grouped
        segregate           (node, q, {0, 1, 2, 3, 4, 5, 6, 7});            // else, segregate the sub-blocks
    }
}

bool OctreeCompression::aggregate(OctreeNode& node, std::vector<int> cells) {
    if (node.homogeneity[cells[0]] == false) {
        return false;
    }
    for (long unsigned int i = 1; i < cells.size(); i++) {
        if (!node.homogeneity[cells[i]] || (*mySlices)[node.z_starts[cells[i]]][node.y_starts[cells[i]]][node.x_starts[cells[i]]] !=
                                           (*mySlices)[node.z_starts[cells[0]]][node.y_starts[cells[0]]][node.x_starts[cells[0]]]) {
            return false;
        }
    }
    PrintOutput(node.x_starts[cells[0]],                                                                                // x_start
                node.y_starts[cells[0]],                                                                                // y_start
                node.z_starts[cells[0]]+current_level,                                                                  // z_start
                node.x_ends[cells[cells.size() - 1]]-node.x_starts[cells[0]]+1,                                         // x_size
                node.y_ends[cells[cells.size() - 1]]-node.y_starts[cells[0]]+1,                                         // y_size
                node.z_ends[cells[cells.size() - 1]]-node.z_starts[cells[0]]+1,                                         // z_size
                (*myTagTable)[(*mySlices)[node.z_starts[cells[0]]][node.y_starts[cells[0]]][node.x_starts[cells[0]]]]); // tag
    return true;
}

bool OctreeCompression::n_take_one_aggregate(OctreeNode& node, std::queue<std::tuple<int, int, int, int, int, int>>& q, int side) {
    std::vector<std::vector<int>>& faces = side == -1 ? sides : side_edges[side];
    int n = faces.size();
    for (int i = 0; i < n; i++) {
        if (aggregate(node, faces[i])                           && // if the current side can be grouped
            ((i < 3 && aggregate(node, faces[n-1-i]))           ||     // then, if the opposite side can be grouped as well
             (side == -1 && n_take_one_aggregate(node, q, 5-i)) ||         // else, if any of the sub-edges of the opposite side can be grouped
             segregate(node, q, faces[n-1-i])))                                // else, segregate the sub-blocks
        return true;
    }
    return false;
}

bool OctreeCompression::segregate(OctreeNode& node, std::queue<std::tuple<int, int, int, int, int, int>>& q, std::vector<int> cells) {
    for (long unsigned int i = 0; i < cells.size(); i++) {
        if (!aggregate(node, {cells[i]})) {
            q.push(node.get_tuple(cells[i]));
        }
    }
    return true;
}