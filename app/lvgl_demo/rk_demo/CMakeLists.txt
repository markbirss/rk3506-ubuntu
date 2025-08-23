cmake_minimum_required(VERSION 3.8)

project(rk_demo)

if (LVGL_V9)
	MESSAGE(FATAL_ERROR "This app no support LVGL9, please switch to LVGL8")
endif()

add_definitions(-DMULTIMEDIA_EN=${RK_DEMO_MULTIMEDIA_EN})
add_definitions(-DUSE_SENSOR=${RK_DEMO_SENSOR_EN})
add_definitions(-DASR_EN=${RK_DEMO_ASR_EN})
add_definitions(-DWIFIBT_EN=${RK_DEMO_WIFIBT_EN})

include_directories(
	${PROJECT_SOURCE_DIR}/
	${PROJECT_SOURCE_DIR}/audio_algorithm/include
	${PROJECT_SOURCE_DIR}/home
	${PROJECT_SOURCE_DIR}/furniture_control
	${PROJECT_SOURCE_DIR}/setting
	${PROJECT_SOURCE_DIR}/smart_home
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/intercom_homepage
	${PROJECT_SOURCE_DIR}/intercom_homepage/video_monitor
	${PROJECT_SOURCE_DIR}/intercom_homepage/intercom_call
	${PROJECT_SOURCE_DIR}/rockit
	${PROJECT_SOURCE_DIR}/wifibt
	${PROJECT_SOURCE_DIR}/asr
	${PROJECT_SOURCE_DIR}/asr/lib
	${CMAKE_SYSROOT}/usr/include/rga/
	${CMAKE_SYSROOT}/usr/include/rkadk/
	)

link_directories(${PROJECT_SOURCE_DIR}/audio_algorithm/lib)

aux_source_directory(. SRCS)
aux_source_directory(./home SRCS)
aux_source_directory(./furniture_control SRCS)
aux_source_directory(./setting SRCS)
aux_source_directory(./smart_home SRCS)
aux_source_directory(./intercom_homepage SRCS)
aux_source_directory(./intercom_homepage/video_monitor SRCS)
aux_source_directory(./intercom_homepage/intercom_call SRCS)
aux_source_directory(./layout SRCS)
aux_source_directory(./wifibt SRCS)
aux_source_directory(./asr SRCS)

if (RK_DEMO_MULTIMEDIA_EN)
	aux_source_directory(./rockit SRCS)
endif()

add_executable(${PROJECT_NAME} ${SRCS})

if (RK_DEMO_WIFIBT_EN)
	target_link_libraries(${PROJECT_NAME} rkwifibt)
endif()

if (RK_DEMO_ASR_EN)
	target_link_libraries(${PROJECT_NAME} rkaudio_common rkaudio_vqe asound)
endif()

if (RK_DEMO_MULTIMEDIA_EN)
	target_link_libraries(${PROJECT_NAME} rockit rkadk)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(DIRECTORY resource DESTINATION share)
install(PROGRAMS S10lv_demo DESTINATION /etc/init.d)
if (NOT DEFINED LVGL_DEMO_RK3506)
	install(FILES asound.conf DESTINATION /etc)
endif()
if (RK_DEMO_ASR_EN)
	install(DIRECTORY asr/pcm DESTINATION share/resource)
endif()
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/audio_algorithm/lib/
	DESTINATION "lib"
	FILES_MATCHING PATTERN "*.so"
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/audio_algorithm/configs/
	DESTINATION "vqefiles"
	FILES_MATCHING PATTERN "*.json"
)
