cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

add_library(voxel
	./src/voxel/voxel.hpp

	./src/voxel/voxel_traversal.hpp
	./src/voxel/voxel_traversal.cpp

	./src/voxel/smart_voxel_container.hpp
	./src/voxel/smart_voxel_container.cpp

	./src/voxel/interval_tree.hpp
	./src/voxel/interval_tree.inl

	./src/voxel/chunk.hpp
	./src/voxel/chunk.cpp

	./src/voxel/voxel_dict.hpp
	./src/voxel/world_generator_interface.hpp

	./src/voxel/chunk_container.hpp
	./src/voxel/chunk_container.cpp
)

target_compile_features(voxel PUBLIC cxx_std_17)

target_link_libraries(voxel
	glm
	entt
)

target_include_directories(voxel PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src")

############################################################

add_library(voxel_mesher1
	./src/voxel_mesher/voxel_mesher1.hpp
	./src/voxel_mesher/voxel_mesher1.cpp
)

target_link_libraries(voxel_mesher1
	voxel
)

############################################################

# calculate the look up table for the ao block pos of the mesher
add_executable(ao_lut_calc EXCLUDE_FROM_ALL
	./src/voxel_mesher/ao_lut_calc.cpp
)

target_link_libraries(ao_lut_calc
	glm
)

target_compile_features(ao_lut_calc PRIVATE cxx_std_17)

