#pragma once

#include <unordered_map>
#include <list>

#include "./chunk.hpp"

#include "./world_generator_interface.hpp"

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

	inline bool has(vvox::ChunkID id) {
		return chunks.count(id);
	}

	inline bool initChunk(vvox::ChunkID id) {
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
};

} // voxel

