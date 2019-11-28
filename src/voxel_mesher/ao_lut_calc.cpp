#include <iostream>

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cassert>

enum BlockFace : uint8_t {
	BOTTOM = 0x00,
	TOP,
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	BlockFace_COUNT
};

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
	os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
	return os;
}

std::ostream& operator<<(std::ostream& os, const glm::ivec3& vec) {
	os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
	return os;
}

std::ostream& operator<<(std::ostream& os, const glm::mat3& mat) {
	os << "{" << mat[0] << ",\n" << mat[1] << ",\n" << mat[2] << "}";
	return os;
}

int main(void) {

	glm::ivec3 facing_vecs[BlockFace_COUNT] {
		{0, 0, -1},
		{0, 0, +1},
		{0, -1, 0},
		{0, +1, 0},
		{-1, 0, 0},
		{+1, 0, 0},

	};

	//for (int face = 0; face < 6; face++) {
		//print_vec3(facing_vecs[face]);
		//std::cout << "\n";
	//}

	//std::cout << facing_vecs[FRONT] << "\n";

	const char* face_name[6] {
		"BOTTOM",
		"TOP",
		"FRONT",
		"BACK",
		"LEFT",
		"RIGHT"
	};

	char ao_vert_name[4] {
		'A',
		'B',
		'C',
		'D',
	};

	// normaly column wise, but in this case use row wise
	glm::mat3 ao_vert_tmat[4] {
		// a
		glm::mat3(1),
		// b
		{
			{0, 0, 1},
			{0, 1, 0},
			{-1, 0, 0}
		},
		// c
		{
			{0, 0, -1},
			{0, 1, 0},
			{1, 0, 0}
		},
		// d
		{
			{-1, 0, 0},
			{0, 1, 0},
			{0, 0, -1}
		},
	};

	//for (int i = 0; i < 4; i++) {
		 //ao_vert_tmat[i]= glm::transpose(ao_vert_tmat[i]);
	//}

	glm::mat3 face_rot_tmat[6] {
		// bottom
		{
			{1, 0, 0},
			{0, 0, 1},
			{0, -1, 0}
		},
		// top
		{
			{1, 0, 0},
			{0, 0, -1},
			{0, 1, 0}
		},
		// front
		glm::mat3(1),
		// back
		{
			{-1, 0, 0},
			{0, -1, 0},
			{0, 0, 1}
		},
		// left
		{
			{0, -1, 0},
			{1, 0, 0},
			{0, 0, 1}
		},
		// right
		{
			{0, 1, 0},
			{-1, 0, 0},
			{0, 0, 1}
		}
	};

	//for (int i = 0; i < 6; i++) {
		 //face_rot_tmat[i]= glm::transpose(face_rot_tmat[i]);
	//}

	//std::cout << "mat(1):\n" << glm::mat3(1) << "\n";

	for (size_t face = 0; face < BlockFace_COUNT; face++) {
		std::cout << "// " << face_name[face] << "\n{\n";

		for (size_t ao_vert = 0; ao_vert < 4; ao_vert++) {
			// a
			auto s1 = facing_vecs[FRONT] + glm::ivec3{-1, 0, 0};
			auto s2 = facing_vecs[FRONT] + glm::ivec3{0, 0, -1};
			auto c = facing_vecs[FRONT] + glm::ivec3{-1, 0, -1};

			// transpose by ao_vert pos
			s1 = ao_vert_tmat[ao_vert] * s1;
			s2 = ao_vert_tmat[ao_vert] * s2;
			c = ao_vert_tmat[ao_vert] * c;

			// transpose by face
			s1 = face_rot_tmat[face] * s1;
			s2 = face_rot_tmat[face] * s2;
			c = face_rot_tmat[face] * c;

			// tests
			if (false) {
				if (face == BOTTOM) {
					if (ao_vert == 0) {
						assert(s1 == glm::ivec3(-1, 0, -1));
						assert(s2 == glm::ivec3(0, 1, -1));
						assert(c == glm::ivec3(-1, 1, -1));
					}
				} else if (face == TOP) {
					if (ao_vert == 2) { // c
						//assert(s1 == glm::ivec3(1, 0, 1));
						//assert(s2 == glm::ivec3(0, 1, 1));
						//std::cout << "#################\n";
						//std::cout << "c: " << c << "\n";
						assert(c == glm::ivec3(1, 1, 1));
					}
				} else if (face == FRONT) {
					if (ao_vert == 0) {
						assert(s1 == glm::ivec3(-1, -1, 0));
						assert(s2 == glm::ivec3(0, -1, -1));
						assert(c == glm::ivec3(-1, -1, -1));
					}
				}
			}

			std::cout << "\t// " << ao_vert_name[ao_vert] << "\n\t{\n";

			std::cout << "\t\t// s1\n\t\t" << s1 << ",\n";
			std::cout << "\t\t// s2\n\t\t" << s2 << ",\n";
			std::cout << "\t\t// c\n\t\t" << c << ",\n";

			std::cout << "\t},\n\n";
		}

		std::cout << "},\n\n";
	}

	return 0;
}

