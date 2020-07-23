#pragma once

#include <unordered_map>
#include <list>
#include <functional>

#include "./chunk.hpp"

#include "./world_generator_interface.hpp"

#include <glm/fwd.hpp>

namespace voxel {

struct ChunkInfo {
	vvox::ChunkPtr chunk = nullptr;
	//bool isLoading = false;
	bool isLoaded = false;
	bool isActive = false; // ??
};

struct ChunkContainer {
	std::unordered_map<vvox::ChunkID, ChunkInfo> chunks;
	std::list<vvox::Chunk> chunk_storage;

	std::shared_ptr<WorldGeneratorI> world_generator = nullptr;

	bool has(vvox::ChunkID id) {
		return chunks.count(id);
	}

	bool initChunk(vvox::ChunkID id);

	// broken, reimplement pls
	void rayTraverseVoxels(const glm::vec3& start, const glm::vec3& end, std::function<bool(int32_t, int32_t, int32_t, vvox::ChunkID)> fn);
};

} // voxel

