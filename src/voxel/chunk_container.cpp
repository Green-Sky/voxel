#include "./chunk_container.hpp"

#include <glm/common.hpp>

#include <voxel/smart_voxel_container.hpp>

#include "./voxel_traversal.hpp"
#include "glm/ext/vector_float3.hpp"

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
	glm::vec3 start_chunk = start / float(CHUNK_WIDTH);
	glm::vec3 end_chunk = end / float(CHUNK_WIDTH);

	voxel::ray_traverse(
		start_chunk.x, start_chunk.y, start_chunk.z,
		end_chunk.x, end_chunk.y, end_chunk.z,
		[&](int32_t chunk_x, int32_t chunk_y, int32_t chunk_z) -> bool {
			vvox::ChunkID c_id(chunk_x, chunk_y, chunk_z);

			if (!this->has(c_id)) {
				return false;
			}

			glm::vec3 curr_chunk_origin{chunk_x, chunk_y, chunk_z};
			curr_chunk_origin *= float(CHUNK_WIDTH);

			glm::vec3 start_vox{
				glm::clamp(start.x, chunk_x * float(CHUNK_WIDTH), (chunk_x + 1) * float(CHUNK_WIDTH)),
				glm::clamp(start.y, chunk_y * float(CHUNK_WIDTH), (chunk_y + 1) * float(CHUNK_WIDTH)),
				glm::clamp(start.z, chunk_z * float(CHUNK_WIDTH), (chunk_z + 1) * float(CHUNK_WIDTH)),
			};

			glm::vec3 end_vox{
				glm::clamp(end.x, chunk_x * float(CHUNK_WIDTH), (chunk_x + 1) * float(CHUNK_WIDTH)),
				glm::clamp(end.y, chunk_y * float(CHUNK_WIDTH), (chunk_y + 1) * float(CHUNK_WIDTH)),
				glm::clamp(end.z, chunk_z * float(CHUNK_WIDTH), (chunk_z + 1) * float(CHUNK_WIDTH)),
			};

			// make em chunk local
			start_vox -= curr_chunk_origin;
			end_vox -= curr_chunk_origin;

			voxel::ray_traverse(
				start_vox.x, start_vox.y, start_vox.z,
				end_vox.x, end_vox.y, end_vox.z,
				[&](int32_t voxel_x, int32_t voxel_y, int32_t voxel_z) -> bool {

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

