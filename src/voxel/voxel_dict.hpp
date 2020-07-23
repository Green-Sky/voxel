#pragma once

#include <entt/core/hashed_string.hpp>

#include <array>
#include <unordered_map>
#include <string>

namespace voxel {

enum class VoxelType : uint8_t {
	EMPTY = 0x00,
	SOLID,
	TRANSP_OPAQUE,	// only contains opaque or full transparent colors
	TRANSP_FULL,	// can contain any kind of transparency // TODO: refractive index ?
	VoxelType_COUNT
};

enum VoxelFace : uint8_t {
	BOTTOM = 0x00,
	TOP,
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	VoxelFace_COUNT
};

template<typename T = uint16_t>
struct VoxelDict {
	std::array<std::string, size_t(0x01 << (sizeof(T) * 8))> map_id_to_str;
	std::unordered_map<std::string, T> map_str_to_id;
	std::unordered_map<entt::hashed_string::hash_type, T> map_hs_to_id;

	std::array<VoxelType, size_t(0x01 << (sizeof(T) * 8))> voxel_type;

	// texture atlas id is uint16_t
	// face order: bottom, top, front, back, left, right
	std::array<uint16_t, size_t(0x01 << (sizeof(T) * 8)) * VoxelFace_COUNT> voxel_face_textures;

	public:
		VoxelDict(void) {
			//map_str_to_id.fill("");
			voxel_type.fill(VoxelType::EMPTY);
			//voxel_textures.fill(0);
			voxel_face_textures.fill(8); // tnt lol
		}

		void setType(T id, const VoxelType type) {
			voxel_type[id] = type;
		}

		VoxelType getType(T id) const {
			return voxel_type[id];
		}

		void setString(T id, const std::string& str) {
			map_str_to_id[str] = id;
			map_hs_to_id[entt::hashed_string::value(str.c_str())] = id;
		}

		const std::string& getString(T id) const {
			return map_id_to_str[id];
		}

		T getID(const std::string& str) {
			return map_str_to_id[str];
		}

		T getID(const entt::hashed_string::hash_type hs) {
			return map_hs_to_id[hs];
		}

		void setTexFaces(T id, uint16_t tex_faces[VoxelFace_COUNT]) {
			if (tex_faces) {
				auto* tex_voxel_ptr = voxel_face_textures.data() + id*6;
				for (size_t face = 0; face < VoxelFace_COUNT; face++) {
					tex_voxel_ptr[face] = tex_faces[face];
				}
			}
		}

		const uint16_t* getTexFaces(T id) const {
			return voxel_face_textures.data() + id*VoxelFace_COUNT;
		}

		void set(T id, const VoxelType type, const std::string& str) {
			setType(id, type);
			setString(id, str);
		}

		void set(T id, const VoxelType type, const std::string& str, uint16_t tex_faces[VoxelFace_COUNT]) {
			setType(id, type);
			setString(id, str);
			setTexFaces(id, tex_faces);
		}

		void set(T id, const VoxelType type, const std::string& str,
				uint16_t tex_face_bottom, uint16_t tex_face_top,
				uint16_t tex_face_front, uint16_t tex_face_back,
				uint16_t tex_face_left, uint16_t tex_face_right) {
			setType(id, type);
			setString(id, str);
			uint16_t tex_faces[] = {
				tex_face_bottom, tex_face_top,
				tex_face_front, tex_face_back,
				tex_face_left, tex_face_right
			};
			setTexFaces(id, tex_faces);
		}

		void set(T id, const VoxelType type, const std::string& str, uint16_t tex_face_all) {
			setType(id, type);
			setString(id, str);
			uint16_t tex_faces[] = {
				tex_face_all, tex_face_all,
				tex_face_all, tex_face_all,
				tex_face_all, tex_face_all
			};
			setTexFaces(id, tex_faces);
		}
};

} // voxel

