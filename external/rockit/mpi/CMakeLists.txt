cmake_minimum_required( VERSION 2.8.8 )
project (rockit)
include_directories(sdk/include)
include_directories(sdk/lib/${ARCH}/${CHIP_TYPE})
install(FILES sdk/lib/${ARCH}/${CHIP_TYPE}/rk_defines.h DESTINATION "include")
set(ROCKIT_DUMPSYS_FILE ${CMAKE_CURRENT_LIST_DIR}/example/bin/${ARCH}/${CHIP_TYPE}/dumpsys)
set(ROCKIT_DEP_COMMON_LIBS
    ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit.so
)
set(ROCKIT_DEP_COMMON_LIBS_STATIC
    ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit.a
)

if (CHIP_TYPE STREQUAL rv1106
    OR CHIP_TYPE STREQUAL rv1103
    OR CHIP_TYPE STREQUAL rv1103b)
    set(ROCKIT_DEP_COMMON_LIBS_FULL
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit_full.so
    )
    set(ROCKIT_DEP_COMMON_LIBS_TINY
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit_tiny.so
    )
    set(ROCKIT_DEP_COMMON_LIBS_STATIC_FULL
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit_full.a
    )
    set(ROCKIT_DEP_COMMON_LIBS_STATIC_TINY
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit_tiny.a
    )
else()
    set(ROCKIT_DEP_COMMON_LIBS_FULL
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit.so
    )
    set(ROCKIT_DEP_COMMON_LIBS_TINY
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit.so
    )
    set(ROCKIT_DEP_COMMON_LIBS_STATIC_FULL
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit.a
    )
    set(ROCKIT_DEP_COMMON_LIBS_STATIC_TINY
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/${ARCH}/${CHIP_TYPE}/linux/librockit.a
    )
endif()

if(ARCH STREQUAL arm64)
    set(ROCKIT_FILE_LIBS
        ${ROCKIT_DEP_COMMON_LIBS}
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/arm64/libgraphic_lsf.so
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/arm64/libRkSwCac.so
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/arm64/libturbojpeg.so
        ${CMAKE_CURRENT_LIST_DIR}/sdk/lib/arm64/libjpeg.so
    )
else()
    set(ROCKIT_FILE_LIBS
        ${ROCKIT_DEP_COMMON_LIBS}
        ${ROCKIT_DEP_COMMON_LIBS_STATIC}
    )
    if (CHIP_TYPE STREQUAL rv1106
        OR CHIP_TYPE STREQUAL rv1103
        OR CHIP_TYPE STREQUAL rv1103b)
        set(ROCKIT_FILE_LIBS
            ${ROCKIT_FILE_LIBS}
            ${ROCKIT_DEP_COMMON_LIBS_FULL}
            ${ROCKIT_DEP_COMMON_LIBS_TINY}
            ${ROCKIT_DEP_COMMON_LIBS_STATIC_FULL}
            ${ROCKIT_DEP_COMMON_LIBS_STATIC_TINY}
        )
    endif()
endif()

add_subdirectory(sdk)
if (CHIP_TYPE STREQUAL rk3308
    OR CHIP_TYPE STREQUAL rk3506)
else()
add_subdirectory(example)
endif()
