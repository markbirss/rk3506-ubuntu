cmake_minimum_required(VERSION 3.8)

project(motor_demo)

if (LVGL_V9)
	MESSAGE(FATAL_ERROR "This app no support LVGL9, please switch to LVGL8")
endif()

include_directories(${PROJECT_SOURCE_DIR}/)

link_directories(./)

aux_source_directory(. SRCS)

add_executable(${PROJECT_NAME} ${SRCS})

#This demo only provides ui display. For details about the prototype library, consult RK
#target_link_libraries(${PROJECT_NAME}
#	madht1505ba1 ethercat
#)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY resource DESTINATION share)
