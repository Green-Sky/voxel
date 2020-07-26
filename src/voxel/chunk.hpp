//
// chunk.hpp
// Seed of Andromeda
//
// Created by Cristian Zaloj on 25 May 2015
// Copyright 2014 Regrowth Studios
// MIT License
// Modified by Erik Scholz 2019
//

#pragma once

#ifndef vorb_chunk_hpp
#define vorb_chunk_hpp

#include "./smart_voxel_container.hpp"

//#include "VoxelCoordinateSpaces.h"
//#include "PlanetHeightData.h"
//#include "MetaSection.h"
//#include "ChunkGenerator.h"
//#include "ChunkID.h"

#include <memory>
#include <shared_mutex>
#include <cassert>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#if defined(_MSC_VER)
#define ALIGNED_(x) __declspec(align(x))
#else
#define ALIGNED_(x) __attribute__ ((aligned(x)))
#endif


namespace voxel {

struct ChunkContainer;

#define CHUNK_STRIDE_X CHUNK_WIDTH
#define CHUNK_STRIDE_Y CHUNK_LAYER
#define CHUNK_STRIDE_Z 1

#define POS_IN_CHUNK(x,y,z) (((x) * CHUNK_STRIDE_X) + ((y) * CHUNK_STRIDE_Y) + ((z) * CHUNK_STRIDE_Z))


using ChunkPosition2D = glm::ivec2;
using ChunkPosition3D = glm::ivec3;

// TODO(green): pack
struct ChunkID {
	ChunkID() : id(0) {};
	ChunkID(const ChunkPosition3D& cp) : p{cp.x, cp.y, cp.z} {};
	ChunkID(int32_t x, int32_t y, int32_t z) : p{x, y, z} {};
	ChunkID(uint64_t id) : id{id} {};
	union {
		struct {
			int64_t x : 24;
			int64_t y : 16;
			int64_t z : 24;
		} p;
		uint64_t id;
	};
	operator uint64_t() const { return id; }
};
// for hashing, see botom of file

static_assert(sizeof(ChunkID) == 8, "ChunkID is not 64 bits");

class Chunk;
typedef Chunk* ChunkPtr;

// TODO(Ben): Move to file
typedef uint16_t BlockIndex;

class ChunkGridData {
	public:
		ChunkGridData() : isLoading(false), isLoaded(false), refCount(1){};
		ChunkGridData(const ChunkPosition3D& pos) : isLoading(false), isLoaded(false), refCount(1)
		{
			gridPosition = {pos.x, pos.y};
			//gridPosition.face = pos.face;
		}

		ChunkPosition2D gridPosition;
		//PlanetHeightData heightData[CHUNK_LAYER];
		bool isLoading;
		bool isLoaded;
		int refCount;
};

// TODO(Ben): Can lock two chunks without deadlock worry with checkerboard pattern updates.

class Chunk {
	//friend class ChunkAccessor;
	//friend class ChunkGenerator;
	//friend class ChunkGrid;
	//friend class ChunkMeshManager;
	//friend class ChunkMeshTask;
	//friend class PagedChunkAllocator;
	//friend class SphericalVoxelComponentUpdater;
	friend struct ChunkContainer;

	public:
		Chunk() : /*neighbor(), genLevel(ChunkGenLevel::GEN_NONE), pendingGenLevel(ChunkGenLevel::GEN_NONE),*/ isAccessible(false),/* accessor(nullptr),*/ m_inLoadRange(false),  m_handleState(0), m_handleRefCount(0) {}

		// Initializes the chunk but does not set voxel data
		// Should be called after ChunkAccessor sets m_id
		void init(void);

		// Initializes the chunk and sets all voxel data to 0
		void initAndFillEmpty(vvox::VoxelStorageState = vvox::VoxelStorageState::INTERVAL_TREE);

		//void setRecyclers(vcore::FixedSizeArrayRecycler<CHUNK_SIZE, uint16_t>* shortRecycler);
		inline void updateContainers(void) {
			voxels.update(dataMutex);
			//tertiary.update(dataMutex);
		}

		/************************************************************************/
		/* Getters															  */
		/************************************************************************/
		const ChunkPosition3D& getChunkPosition() const { return m_chunkPosition; }
		//const VoxelPosition3D& getVoxelPosition() const { return m_voxelPosition; }
		const ChunkID& getID() const { return m_id; }

		uint16_t getBlockData(size_t c) const {
			return voxels.get(c);
		}

		uint16_t getBlockData(int x, int y, int z) const {
#if 1
			// use for debugging
			assert(x >= 0);
			assert(x < CHUNK_WIDTH);
			assert(y >= 0);
			assert(y < CHUNK_WIDTH);
			assert(z >= 0);
			assert(z < CHUNK_WIDTH);
#endif

			return voxels.get(POS_IN_CHUNK(x, y, z));
		}

		//inline uint16_t getTertiaryData(int c) const {
			//return tertiary.get(c);
		//}

		inline void setBlock(int x, int y, int z, uint16_t id) {
#if 1
			// use for debugging
			assert(x >= 0);
			assert(x < CHUNK_WIDTH);
			assert(y >= 0);
			assert(y < CHUNK_WIDTH);
			assert(z >= 0);
			assert(z < CHUNK_WIDTH);
#endif

			voxels.set(POS_IN_CHUNK(x, y, z), id);
		}

		// Marks the chunks as dirty and flags for a re-mesh
		inline void flagDirty() { isDirty = true; }

		/************************************************************************/
		/* Members															  */
		/************************************************************************/
		// Everything else uses this grid data handle
		ChunkGridData* gridData = nullptr;
		//MetaFieldInformation meta;
		//union {
			//struct {
				//ChunkHandle left;
				//ChunkHandle right;
				//ChunkHandle bottom;
				//ChunkHandle top;
				//ChunkHandle back;
				//ChunkHandle front;
			//} neighbor;
			//ChunkHandle neighbors[6];
		//};

		//volatile ChunkGenLevel genLevel;
		//ChunkGenLevel pendingGenLevel;

		bool isDirty;

		//f32 distance2; //< Squared distance

		std::shared_mutex dataMutex;

		volatile bool isAccessible; // ??

		vvox::SmartVoxelContainer<uint16_t> voxels;
		//vvox::SmartVoxelContainer<uint16_t> tertiary;

		//// Block indexes where flora must be generated.
		//std::vector<uint16_t> floraToGenerate;
		volatile uint32_t updateVersion;

		//ChunkAccessor* accessor;

	private:
		// uint32_t m_loadingNeighbors = 0u; ///< Seems like a good idea to get rid of isAccesible
		ChunkPosition3D m_chunkPosition;
		//VoxelPosition3D m_voxelPosition;

		//uint32_t m_activeIndex; ///< Position in active list for m_chunkGrid
		bool m_inLoadRange;

		ChunkID m_id;

		/************************************************************************/
		/* Chunk Handle Data													*/
		/************************************************************************/
		//std::mutex m_handleMutex;
		ALIGNED_(4) volatile uint32_t m_handleState;
		ALIGNED_(4) volatile uint32_t m_handleRefCount;
};

} // vorb::voxel

// Hash for ChunkID
template <>
struct std::hash<voxel::ChunkID> {
	size_t operator()(const voxel::ChunkID& id) const {
		::std::hash<uint64_t> h;
		return h(id.id);
	}
};

namespace vvox = voxel;

#endif // vorb_chunk_hpp
