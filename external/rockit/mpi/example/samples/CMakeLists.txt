cmake_minimum_required( VERSION 2.8.8 )
set(RK_MPI_TEST_APLAY_SRC
    rk_mpi_aplay_test.cpp
)

set(RK_MPI_TEST_VPLAY_SRC
    rk_mpi_vplay_test.cpp
)

set(RK_MPI_TEST_ACAPTURE_SRC
    rk_mpi_acapture_test.cpp
)

set(RK_MPI_TEST_VWRAP_SRC
    rk_mpi_vwrap_test.c
)



#--------------------------
# rk_mpi_wrap_test
#--------------------------
add_executable(rk_mpi_vwrap_test ${RK_MPI_TEST_VWRAP_SRC} ${RK_MPI_TEST_COMMON_SRC})
target_link_libraries(rk_mpi_vwrap_test ${ROCKIT_DEP_COMMON_LIBS})
install(TARGETS rk_mpi_vwrap_test RUNTIME DESTINATION "bin")


