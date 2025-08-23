#1.cmake version
cmake_minimum_required(VERSION 3.2)

#2.project name
project(RKAUDIOALGORITHMS)

# ----------------------------------------------------------------------------
#  install resource
# ----------------------------------------------------------------------------
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/conf/
	DESTINATION "vqefiles"
	FILES_MATCHING PATTERN "*.json"
)

# ----------------------------------------------------------------------------
#  install headers
# ----------------------------------------------------------------------------
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION "include"
	FILES_MATCHING PATTERN "*.h"
)

# ----------------------------------------------------------------------------
#  install libs
# ----------------------------------------------------------------------------
if (RV1103B)
set(uclibc_src uclibc_rv1103b)
else()
set(uclibc_src uclibc)
endif()
if (USE_UCLIBC)
	if(RKAPPLUS_AED_ENABLE OR RKAPPLUS_ENABLE)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_common.so
				${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_common.a
			DESTINATION "lib"
		)
		install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/conf/
			DESTINATION "vqefiles"
			FILES_MATCHING PATTERN "*.rknn"
		)
		if (RV1103B)
			install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/conf_rv1103b/
				DESTINATION "vqefiles"
				FILES_MATCHING PATTERN "*.*"
			)
		endif()
	endif()

	if(RKAPPLUS_ENABLE)
		if (RKAPPLUS_WAKEUP_ENABLE)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/libaec_bf_process_wakeup.so
				DESTINATION "lib"
				RENAME librkaudio_vqe.so
			)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/libaec_bf_process_wakeup.a
				DESTINATION "lib"
				RENAME librkaudio_vqe.a
			)
	elseif (RKAPPLUS_AINR_ENABLE)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_vqe_ainr.so
				DESTINATION "lib"
				RENAME librkaudio_vqe.so
			)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_vqe_ainr.a
				DESTINATION "lib"
				RENAME librkaudio_vqe.a
			)
        elseif (RKAPPLUS_WAKEUP_MODE1_NN_ENABLE)
            install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../../../iva/iva/librockiva/rockiva-rv1106-Linux/lib/librknnmrt.so
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../../iva/iva/librockiva/rockiva-rv1106-Linux/lib/librknnmrt.a
				DESTINATION "lib"
            )
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/libaec_bf_process_wakeup_mode1_nn.so
				DESTINATION "lib"
				RENAME libaec_bf_process.so
			)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/libaec_bf_process_wakeup_mode1_nn.a
				DESTINATION "lib"
				RENAME libaec_bf_process.a
            )
        elseif (RKAPPLUS_WAKEUP_MODE2_NN_ENABLE)
            install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../../../../iva/iva/librockiva/rockiva-rv1106-Linux/lib/librknnmrt.so
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../../iva/iva/librockiva/rockiva-rv1106-Linux/lib/librknnmrt.a
				DESTINATION "lib"
			)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/libaec_bf_process_wakeup_mode2_nn.so
				DESTINATION "lib"
				RENAME libaec_bf_process.so
			)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/libaec_bf_process_wakeup_mode2_nn.a
				DESTINATION "lib"
				RENAME libaec_bf_process.a
            )
		else()
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_vqe.so
				${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_vqe.a
				DESTINATION "lib"
			)
		endif()
	endif()

	if (RKAPPLUS_AED_ENABLE)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_detect.so
				${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_detect.a
			DESTINATION "lib"
		)
	endif()

	if (RKAPPLUS_EFFECT_EQDRC_ENABLE)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_effect_eqdrc.so
			      ${CMAKE_CURRENT_SOURCE_DIR}/${uclibc_src}/librkaudio_effect_eqdrc.a
			DESTINATION "lib"
		)
		install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/conf/
			DESTINATION "vqefiles"
			FILES_MATCHING PATTERN "*.bin"
		)
	endif()
endif()

if (USE_32BIT)
	if(RKAPPLUS_AED_ENABLE OR RKAPPLUS_ENABLE)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib32/librkaudio_common.so
				${CMAKE_CURRENT_SOURCE_DIR}/lib32/librkaudio_common.a
			DESTINATION "lib"
		)
	endif()

	if(RKAPPLUS_ENABLE)
	    if (RKAPPLUS_WAKEUP_ENABLE)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib32/libaec_bf_process_wakeup.so
				DESTINATION "lib"
				RENAME librkaudio_vqe.so
			)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib32/libaec_bf_process_wakeup.a
				DESTINATION "lib"
				RENAME librkaudio_vqe.a
			)
		else()
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib32/librkaudio_vqe.so
				${CMAKE_CURRENT_SOURCE_DIR}/lib32/librkaudio_vqe.a
				DESTINATION "lib"
			)
		endif()
	endif()

	if (RKAPPLUS_AED_ENABLE)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib32/librkaudio_detect.so
				${CMAKE_CURRENT_SOURCE_DIR}/lib32/librkaudio_detect.a
			DESTINATION "lib"
	)
	endif()

	if (RKAPPLUS_EFFECT_EQDRC_ENABLE)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib32/librkaudio_effect_eqdrc.so
			      ${CMAKE_CURRENT_SOURCE_DIR}/lib32/librkaudio_effect_eqdrc.a
			DESTINATION "lib"
		)
		install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/conf/
			DESTINATION "vqefiles"
			FILES_MATCHING PATTERN "*.bin"
		)
	endif()

endif()

if (USE_64BIT)
	if(RKAPPLUS_AED_ENABLE OR RKAPPLUS_ENABLE)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib64/librkaudio_common.so
				${CMAKE_CURRENT_SOURCE_DIR}/lib64/librkaudio_common.a
			DESTINATION "lib"
		)
	endif()

	if(RKAPPLUS_ENABLE)
		if (RKAPPLUS_WAKEUP_ENABLE)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib32/libaec_bf_process_wakeup.so
				DESTINATION "lib"
				RENAME librkaudio_vqe.so
			)
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib64/libaec_bf_process_wakeup.a
				DESTINATION "lib"
				RENAME librkaudio_vqe.a
			)
		else()
			install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib64/librkaudio_vqe.so
				${CMAKE_CURRENT_SOURCE_DIR}/lib64/librkaudio_vqe.a
				DESTINATION "lib"
			)
		endif()
	endif()

	if (RKAPPLUS_AED_ENABLE)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib64/librkaudio_detect.so
				${CMAKE_CURRENT_SOURCE_DIR}/lib64/librkaudio_detect.a
			DESTINATION "lib"
	)
	endif()

	if (RKAPPLUS_EFFECT_EQDRC_ENABLE)
		install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/lib64/librkaudio_effect_eqdrc.so
			      ${CMAKE_CURRENT_SOURCE_DIR}/lib64/librkaudio_effect_eqdrc.a
			DESTINATION "lib"
		)
		install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/conf/
			DESTINATION "vqefiles"
			FILES_MATCHING PATTERN "*.bin"
		)
	endif()
endif()
