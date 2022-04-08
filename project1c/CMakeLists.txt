add_executable(project1c src/project1c.cc)
target_link_libraries(project1c linalg)
target_include_directories(project1c PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

# Allow for setting parameters via make, e.g.:
#   `make project1c BUILDPARAMS="NX=128 DT=.1"`
set(BUILDPARAMS_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/buildparams.h)

add_custom_target(p1c_buildparams COMMAND python3 ${PROJECT_SOURCE_DIR}/python/generate_buildparams.py ${BUILDPARAMS_FILE} \${BUILDPARAMS})
add_dependencies(project1c p1c_buildparams)