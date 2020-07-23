#pragma once

#include <functional>

namespace voxel {

// assumes the center of a voxel to be 0.5
// calls fn for each voxel, in order
// if fn returns false, the algorithm is terminated
void ray_traverse(
	float start_x, float start_y, float start_z,
	float end_x, float end_y, float end_z,
	std::function<bool(int32_t, int32_t, int32_t)> fn
);

} // voxel

