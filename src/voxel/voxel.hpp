#pragma once

#include <cstdint>

namespace voxel {

enum VoxelFace : uint8_t {
	BOTTOM = 0x00,
	TOP,
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	VoxelFace_COUNT
};

} // voxel

