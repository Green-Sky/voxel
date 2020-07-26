#pragma once

#include <functional>

namespace voxel {

// 3D-DDA
// assumes the center of a voxel to be 0.5
// calls fn for each voxel, in order
// if fn returns false, the algorithm is terminated
// TESTED, works
void ray_traversal(
	float start_x, float start_y, float start_z,
	float end_x, float end_y, float end_z,
	std::function<bool(int32_t, int32_t, int32_t)> fn
);

// does not cover all of the "ray-hit"-voxels, but has other uses
void bresenhams_traversal(
	int32_t start_x, int32_t start_y, int32_t start_z,
	int32_t end_x, int32_t end_y, int32_t end_z,
	std::function<bool(int32_t, int32_t, int32_t)> fn
);

// TODO: actually implement
// it generates a line using bresenhams line algorithm in 3D,
// than it starts off at each of those line voxels a (plane) filling algorithm,
// which keeps the chessboard-distance. this only works, bc bresenham
// creates a line with each voxel 1 chessboard-distance further away.
// the voxels from the plane-filling-algorithm now are the starting voxels for
// the 2. round of plane-fills on a perpendicular plane.
//
// this algorithm has the nice property of being front-to-back, but its easy to reverse
// it uses the center of a given voxel (chunk)
//
// i use this for generating chunk ids to render
//
// this is based on the algorithm as discribed in:
// "2D Dynamic Scene Occlusion Culling using a Regular Grid" (2001)
// by Harlen Costa Batagelo AND Wu, Shin-Ting
void frustum_traversal(
	// nope, i assume 0,0,0 as start. offset it urself
	//int32_t start_x, int32_t start_y, int32_t start_z,

	// relative to 0,0,0
	int32_t end_x, int32_t end_y, int32_t end_z,

	// checks wether a voxel is still in view
	std::function<bool(int32_t, int32_t, int32_t)> in_view_fn,

	// called for each voxel in view
	std::function<bool(int32_t, int32_t, int32_t)> each_fn
);

} // voxel

