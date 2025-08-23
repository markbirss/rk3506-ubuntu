cmake_minimum_required(VERSION 3.8)

project(lv_demo)

include_directories(${PROJECT_SOURCE_DIR}/./)

if (LVGL_DEMO_WIDGETS)
	add_definitions(-DUSE_DEMO_WIDGETS=1)
	set(LINK_LVGL_DEMOS ON)
elseif (LVGL_DEMO_BENCHMARK)
	add_definitions(-DUSE_DEMO_BENCHMARK=1)
	set(LINK_LVGL_DEMOS ON)
elseif (LVGL_DEMO_MUSIC)
	add_definitions(-DUSE_DEMO_MUSIC=1)
	set(LINK_LVGL_DEMOS ON)
endif()

aux_source_directory(. SRCS)

add_executable(${PROJECT_NAME} ${SRCS})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(PROGRAMS S10lv_demo DESTINATION /etc/init.d)

