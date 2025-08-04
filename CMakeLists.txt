cmake_minimum_required(VERSION 3.22)



# Basic setup and utils - keep these early
include(${CMAKE_CURRENT_SOURCE_DIR}/qpm_defines.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/qpm.cmake)

# Add compiler caching here
find_program(CCACHE_PROGRAM ccache)
if(NOT CCACHE_PROGRAM)
    find_program(CCACHE_PROGRAM sccache)
endif()
if(CCACHE_PROGRAM)
    message(STATUS "Using compiler cache: ${CCACHE_PROGRAM}")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Define project
project(ImageFactory VERSION ${PACKAGE_VERSION})

# Globals
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Set COMPILE_ID early for qpm purposes
set(COMPILE_ID ${CMAKE_PROJECT_NAME})

# Add compile definitions
add_compile_definitions(MOD_ID="${CMAKE_PROJECT_NAME}")
add_compile_definitions(VERSION="${CMAKE_PROJECT_VERSION}")
add_compile_options(-frtti -fexceptions -fvisibility=hidden -fPIE -fPIC)

# Build options
option(QUEST "Build for quest" ON)
if(QUEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/targets/quest.cmake)
endif()

# Build type configuration
if(${CMAKE_BUILD_TYPE} STREQUAL "RELEASE" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
    message(STATUS "Building in release mode")
    add_compile_options(-O3)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    add_compile_options(-flto)
else()
    message(STATUS "Building in debug mode")
    add_compile_options(-O0 -g)
endif()

# recursively get all src files
RECURSE_FILES(cpp_file_list ${SOURCE_DIR}/*.cpp)
RECURSE_FILES(c_file_list ${SOURCE_DIR}/*.c)

RECURSE_FILES(inline_hook_c ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.c)
RECURSE_FILES(inline_hook_cpp ${EXTERN_DIR}/includes/beatsaber-hook/shared/inline-hook/*.cpp)

# Define library target FIRST before QPM setup
add_library(
    ${CMAKE_PROJECT_NAME}
    SHARED
    ${cpp_file_list}
    ${c_file_list}
    ${inline_hook_c}
    ${inline_hook_cpp}
    "${CMAKE_CURRENT_SOURCE_DIR}/include/submodules/EasyGifReader/EasyGifReader.cpp"
)


# Include Kaleb's CMake includes
include(${EXTERN_DIR}/includes/kaleb/shared/cmake/assets.cmake)

# add assets
add_assets(${CMAKE_PROJECT_NAME}-assets STATIC ${CMAKE_CURRENT_LIST_DIR}/assets ${INCLUDE_DIR}/assets.hpp)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE -llog -lz ${CMAKE_PROJECT_NAME}-assets)

# Include directories
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SHARED_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${EXTERN_DIR}/includes/gif-lib/shared)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/submodules/EasyGifReader/")
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCE_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE test/src)

# Post build
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/strip.cmake)

# stop symbols leaking
add_link_options(-Wl, --exclude-libs, ALL)