cmake_minimum_required(VERSION 3.8)

project(gallery)

if (LVGL_V9)
	MESSAGE(FATAL_ERROR "This app no support LVGL9, please switch to LVGL8")
endif()

include_directories(${PROJECT_SOURCE_DIR}/./)

aux_source_directory(. SRCS)
aux_source_directory(pics SRCS)
aux_source_directory(anims SRCS)

add_executable(${PROJECT_NAME} ${SRCS})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

