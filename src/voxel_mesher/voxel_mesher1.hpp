#pragma once

#include <voxel/chunk.hpp>
#include <voxel/voxel_dict.hpp>
#include <voxel/chunk_container.hpp> // for looking up neighboring chunks

// TODO: make this configurable
namespace voxel::mesher1 {

	// data per face (4(6) verts)
	using vertex_type = uint64_t;
	// packing:
	// cords (15 bit)
	//   5 bit x
	//   5 bit y
	//   5 bit z
	// ao (8 bit)
	//   2 bit a
	//   2 bit b
	//   2 bit c
	//   2 bit d
	// UNUSED (9 bit)
	// tex index (16 bit)
	//  16 bit texture index
	// UNUSED (16 bit)
	//  16 bit UNUSED

	// ao - uv, correlation:
	// c--d
	// |  |
	// a--b

	// provide to mesher, gets filled with mesh data
	struct mesh_buffer_t {
		// for 32x32x32 practical sizes are: 0 - ~3000
		// theoretical chunk with most faces has checker pattern, so a half filled chunk: CHUNK_SIZE/2
		// but eg. checker pettern of different leaf types would create CHUNK_SIZE
		vertex_type build_buffer[voxel::VoxelFace_COUNT][CHUNK_SIZE];
		//vertex_type* build_buffer[voxel::VoxelFace_COUNT];
		size_t buffer_size[voxel::VoxelFace_COUNT] = {0,0,0,0,0,0};
	};

	// quick check, wether neighboring chunks are loaded
	// TODO: optimize
	inline bool canMesh(const vvox::ChunkID id, voxel::ChunkContainer& cc) {
		for (int x = -1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				for (int z = -1; z <= 1; z++) {
					// TODO: skip self?
					if (!cc.has(vvox::ChunkID(id.p.x + x, id.p.y + y, id.p.z + z))) {
						return false;
					}
					if (!cc.chunks[vvox::ChunkID(id.p.x + x, id.p.y + y, id.p.z + z)].isLoaded) {
						return false;
					}
				}
			}
		}
		return true;
	}

	// warp to neighboring chunks
	inline uint16_t getVoxelAt(
			vvox::SmartVoxelContainer<uint16_t>& self_voxels,
			const vvox::ChunkID& self_id,
			voxel::ChunkContainer& cc,
			int32_t x, int32_t y, int32_t z) {
		// TODO: optimize, bitmask?
		// within own chunk
		if (x >= 0 && y >= 0 && z >= 0 &&
			x < CHUNK_WIDTH && y < CHUNK_WIDTH && z < CHUNK_WIDTH) {
			return self_voxels.get(POS_IN_CHUNK(x, y, z));
		}

		// determine neighbor
		vvox::ChunkID neighbor = self_id;
		if (x < 0) {
			neighbor.p.x--;
			x += CHUNK_WIDTH;
		} else if (x == CHUNK_WIDTH) {
			neighbor.p.x++;
			x -= CHUNK_WIDTH;
		}

		if (y < 0) {
			neighbor.p.y--;
			y += CHUNK_WIDTH;
		} else if (y == CHUNK_WIDTH) {
			neighbor.p.y++;
			y -= CHUNK_WIDTH;
		}

		if (z < 0) {
			neighbor.p.z--;
			z += CHUNK_WIDTH;
		} else if (z == CHUNK_WIDTH) {
			neighbor.p.z++;
			z -= CHUNK_WIDTH;
		}

		// get neighbor
		if (!cc.has(neighbor)) {
			// TODO: notify ?
			return 0;
		}

		auto& nci = cc.chunks[neighbor];
		if (!nci.isLoaded || !nci.chunk) {
			// TODO: notify ?
			return 0;
		}

		auto& nvoxels = nci.chunk->voxels;

		return nvoxels.get(POS_IN_CHUNK(x, y, z));
	}

	// essentially the normal of the face
	const int16_t facing_off[voxel::VoxelFace_COUNT][3] {
		{0, 0, -1},
		{0, 0, +1},
		{0, -1, 0},
		{0, +1, 0},
		{-1, 0, 0},
		{+1, 0, 0},
	};

	enum ao_vertex_t {
		A = 0,
		B,
		C,
		D,
		ao_vertex_t_COUNT
	};

	enum ao_vertex_neighbor_t {
		SIDE1 = 0,
		SIDE2,
		CORNER,
		ao_vertex_neighbor_t_COUNT
	};

	const int8_t ao_coord_lut
		[voxel::VoxelFace_COUNT]
		[ao_vertex_t_COUNT]
		[ao_vertex_neighbor_t_COUNT]
		[3] {
// GENERATED BY ao_lut_calc.cpp START #########################
#include "./mesher_ao_lut.inl"
// GENERATED BY ao_lut_calc.cpp END   #########################
	};

	inline bool faceTest(
			const vvox::ChunkID id,
			voxel::ChunkContainer& cc,
			const voxel::VoxelDict<uint16_t>& voxel_dict,
			vvox::SmartVoxelContainer<uint16_t>& self_voxels,
			voxel::VoxelFace face,
			int16_t x, int16_t y, int16_t z);

// define this macro, if you have your own face test!
#ifndef VOXEL_MESHER1_FACETEST_PROVIDED
	inline bool faceTest(
			const vvox::ChunkID id,
			voxel::ChunkContainer& cc,
			const voxel::VoxelDict<uint16_t>& voxel_dict,
			vvox::SmartVoxelContainer<uint16_t>& self_voxels,
			voxel::VoxelFace face,
			int16_t x, int16_t y, int16_t z) {

#define FACING_POS(x,y,z) (x)+facing_off[face][0], (y)+facing_off[face][1], (z)+facing_off[face][2]
		// switch on own voxel type
		switch (voxel_dict.getType(getVoxelAt(self_voxels, id, cc, x, y, z))) {
			case voxel::VoxelType::SOLID:
				if (voxel_dict.getType(getVoxelAt(self_voxels, id, cc, FACING_POS(x, y, z))) == voxel::VoxelType::SOLID) {
					return false;
				} else {
					return true;
				}
			case voxel::VoxelType::TRANSP_FULL: //[[fallthrogh]]
			case voxel::VoxelType::TRANSP_OPAQUE: // for leafs glas ...
				if (getVoxelAt(self_voxels, id, cc, FACING_POS(x, y, z)) == getVoxelAt(self_voxels, id, cc, x, y, z)) {
					return false;
				} else {
					return true;
				}
				break;
			case voxel::VoxelType::EMPTY:
				return false;
			default:
				return false;
		}
#undef FACING_POS
	}

#endif// VOXEL_MESHER1_FACETEST_PROVIDED

	void generateMesh(
			// from
			const vvox::ChunkID id,
			voxel::ChunkContainer& cc,
			const voxel::VoxelDict<uint16_t>& voxel_dict,
			// to
			mesh_buffer_t& mesh_buffer);

} // voxel::mesher1
