file(GLOB SOL_SOURCES config.hpp forward.hpp sol.hpp)

add_library(libsol INTERFACE ${SOL_SOURCES})
# HACK: "/.." here is needed to avoid custom logic in Makefile.
#       Get rid of it once we get rid of Make, and move sol sources into subdirectory.
target_include_directories(libsol SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(libsol INTERFACE liblua)
