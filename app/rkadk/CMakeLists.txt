message(STATUS "cmake version ${CMAKE_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project(rkadk)
include(${PROJECT_SOURCE_DIR}/version/Version.cmake)

install(DIRECTORY include/ DESTINATION include/rkadk FILES_MATCHING PATTERN "*.h")
include_directories(include)

if(BUILD_STATIC_LIBRARY)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -Wl,--gc-sections")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -fdata-sections -ffunction-sections -Wl,--gc-sections")
endif()

if(NOT ${RKADK_CHIP} STREQUAL "rv1126")
	message("chip not rv1126, use aiq2")
	add_definitions(-DRKAIQ_UAPI2)
endif()

if(${RKADK_CHIP} STREQUAL "rk3506")
	add_definitions(-DRK3506)
elseif(${RKADK_CHIP} STREQUAL "rv1103b")
	add_definitions(-DRV1103B)
elseif(${RKADK_CHIP} STREQUAL "rv1106")
	add_definitions(-DRV1106_1103)
elseif(${RKADK_CHIP} STREQUAL "rv1126")
	add_definitions(-DRV1126_1109)
elseif(${RKADK_CHIP} STREQUAL "rk3576")
	add_definitions(-DRK3576)
endif()

add_definitions(-DTHUMB_NORMAL)

if(USE_RKAIQ)
    find_package(RkAiq REQUIRED)
    include_directories(${RKAIQ_INCLUDE_DIRS})
    add_definitions(-DRKAIQ)
endif()

if(ENABLE_FILE_CACHE)
	add_definitions(-DFILE_CACHE)
endif()

if(ENABLE_AOV)
	add_definitions(-DENABLE_AOV)
endif()

if(ENABLE_EIS)
	add_definitions(-DENABLE_EIS)
endif()

if(OS_LINUX)
	add_definitions(-DOS_LINUX)
endif()

add_definitions(-Wall -Wshadow -ffast-math -Wignored-qualifiers -Werror-implicit-function-declaration -D_GNU_SOURCE)

add_subdirectory(src)

if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

#Install rkadk default configuration file
if(${RKADK_CHIP} STREQUAL "rv1106" OR ${RKADK_CHIP} STREQUAL "rv1103b")
	set(RKADK_INICFG_PATH inicfg/rv1106_1103)
elseif(${RKADK_CHIP} STREQUAL "rv1126")
	set(RKADK_INICFG_PATH inicfg/rv1126_1109)
elseif(${RKADK_CHIP} STREQUAL "rk3576")
	set(RKADK_INICFG_PATH inicfg/rk3576)
else()
	message("Warn: no valid ini is selected, chip=${RKADK_CHIP}")
endif()

file(GLOB RKADK_INICFG_LIST ${RKADK_INICFG_PATH}/rkadk_defsetting*.ini)
install(FILES ${RKADK_INICFG_LIST} DESTINATION etc)
