add_executable(project1a src/project1a.cc)
target_link_libraries(project1a linalg)

set(P1A_NU 1 CACHE STRING "diffusivity constant: u_t = nu * u_xx")
set(P1A_NX 128 CACHE STRING "number of spatial gridpoints")
set(P1A_NT 1024 CACHE STRING "time steps to run for")
set(P1A_MIN_X 0 CACHE STRING "lower bound for spatial domain")
set(P1A_MAX_X 1 CACHE STRING "upper bound for spatial domain")
set(P1A_DT CACHE STRING "time between time steps; if unset, it is calculated")
set(P1A_MIN_X_BC 0 CACHE STRING "boundary condition for u(min_x)")
set(P1A_MAX_X_BC 0 CACHE STRING "boundary condition for u(max_x)")
set(P1A_WRITE_EVERY 8 CACHE STRING "write u every <this many> time steps")
set(P1A_OUT_FILE "out.csv" CACHE STRING "output file")

configure_file(src/config.txt src/config.hh)
target_include_directories(project1a PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

set(BUILDPARAMS_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/buildparams.h)

add_custom_target(buildparams COMMAND python3 ${PROJECT_SOURCE_DIR}/python/generate_buildparams.py ${BUILDPARAMS_FILE} \${BUILDPARAMS})
add_dependencies(project1a buildparams)