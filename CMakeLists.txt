cmake_minimum_required(VERSION 3.10)

project("skin-builder")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)


file(GLOB SOURCES "src/*.cpp" "src/*.hpp" "src/*/*.hpp" "src/*/*.cpp" "src/*/*/*.hpp" "src/*/*/*.cpp")

add_executable("${PROJECT_NAME}" "${SOURCES}")
find_package(Vulkan REQUIRED)

# set(GLFW_VULKAN_STATIC ON)
set(GLFW_BUILD_WIN32 OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory("deps/glfw")

add_subdirectory("deps/glm")

message("${Vulkan_LIBRARY}")

target_include_directories("${PROJECT_NAME}" PUBLIC "${Vulkan_INCLUDE_DIRS}" PRIVATE "src")

target_link_libraries("${PROJECT_NAME}" PRIVATE "glfw" "glm::glm" "${Vulkan_LIBRARIES}")