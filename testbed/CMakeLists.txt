set (TESTBED_SOURCE_FILES
	draw.cpp
	draw.h
	imgui_impl_glfw.cpp
	imgui_impl_glfw.h
	imgui_impl_opengl3.cpp
	imgui_impl_opengl3.h
	main.cpp
	settings.h
	settings.cpp
	test.cpp
	test.h
	chromosome.h
	chromosome.cpp
	car.h
	car.cpp
	es_solver.cpp
	sga_solver.cpp
	tests/es_straight_line.hpp
	tests/es_straight_line.cpp
	tests/sga_straight_line.hpp
	tests/sga_straight_line.cpp
	tests/es_random_track.hpp
	tests/es_random_track.cpp
	tests/sga_random_track.hpp
	tests/sga_random_track.cpp
#	tests/es_small_hills.hpp
#	tests/es_small_hills.cpp
#	tests/es_mountains.hpp
#	tests/es_mountains.cpp
	tests/es_benchmark_seed0.hpp
	tests/es_benchmark_seed0.cpp
	tests/es_benchmark_seed1.hpp
	tests/es_benchmark_seed1.cpp
	tests/environments/car_environment.hpp
	tests/environments/car_environment.cpp
#	tests/environments/small_hills.hpp
#	tests/environments/small_hills.cpp
	tests/environments/straight_line.hpp
	tests/environments/straight_line.cpp
#	tests/environments/mountains.hpp
#	tests/environments/mountains.cpp
	tests/environments/random_track.hpp
	tests/environments/random_track.cpp
)

add_executable(testbed ${TESTBED_SOURCE_FILES})
target_include_directories(testbed PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(testbed PUBLIC box2d glfw imgui sajson glad)
set_target_properties(testbed PROPERTIES
	CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

add_compile_options(-Ofast)

# message(STATUS "runtime = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
# message(STATUS "binary = ${CMAKE_CURRENT_BINARY_DIR}")

# Copy font files, etc
add_custom_command(
        TARGET testbed POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/data/
                ${CMAKE_CURRENT_BINARY_DIR}/data/)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TESTBED_SOURCE_FILES})
