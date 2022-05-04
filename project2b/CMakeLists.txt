add_executable(project2b src/project2b.cc)
target_link_libraries(project2b linalg)
target_include_directories(project2b PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

# Allow for setting parameters via make, e.g.:
#   `make project2b BUILDPARAMS="NX=128 DT=.1"`
set(BUILDPARAMS_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/buildparams.h)

add_custom_target(p2b_buildparams COMMAND python3 ${PROJECT_SOURCE_DIR}/python/generate_buildparams.py ${BUILDPARAMS_FILE} \${BUILDPARAMS})
add_dependencies(project2b p2b_buildparams)