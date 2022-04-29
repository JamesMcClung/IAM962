add_executable(project2a src/project2a.cc)
target_link_libraries(project2a linalg)
target_include_directories(project2a PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

# Allow for setting parameters via make, e.g.:
#   `make project2a BUILDPARAMS="NX=128 DT=.1"`
set(BUILDPARAMS_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/buildparams.h)

add_custom_target(p2a_buildparams COMMAND python3 ${PROJECT_SOURCE_DIR}/python/generate_buildparams.py ${BUILDPARAMS_FILE} \${BUILDPARAMS})
add_dependencies(project2a p2a_buildparams)