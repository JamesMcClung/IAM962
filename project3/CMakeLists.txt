add_executable(project3 src/project3.cc)
target_link_libraries(project3 linalg)
target_include_directories(project3 PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

# Allow for setting parameters via make, e.g.:
#   `make project3 BUILDPARAMS="NX=128 DT=.1"`
set(BUILDPARAMS_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/buildparams.h)

add_custom_target(p3_buildparams COMMAND python3 ${PROJECT_SOURCE_DIR}/python/generate_buildparams.py ${BUILDPARAMS_FILE} \${BUILDPARAMS})
add_dependencies(project3 p3_buildparams)