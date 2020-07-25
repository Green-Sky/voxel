#include "./voxel_traversal.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

#include <glm/common.hpp>

namespace voxel {

enum AXIS : size_t {
	X = 0,
	Y = 1,
	Z = 2
};

void ray_traversal(
	float start_x, float start_y, float start_z,
	float end_x, float end_y, float end_z,
	std::function<bool(int32_t, int32_t, int32_t)> fn
) {
	glm::vec3 ray{
		end_x-start_x,
		end_y-start_y,
		end_z-start_z,
	};

	glm::vec3 dir = glm::normalize(ray);

	glm::ivec3 start_voxel {glm::floor(start_x), glm::floor(start_y), glm::floor(start_z)};
	glm::ivec3 end_voxel {glm::floor(end_x), glm::floor(end_y), glm::floor(end_z)};

	glm::ivec3 step = glm::sign(ray);

	glm::ivec3 curr_voxel = start_voxel;

	glm::vec3 tMax{
		(start_voxel.x+1) - start_x,
		(start_voxel.y+1) - start_y,
		(start_voxel.z+1) - start_z,
	};

	tMax = glm::abs(tMax);

	//glm::vec3 tDelta = step / dir;
	glm::vec3 tDelta = glm::sign(ray) / dir;

	//enum AXIS : size_t {
		//X = 0,
		//Y = 1,
		//Z = 2
	//};

	// start voxel
	if (!fn(curr_voxel.x, curr_voxel.y, curr_voxel.z)) {
		return;
	}

	// TODO: face
	do {
		AXIS smallest_tMax = tMax.x < tMax.y ? X : Y;
		smallest_tMax = tMax[smallest_tMax] < tMax.z ? smallest_tMax : Z;

		curr_voxel[smallest_tMax] += step[smallest_tMax];
		tMax[smallest_tMax] += tDelta[smallest_tMax];

		if (!fn(curr_voxel.x, curr_voxel.y, curr_voxel.z)) {
			break;
		}
	} while (curr_voxel != end_voxel);
}

void bresenhams_traversal(
	int32_t start_x, int32_t start_y, int32_t start_z,
	int32_t end_x, int32_t end_y, int32_t end_z,
	std::function<bool(int32_t, int32_t, int32_t)> fn
) {
	if (!fn(start_x, start_y, start_z)) {
		return;
	}

	glm::ivec3 start {start_x, start_y, start_z};
	glm::ivec3 end {end_x, end_y, end_z};

	glm::ivec3 d = glm::abs(end - start);

	// TODO: 0?
	glm::ivec3 sign = glm::sign(d);

	//enum AXIS : size_t {
		//X = 0,
		//Y = 1,
		//Z = 2
	//};

	AXIS driving_axis = d.x > d.y ? AXIS::X : AXIS::Y;
	driving_axis = d[driving_axis] > d.z ? driving_axis : AXIS::Z;

	auto p1 = 2 * d[(driving_axis+1)%3] - d[driving_axis];
	auto p2 = 2 * d[(driving_axis+2)%3] - d[driving_axis];

	glm::ivec3 curr = start;


	while (curr[driving_axis] != end[driving_axis]) {
		curr[driving_axis] += sign[driving_axis];

		if (p1 >= 0) {
			curr[(driving_axis+1)%3] += sign[(driving_axis+1)%3];
			p1 -= 2 * d[driving_axis];
		}
		if (p2 >= 0) {
			curr[(driving_axis+2)%3] += sign[(driving_axis+2)%3];
			p2 -= 2 * d[driving_axis];
		}

		p1 += 2 * d[(driving_axis+1)%3];
		p2 += 2 * d[(driving_axis+2)%3];

		if (!fn(curr.x, curr.y, curr.z)) {
			break;
		}
	}
}

static void chess_distance_2d_walk(
	glm::ivec2 start,
	// only use x and y
	AXIS axis,
	// sign should be 1 or -1
	int32_t sign,
	std::function<bool(int32_t, int32_t)> each_fn
) {
	glm::ivec2 curr = start;

	do {
		curr[axis] += sign;

		if (glm::abs(curr.x) == glm::abs(curr.y)) {
			axis = AXIS((axis+1)%2);
			sign = curr[axis] > 0 ? -1 : 1;
		}

		// quit if we walked 360
		if (curr == start) {
			//break;
			return;
		}
	} while (each_fn(curr.x, curr.y));
}

// 0,0,0 as origin
// also called Chebyshev distance
static void chess_distance_2d_traversal(
	glm::ivec2 start,
	std::function<bool(int32_t, int32_t)> each_fn
) {
	if (glm::abs(start.x) > glm::abs(start.y)) {
		chess_distance_2d_walk(start, AXIS::Y, +1, each_fn);
		chess_distance_2d_walk(start, AXIS::Y, -1, each_fn);
	} else if (glm::abs(start.y) > glm::abs(start.s)) {
		chess_distance_2d_walk(start, AXIS::X, +1, each_fn);
		chess_distance_2d_walk(start, AXIS::X, -1, each_fn);
	} else { // abs() is equal
		if (start.x > 0) {
			if (start.y > 0) {
				chess_distance_2d_walk(start, AXIS::X, -1, each_fn);
				chess_distance_2d_walk(start, AXIS::Y, -1, each_fn);
			} else if (start.y < 0) {
				chess_distance_2d_walk(start, AXIS::X, -1, each_fn);
				chess_distance_2d_walk(start, AXIS::Y, +1, each_fn);
			}
		} else if (start.x < 0) {
			if (start.y > 0) {
				chess_distance_2d_walk(start, AXIS::X, +1, each_fn);
				chess_distance_2d_walk(start, AXIS::Y, -1, each_fn);
			} else if (start.y < 0) {
				chess_distance_2d_walk(start, AXIS::X, +1, each_fn);
				chess_distance_2d_walk(start, AXIS::Y, +1, each_fn);
			}
		}
	}

	// special case 0,0 is dropped
}

void frustum_traversal(
	int32_t end_x, int32_t end_y, int32_t end_z,

	std::function<bool(int32_t, int32_t, int32_t)> in_view_fn,

	std::function<bool(int32_t, int32_t, int32_t)> each_fn
) {
	if (!each_fn(0, 0, 0)) {
		return;
	}

	if (end_x == 0 && end_y == 0 && end_z == 0) {
		return;
	}

	glm::ivec3 end {end_x, end_y, end_z};

	bresenhams_traversal(
		0, 0, 0,
		end_x, end_y, end_z,
		[](int32_t seed_x, int32_t seed_y, int32_t seed_z) -> bool {
			glm::ivec3 seed {seed_x, seed_y, seed_z};

			//// the filling planes are perpendicular to the driving axis
			//AXIS driving_axis = glm::abs(seed.x) >= glm::abs(seed.y) ? AXIS::X : AXIS::Y;
			//driving_axis = seed[driving_axis] >= glm::abs(seed.z) ? driving_axis : AXIS::Z;

			//glm::ivec3 curr = seed;

			//// positive branch
			//{
				//// prepare
				//if (curr[driving_axis] == curr[(driving_axis+1)%3]) {
					//driving_axis = AXIS((driving_axis+1)%3);
				//}

				//curr[(driving_axis+1)%3] += sign[(driving_axis+1)%3];

				//while (true) {
					//if (curr[driving_axis] == curr[(driving_axis+1)%3]) {
						//driving_axis = AXIS((driving_axis+1)%3);
					//}

					//curr[(driving_axis+1)%3] += sign[(driving_axis+1)%3];
				//}
			//}

			//// negative branch
			//{
				//curr[(driving_axis+1)%3] -= sign[(driving_axis+1)%3];

				//while (true) {
					//if (curr[driving_axis] == curr[(driving_axis+1)%3]) {
						//driving_axis = AXIS((driving_axis+1)%3);
					//}

					//curr[(driving_axis+1)%3] -= sign[(driving_axis+1)%3];
				//}
			//}

			return true;
		}
	);
}

} // voxel

