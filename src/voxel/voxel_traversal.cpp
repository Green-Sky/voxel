#include "./voxel_traversal.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"

#include <glm/common.hpp>

namespace voxel {

void ray_traverse(
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

	enum AXIS : size_t {
		X = 0,
		Y = 1,
		Z = 2
	};

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

} // voxel

