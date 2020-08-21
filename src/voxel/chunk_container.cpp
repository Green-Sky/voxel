#include "./chunk_container.hpp"

#include <glm/common.hpp>
#include <glm/geometric.hpp>

#include <voxel/smart_voxel_container.hpp>

#include "./voxel_traversal.hpp"
#include "glm/fwd.hpp"

namespace voxel {

bool ChunkContainer::initChunk(vvox::ChunkID id) {
	if (has(id)) {
		return false;
	}

	auto& new_chunk = chunk_storage.emplace_back();

	new_chunk.m_id = id;
	new_chunk.initAndFillEmpty();
	new_chunk.flagDirty();

	chunks[id] = {&new_chunk, false, false};

	return true;
}

void ChunkContainer::rayTraverseVoxels(const glm::vec3& start, const glm::vec3& end, std::function<bool(int32_t, int32_t, int32_t, vvox::ChunkID)> fn) {
	// traverse chunks
	glm::vec3 start_inchunkres = start / float(CHUNK_WIDTH);
	glm::ivec3 start_chunk = glm::floor(start_inchunkres);
	glm::vec3 end_inchunkres = end / float(CHUNK_WIDTH);
	glm::ivec3 end_chunk = glm::floor(end_inchunkres);

	glm::vec3 ray_dir = glm::normalize(end-start);

	voxel::ray_traversal(
		start_inchunkres,
		end_inchunkres,
		[&](const glm::ivec3& chunk, AXIS hit_on_axis, const glm::ivec3& sign, const glm::vec3& tMax) -> bool {
			vvox::ChunkID c_id(chunk);

			if (!this->has(c_id)) {
				return false;
			}

			glm::vec3 curr_chunk_origin = chunk;
			curr_chunk_origin *= float(CHUNK_WIDTH);

			glm::vec3 start_vox = start;
			glm::vec3 end_vox = end;

			// make em chunk local
			start_vox -= curr_chunk_origin;
			end_vox -= curr_chunk_origin;

#if 0 // hacked around
			// TODO: make propper
			glm::vec3 lower_bound{0.01f, 0.01f, 0.1f};
			glm::vec3 upper_bound{31.99f, 31.99f, 31.99f};

			// clamp to entry point
			if (chunk != start_chunk) {
				float ray_length = tMax[hit_on_axis] / ray_dir[hit_on_axis];
				std::cerr << "-----ray_length " << ray_length << "\n";

				// adjust start_vox

				// TODO: replace
				start_vox = glm::clamp(start_vox, lower_bound, upper_bound);
			}


			// clamp to exit point
			if (chunk != end_chunk) {
				// TODO: need ray cast <.<

				// adjust end_vox

				// TODO: replace
				end_vox = glm::clamp(end_vox, lower_bound, upper_bound);
			}
#endif

			voxel::ray_traversal(
				start_vox.x, start_vox.y, start_vox.z,
				end_vox.x, end_vox.y, end_vox.z,
				[&](int32_t voxel_x, int32_t voxel_y, int32_t voxel_z, AXIS hit_on_axis, int sign_x, int sign_y, int sign_z) -> bool {

					// skip out of range voxels, TODO: fix this asap
					if (voxel_x < 0) {
						//return false;
						return true;
					}
					if (voxel_y < 0) {
						//return false;
						return true;
					}
					if (voxel_z < 0) {
						//return false;
						return true;
					}
					if (voxel_x >= CHUNK_WIDTH) {
						//return false;
						return true;
					}
					if (voxel_y >= CHUNK_WIDTH) {
						//return false;
						return true;
					}
					if (voxel_z >= CHUNK_WIDTH) {
						//return false;
						return true;
					}

					return fn(
						voxel_x,
						voxel_y,
						voxel_z,
						c_id
					);
				}
			);

			return true;
		}
	);
}

} // voxel

