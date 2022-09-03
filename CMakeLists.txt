cmake_minimum_required(VERSION 3.10)

project("skin-builder")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)


file(GLOB IMGUI_SOURCES "deps/imgui/*.cpp")

file(GLOB SOURCES "src/*.cpp" "src/*.hpp" "src/*/*.hpp" "src/*/*.cpp" "src/*/*/*.hpp" "src/*/*/*.cpp")
file(GLOB SHADERS "assets/shaders/*.frag" "assets/shaders/*.vert")


find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

function(compile_shaders target)
    #cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV;FORMAT" "SOURCES")
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "" "SHADERS")
    foreach(source ${arg_SHADERS})
        get_filename_component(SHADER_FILE_NAME "${source}" NAME_WE)
        get_filename_component(SHADER_STAGE "${source}" LAST_EXT)
        add_custom_command(
            OUTPUT "${CMAKE_BINARY_DIR}/${SHADER_FILE_NAME}${SHADER_STAGE}.spv"
            DEPENDS ${source}
            DEPFILE "${CMAKE_BINARY_DIR}/${SHADER_FILE_NAME}${SHADER_STAGE}.d"
            COMMAND
                ${glslc_executable}
                #$<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                #$<$<BOOL:${arg_FORMAT}>:-mfmt=${arg_FORMAT}>
                -MD -MF "${CMAKE_BINARY_DIR}/${SHADER_FILE_NAME}${SHADER_STAGE}.d"
                -o "${CMAKE_BINARY_DIR}/${SHADER_FILE_NAME}${SHADER_STAGE}.spv"
                ${source}
        )
        message("-- Registering shader: ${SHADER_FILE_NAME}${SHADER_STAGE}")
        target_sources(${target} PRIVATE "${CMAKE_BINARY_DIR}/${SHADER_FILE_NAME}${SHADER_STAGE}.spv")
    endforeach()
endfunction()

# set(GLFW_VULKAN_STATIC ON)
set(GLFW_BUILD_WIN32 OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory("deps/glfw")

add_subdirectory("deps/glm")

add_subdirectory("deps/spdlog")

add_subdirectory("deps/VulkanMemoryAllocator")

option(ASSIMP_INSTALL OFF)
option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
add_subdirectory("deps/assimp" EXCLUDE_FROM_ALL)

add_executable("${PROJECT_NAME}" "${SOURCES}" "${IMGUI_SOURCES}")
compile_shaders(${PROJECT_NAME} SHADERS ${SHADERS})

target_include_directories("${PROJECT_NAME}" PUBLIC "deps/imgui")
target_include_directories("${PROJECT_NAME}" PUBLIC "deps/stbi")

target_compile_definitions("${PROJECT_NAME}" PRIVATE
  $<$<CONFIG:Debug>:SKINBUILDER_DEBUG>
  $<$<CONFIG:Release>:SKINBUILDER_RELEASE>
)

target_compile_options("${PROJECT_NAME}" PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

target_include_directories("${PROJECT_NAME}" PUBLIC "${Vulkan_INCLUDE_DIRS}" PRIVATE "src")

target_link_libraries("${PROJECT_NAME}" PRIVATE "glfw" "glm::glm" "${Vulkan_LIBRARIES}" "spdlog" "VulkanMemoryAllocator" "assimp")