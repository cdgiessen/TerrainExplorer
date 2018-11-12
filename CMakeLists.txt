cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(GraphicsProject4 VERSION 0.0.1 LANGUAGES CXX)

find_package (OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SOIL CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_executable(main 

src/TerrainFPS.cpp
src/GridFloatData.cpp
)

target_link_libraries(main PRIVATE ${OPENGL_LIBRARIES})

target_include_directories(main PRIVATE GLEW)
target_link_libraries(main PRIVATE GLEW::GLEW)

target_link_libraries(main PRIVATE SOIL)

target_link_libraries(main PUBLIC glm)

target_include_directories(main PUBLIC glfw)
target_link_libraries(main PRIVATE glfw)

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_custom_command(TARGET main POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${PROJECT_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:main>)