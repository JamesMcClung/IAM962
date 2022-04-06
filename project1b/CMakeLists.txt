add_executable(project1b src/project1b.cc)
target_link_libraries(project1b linalg)
target_include_directories(project1b PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

# Allow for setting parameters via make, e.g.:
#   `make project1b BUILDPARAMS="NX=128 DT=.1"`
set(BUILDPARAMS_FILE ${CMAKE_CURRENT_BINARY_DIR}/src/buildparams.h)

add_custom_target(p1b_buildparams COMMAND python3 ${PROJECT_SOURCE_DIR}/python/generate_buildparams.py ${BUILDPARAMS_FILE} \${BUILDPARAMS})
add_dependencies(project1b p1b_buildparams)