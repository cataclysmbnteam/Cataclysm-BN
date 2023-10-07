# SPDX-License-Identifier: GPL-3.0-only
# Author: 2023 scarf <greenscarf005@gmail.com>

function (setup_library TARGET)

    add_library(${TARGET} OBJECT ${CATACLYSM_BN_SOURCES} ${CATACLYSM_BN_HEADERS} ${LUA_C_SOURCES})
    target_include_directories(${TARGET} INTERFACE ${CMAKE_SOURCE_DIR}/src)

    add_dependencies(${TARGET} get_version)

    if (USE_PCH_HEADER)
        target_precompile_headers(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/pch/main-pch.hpp)
    endif ()

    if (CMAKE_USE_PTHREADS_INIT)
        target_compile_options(${TARGET} PUBLIC "-pthread")
    endif ()

    if (CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})
    endif ()

    if (LUA)
        target_compile_definitions(${TARGET} PUBLIC LUA)
        target_include_directories(${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/src)
        target_include_directories(${TARGET} PUBLIC ${CMAKE_SOURCE_DIR}/src/lua)
    endif ()

    if (WIN32)
        # Global settings for Windows targets (at end)
        target_link_libraries(${TARGET} gdi32.lib)
        target_link_libraries(${TARGET} winmm.lib)
        target_link_libraries(${TARGET} imm32.lib)
        target_link_libraries(${TARGET} ole32.lib)
        target_link_libraries(${TARGET} oleaut32.lib)
        target_link_libraries(${TARGET} version.lib)

        if (BACKTRACE)
            target_link_libraries(${TARGET} dbghelp.lib)
        endif ()
    endif ()

    if (LIBBACKTRACE)
        target_link_libraries(${TARGET} backtrace)
    endif ()

endfunction ()
