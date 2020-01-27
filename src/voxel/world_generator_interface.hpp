#pragma once

#include "./chunk.hpp"
#include "./voxel_dict.hpp"

namespace voxel {

class WorldGeneratorI {
	public:
		WorldGeneratorI(VoxelDict<uint16_t>& voxel_dict) : _voxel_dict(voxel_dict) {}
		virtual ~WorldGeneratorI(void) {};

		// returns generated vox count
		virtual uint64_t generateChunk(vvox::Chunk& chunk) = 0;
		//virtual void generateChunk(vvox::Chunk& chunk) = 0;

	protected:
		VoxelDict<uint16_t>& _voxel_dict;
};

} // voxel

