#include "./chunk.hpp"

//#include "VoxelSpaceConversions.h"

namespace voxel {

//void Chunk::init(WorldCubeFace face) {
void Chunk::init(void) {
    // Get position from ID
    m_chunkPosition = {m_id.p.x, m_id.p.y, m_id.p.z};
    //m_voxelPosition = VoxelSpaceConversions::chunkToVoxel(m_chunkPosition);
}

void Chunk::initAndFillEmpty(vvox::VoxelStorageState /*= vvox::VoxelStorageState::INTERVAL_TREE*/) {
    init();
    IntervalTree<uint16_t>::LNode voxelNode;
    //IntervalTree<uint16_t>::LNode tertiaryNode;
    voxelNode.set(0, CHUNK_SIZE, 0);
    //tertiaryNode.set(0, CHUNK_SIZE, 0);
    voxels.initFromSortedArray(vvox::VoxelStorageState::INTERVAL_TREE, &voxelNode, 1);
    //tertiary.initFromSortedArray(vvox::VoxelStorageState::INTERVAL_TREE, &tertiaryNode, 1);
}

} // voxel

