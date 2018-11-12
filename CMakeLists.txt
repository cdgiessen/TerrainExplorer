cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

project(GraphicsProject4 VERSION 0.0.1 LANGUAGES CXX)

find_package (OpenGL REQUIRED)
find_package(GLEW REQUIRED)
#find_package(SOIL CONFIG REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

IF (SOIL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(SOIL_FIND_QUIETLY TRUE)
ENDIF (SOIL_INCLUDE_DIR)

FIND_PATH(SOIL_INCLUDE_DIR SOIL.h PATH_SUFFIXES include/SOIL include)

SET(SOIL_NAMES SOIL Soil soil)
FIND_LIBRARY(SOIL_LIBRARY NAMES ${SOIL_NAMES} )
MARK_AS_ADVANCED( SOIL_LIBRARY SOIL_INCLUDE_DIR )

# Per-recommendation
SET(SOIL_INCLUDE_DIRS "${SOIL_INCLUDE_DIR}")
SET(SOIL_LIBRARIES    "${SOIL_LIBRARY}")

# handle the QUIETLY and REQUIRED arguments and set SOIL_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SOIL DEFAULT_MSG SOIL_LIBRARIES SOIL_INCLUDE_DIRS)

add_executable(main 

src/TerrainFPS.cpp
src/GridFloatData.cpp
)

target_link_libraries(main PRIVATE ${OPENGL_LIBRARIES})

target_include_directories(main PRIVATE GLEW)
target_link_libraries(main PRIVATE GLEW::GLEW)

target_link_libraries(main PRIVATE ${SOIL_LIBRARY})

target_link_libraries(main PUBLIC glm)

target_include_directories(main PUBLIC glfw)
target_link_libraries(main PRIVATE glfw)

target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/src)

add_custom_command(TARGET main POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${PROJECT_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:main>)
