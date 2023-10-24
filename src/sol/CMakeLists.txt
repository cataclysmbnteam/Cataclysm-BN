file(GLOB SOL_SOURCES sol/config.hpp sol/forward.hpp sol/sol.hpp)

add_library(libsol INTERFACE ${SOL_SOURCES})
target_include_directories(libsol INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libsol INTERFACE liblua)
