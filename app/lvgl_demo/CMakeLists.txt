cmake_minimum_required(VERSION 3.8)

project(lvgl_demo)

add_definitions(-g -D__EXPORTED_HEADERS__)

include(FindPkgConfig)
find_package(PkgConfig)

include_directories(
	${PROJECT_SOURCE_DIR}/lvgl/
	${PROJECT_SOURCE_DIR}/sys/
	${PROJECT_SOURCE_DIR}/cJSON/
	${PROJECT_SOURCE_DIR}/common/
	${CMAKE_SYSROOT}/usr/include/libdrm/
	${CMAKE_SYSROOT}/usr/include/rkadk/
	${CMAKE_SYSROOT}/usr/include/lvgl/
	${CMAKE_SYSROOT}/usr/include/lvgl/lv_drivers/
	${CMAKE_SYSROOT}/usr/include/rockchip/
	)

link_libraries(lvgl pthread m freetype)

if (LV_DRV_USE_SDL_GPU)
	add_definitions(-DUSE_SDL_GPU=1)
	link_libraries(SDL2)
endif()

if (LV_DRV_USE_RKADK)
	add_definitions(-DUSE_RKADK=1 -DUSE_EVDEV=1)
	link_libraries(rkadk rockit evdev)
	if(LVGL_DEMO_RK3506)
		link_libraries(rga)
	endif()
endif()

if (LV_DRV_USE_DRM)
	add_definitions(-DUSE_DRM=1 -DUSE_EVDEV=1)
	link_libraries(drm evdev)
endif()

if (LV_USE_RGA)
	link_libraries(rga)
endif()

if (LVGL_DEMO_RK3506)
	add_definitions(-DUSE_RK3506=1)
endif()

if (LVGL_V9)
	add_definitions(-DLVGL_V9=1)
	include_directories(${PROJECT_SOURCE_DIR}/lvgl9/)
	aux_source_directory(${PROJECT_SOURCE_DIR}/lvgl9 SRCS)
else()
	include_directories(${PROJECT_SOURCE_DIR}/lvgl8/)
	aux_source_directory(${PROJECT_SOURCE_DIR}/lvgl8 SRCS)
	link_libraries(lv_drivers)
endif()
aux_source_directory(${PROJECT_SOURCE_DIR}/sys SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/cJSON SRCS)
aux_source_directory(${PROJECT_SOURCE_DIR}/common SRCS)

if (LV_DRV_USE_OPENGL)
	pkg_check_modules(PKG_GL glesv2)
	link_libraries(${PKG_GL_LIBRARIES})
endif()

if (LV_USE_RK_DEMO)
	add_subdirectory(rk_demo)
#	add_subdirectory(gallery)
#	add_subdirectory(amp_monitor)
#	add_subdirectory(flexbus)
#	add_subdirectory(motor_demo)
else()
	add_subdirectory(lv_demo)
endif()

if (EXISTS "${PROJECT_SOURCE_DIR}/tools/pre-commit")
	set(GIT_HOOK_SRC "${PROJECT_SOURCE_DIR}/tools/pre-commit")
	if(EXISTS "${PROJECT_SOURCE_DIR}/.git/hooks")
		set(GIT_HOOK_DST "${PROJECT_SOURCE_DIR}/.git/hooks/")
		file(COPY ${GIT_HOOK_SRC} DESTINATION ${GIT_HOOK_DST})
		message(STATUS "Install git hooks done")
	endif()
endif()

