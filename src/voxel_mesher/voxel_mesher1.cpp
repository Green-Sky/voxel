#include "./voxel_mesher1.hpp"

#include <cstring>
#include <functional>

namespace voxel::mesher1 {

// https://0fps.net/2013/07/03/ambient-occlusion-for-minecraft-like-worlds/
static inline uint8_t calc_ao_vertex(bool side1, bool side2, bool corner) {
	if (side1 && side2)
		return 0;

	return 3 - (side1 + side2 + corner);
	//#define BOOL_TO_NUM(x) (x ? 1 : 0)
	//return 3 - (BOOL_TO_NUM(side1) + BOOL_TO_NUM(side2) + BOOL_TO_NUM(corner));
}

bool faceTest(
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
		case voxel::VoxelType::TRANSP_FULL: //[[fallthrough]]
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

void generateMesh(
		const vvox::ChunkID id,
		voxel::ChunkContainer& cc,
		const voxel::VoxelDict<uint16_t>& voxel_dict,
		mesh_buffer_t& mesh_buffer) {

#define CUT_COORD(x) (x&0x1f)

#define COMPRESS_COORDS(x,y,z) \
	vertex_type((CUT_COORD(x)) | \
	(CUT_COORD(y) << 5) | \
	(CUT_COORD(z) << 10))

#define CUT_AO(x) (x&0x03)

#define COMPRESS_AO(a,b,c,d) \
	(CUT_AO(a) | \
	(CUT_AO(b) << 2) | \
	(CUT_AO(c) << 4) | \
	(CUT_AO(d) << 6))

#define COMPRESS_FACE(x,y,z,ao,tex) \
	(COMPRESS_COORDS(x,y,z) | \
	(vertex_type(ao) << 15) | \
	(vertex_type(tex) << 32))

	auto& self_voxels = cc.chunks[id].chunk->voxels;

	// TODO: optimize, if interval tree
	for (int64_t x = 0; x < CHUNK_WIDTH; x++) {
		for (int64_t y = 0; y < CHUNK_WIDTH; y++) {
			for (int64_t z = 0; z < CHUNK_WIDTH; z++) {
				// TODO: seperate sold from transparent
				for (size_t face = 0; face < voxel::VoxelFace_COUNT; face++) {
					if (mesh_buffer.face_test_fn(id, cc, voxel_dict, self_voxels, (voxel::VoxelFace)face, x, y, z)) {
						const auto voxel_id = getVoxelAt(self_voxels, id, cc, x, y, z);
						uint8_t ao_verts[4];
						for (int ao_vert = 0; ao_vert < 4; ao_vert++) {

							int64_t s1_coord[3] {
								ao_coord_lut[face][ao_vert][0][0] + x,
								ao_coord_lut[face][ao_vert][0][1] + y,
								ao_coord_lut[face][ao_vert][0][2] + z
							};

							int64_t s2_coord[3] {
								ao_coord_lut[face][ao_vert][1][0] + x,
								ao_coord_lut[face][ao_vert][1][1] + y,
								ao_coord_lut[face][ao_vert][1][2] + z
							};

							int64_t c_coord[3] {
								ao_coord_lut[face][ao_vert][2][0] + x,
								ao_coord_lut[face][ao_vert][2][1] + y,
								ao_coord_lut[face][ao_vert][2][2] + z
							};

							auto s1_voxel = getVoxelAt(self_voxels, id, cc, s1_coord[0], s1_coord[1], s1_coord[2]);
							auto s2_voxel = getVoxelAt(self_voxels, id, cc, s2_coord[0], s2_coord[1], s2_coord[2]);
							auto c_voxel = getVoxelAt(self_voxels, id, cc, c_coord[0], c_coord[1], c_coord[2]);

							ao_verts[ao_vert] = calc_ao_vertex(
								voxel_dict.getType(s1_voxel) == voxel::VoxelType::SOLID,
								voxel_dict.getType(s2_voxel) == voxel::VoxelType::SOLID,
								voxel_dict.getType(c_voxel) == voxel::VoxelType::SOLID
							);
						}

						uint8_t ao = COMPRESS_AO(ao_verts[0], ao_verts[1], ao_verts[2], ao_verts[3]);

						mesh_buffer.build_buffer[face][mesh_buffer.buffer_size[face]++] =
							COMPRESS_FACE(x, y, z, ao, (voxel_dict.getTexFaces(voxel_id))[face]);
					}
				}
			}
		}
	}
}

} // voxel::mesher1

