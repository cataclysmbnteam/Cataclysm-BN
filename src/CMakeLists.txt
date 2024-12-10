set(MAIN_CPP ${CMAKE_SOURCE_DIR}/src/main.cpp)
set(MESSAGES_CPP ${CMAKE_SOURCE_DIR}/src/messages.cpp)
set(RESOURCE_RC ${CMAKE_SOURCE_DIR}/src/resource.rc)

file(GLOB CATACLYSM_BN_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

list(REMOVE_ITEM CATACLYSM_BN_SOURCES ${MAIN_CPP} ${MESSAGES_CPP})

file(GLOB CATACLYSM_BN_HEADERS ${CMAKE_SOURCE_DIR}/src/*.h)

if (LUA)
    add_subdirectory(lua)
    add_subdirectory(sol)
endif ()

# Get GIT version strings
add_custom_target(
    get_version
    DEPENDS ${CMAKE_SOURCE_DIR}/src/version.h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/version.h
    COMMAND ${CMAKE_COMMAND}
            -D SRC=${CMAKE_SOURCE_DIR}/src/version.h.in
            -D DST=${CMAKE_SOURCE_DIR}/src/version.h
            -D GIT_EXECUTABLE=${GIT_EXECUTABLE}
            -P ${CMAKE_SOURCE_DIR}/src/version.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# Build tiles version if requested
if (TILES)
    add_library(
            cataclysm-bn-tiles-common OBJECT
            ${CATACLYSM_BN_SOURCES}
            ${CATACLYSM_BN_HEADERS})
    target_include_directories(cataclysm-bn-tiles-common INTERFACE ${CMAKE_SOURCE_DIR}/src)

    if (WIN32)
        add_definitions(-DUSE_WINMAIN)
        add_executable(
                cataclysm-bn-tiles WIN32
                ${MAIN_CPP}
                ${MESSAGES_CPP}
                ${RESOURCE_RC})
    else ()
        add_executable(cataclysm-bn-tiles
                ${MAIN_CPP}
                ${MESSAGES_CPP})
    endif ()

    if (LUA)
        target_compile_definitions(cataclysm-bn-tiles-common PUBLIC LUA)
            if (UNIX)
                target_compile_definitions(cataclysm-bn-tiles-common PUBLIC LUA_USE_LINUX)
            elseif (APPLE)
                target_compile_definitions(cataclysm-bn-tiles-common PUBLIC LUA_USE_MACOSX)
            endif()
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC libsol)
    endif ()

    add_dependencies(cataclysm-bn-tiles-common get_version)

    target_link_libraries(cataclysm-bn-tiles-common PUBLIC ZLIB::ZLIB)
    target_link_libraries(cataclysm-bn-tiles-common PUBLIC SQLite::SQLite3)
    target_link_libraries(cataclysm-bn-tiles PRIVATE cataclysm-bn-tiles-common)
    target_compile_definitions(cataclysm-bn-tiles-common PUBLIC TILES )

    if (NOT "${CMAKE_EXPORT_COMPILE_COMMANDS}")
        target_precompile_headers(cataclysm-bn-tiles-common PRIVATE
            ${CMAKE_SOURCE_DIR}/pch/main-pch.hpp)
    endif ()

    if (CMAKE_USE_PTHREADS_INIT)
        target_compile_options(cataclysm-bn-tiles-common PUBLIC "-pthread")
    endif ()

    if (CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    endif ()

    if (NOT DYNAMIC_LINKING)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC
            SDL2::SDL2-static
            SDL2_image::SDL2_image-static
            SDL2_ttf::SDL2_ttf-static
        )
    else()
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC
            SDL2::SDL2
            SDL2_image::SDL2_image
            SDL2_ttf::SDL2_ttf
        )
    endif ()
    if (SOUND)
        if (NOT DYNAMIC_LINKING)
            target_link_libraries(cataclysm-bn-tiles-common PUBLIC
                SDL2_mixer::SDL2_mixer-static
            )
        else()
            target_link_libraries(cataclysm-bn-tiles-common PUBLIC
                SDL2_mixer::SDL2_mixer
            )
        endif()
        add_definitions(-DSDL_SOUND)
    endif ()

    if (WIN32)
        # Global settings for Windows targets (at end)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC gdi32.lib)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC winmm.lib)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC imm32.lib)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC ole32.lib)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC oleaut32.lib)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC version.lib)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC setupapi.lib)
        if (SOUND)
            target_link_libraries(cataclysm-bn-tiles-common PUBLIC shlwapi.lib)
        endif()
        if (BACKTRACE)
            target_link_libraries(cataclysm-bn-tiles-common PUBLIC dbghelp.lib)
            if (LIBBACKTRACE)
                target_link_libraries(cataclysm-bn-tiles-common PUBLIC backtrace)
            endif ()
        endif ()
    elseif (APPLE)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC "-framework CoreFoundation")
    endif ()

    if (LIBBACKTRACE)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC backtrace)
    endif ()

    if (USE_TRACY)
        target_link_libraries(cataclysm-bn-tiles-common PUBLIC TracyClient)
        target_include_directories(cataclysm-bn-tiles-common SYSTEM PUBLIC ${tracy_SOURCE_DIR}/public)
        target_compile_definitions(cataclysm-bn-tiles-common PUBLIC USE_TRACY)
    endif ()

    if (RELEASE)
        install(TARGETS cataclysm-bn-tiles RUNTIME)
    endif ()

    set_target_properties( cataclysm-bn-tiles PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" )
endif ()

# Build curses version if requested
if (CURSES)
    add_library(cataclysm-bn-common OBJECT
            ${CATACLYSM_BN_SOURCES}
            ${CATACLYSM_BN_HEADERS})
    target_include_directories(cataclysm-bn-common INTERFACE ${CMAKE_SOURCE_DIR}/src)

    if (NOT "${CMAKE_EXPORT_COMPILE_COMMANDS}")
        target_precompile_headers(cataclysm-bn-common PRIVATE
            ${CMAKE_SOURCE_DIR}/pch/main-pch.hpp)
    endif ()

    if (WIN32)
        add_executable(cataclysm-bn
                ${MAIN_CPP}
                ${MESSAGES_CPP}
                ${RESOURCE_RC})
    else ()
        add_executable(cataclysm-bn
                ${MAIN_CPP}
                ${MESSAGES_CPP})
    endif ()

    if (LUA)
        target_compile_definitions(cataclysm-bn-common PUBLIC LUA)
        if (UNIX)
            target_compile_definitions(cataclysm-bn-common PUBLIC LUA_USE_LINUX)
        elseif (APPLE)
            target_compile_definitions(cataclysm-bn-common PUBLIC LUA_USE_MACOSX)
        endif()
        target_link_libraries(cataclysm-bn-common PUBLIC libsol)
    endif ()

    add_dependencies(cataclysm-bn-common get_version)

    target_link_libraries(cataclysm-bn PRIVATE cataclysm-bn-common)

    target_include_directories(cataclysm-bn-common PUBLIC ${CURSES_INCLUDE_DIR})
    target_link_libraries(cataclysm-bn-common PUBLIC ZLIB::ZLIB)
    target_link_libraries(cataclysm-bn-common PUBLIC SQLite::SQLite3)
    target_link_libraries(cataclysm-bn-common PUBLIC ${CURSES_LIBRARIES})

    if (CMAKE_USE_PTHREADS_INIT)
        target_compile_options(cataclysm-bn-common PUBLIC "-pthread")
    endif ()

    if (CMAKE_THREAD_LIBS_INIT)
        target_link_libraries(cataclysm-bn-common PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    endif ()

    if (WIN32)
        # Global settings for Windows targets (at end)
        target_link_libraries(cataclysm-bn-common PUBLIC gdi32.lib)
        target_link_libraries(cataclysm-bn-common PUBLIC winmm.lib)
        target_link_libraries(cataclysm-bn-common PUBLIC imm32.lib)
        target_link_libraries(cataclysm-bn-common PUBLIC ole32.lib)
        target_link_libraries(cataclysm-bn-common PUBLIC oleaut32.lib)
        target_link_libraries(cataclysm-bn-common PUBLIC version.lib)
        if (BACKTRACE)
            target_link_libraries(cataclysm-bn-common PUBLIC dbghelp.lib)
        endif ()
    elseif (APPLE)
        target_link_libraries(cataclysm-bn-common PUBLIC "-framework CoreFoundation")
    endif ()

    if (LIBBACKTRACE)
        target_link_libraries(cataclysm-bn-common PUBLIC backtrace)
    endif ()

    if (USE_TRACY)
        target_link_libraries(cataclysm-bn-common PUBLIC TracyClient)
        target_include_directories(cataclysm-bn-common SYSTEM PUBLIC ${tracy_SOURCE_DIR}/public)
        target_compile_definitions(cataclysm-bn-common PUBLIC USE_TRACY)
    endif ()

    if (RELEASE)
        install(TARGETS cataclysm-bn RUNTIME)
    endif ()
endif ()

if (MINGW AND NOT RELEASE)
    # Try to Install shared libraries for dev builds
    # Note: It is specific to MSYS2 and uses hardcoded versions so
    # probably it will fail if you run it :)

    # GCC-specific libraries
    find_library(RuntimeLib_GCC_S_DW2_1  "gcc_s_dw2-1")
    find_library(RuntimeLib_STDC_PP_6    "stdc++-6")
    find_library(RuntimeLib_WINPTHREAD_1 "winpthread-1")

    set(RuntimeLib_GCC_ALL
            ${RuntimeLib_GCC_S_DW2_1}
            ${RuntimeLib_STDC_PP_6}
            ${RuntimeLib_WINPTHREAD_1})

    if (TILES)
        # SDL2 can have a varius deps. Here you are the MSYS2 ones...
        find_library(RuntimeLib_SDL2  "SDL2")
        find_library(RuntimeLib_SDL2_IMG  "SDL2_image")
        find_library(RuntimeLib_png  "libpng16-16")
        find_library(RuntimeLib_jpeg  "libjpeg-8")
        find_library(RuntimeLib_jbig  "libjbig-0")
        find_library(RuntimeLib_tiff  "libtiff-5")
        find_library(RuntimeLib_webp  "libwebp-5")
        find_library(RuntimeLib_lzma  "liblzma-5")
        find_library(RuntimeLib_bz2  "libbz2-1")
        find_library(RuntimeLib_zlib  "zlib1")
        find_library(RuntimeLib_hb  "libharfbuzz-0")
        find_library(RuntimeLib_SDL2_TTF  "SDL2_ttf")
        find_library(RuntimeLib_ft  "libfreetype-6")
        find_library(RuntimeLib_glib  "libglib-2.0-0")
        set(RuntimeLib_SDL
            ${RuntimeLib_SDL2}
            ${RuntimeLib_SDL2_IMG}
            ${RuntimeLib_png}
            ${RuntimeLib_jpeg}
            ${RuntimeLib_jbig}
            ${RuntimeLib_tiff}
            ${RuntimeLib_webp}
            ${RuntimeLib_lzma}
            ${RuntimeLib_bz2}
            ${RuntimeLib_zlib}
            ${RuntimeLib_hb}
            ${RuntimeLib_SDL2_TTF}
            ${RuntimeLib_ft}
            ${RuntimeLib_glib})

        if (SOUND)
            find_library(RuntimeLib_SDL_SND  "SDL2_mixer")
            find_library(RuntimeLib_flac  "libFLAC-8")
            find_library(RuntimeLib_ogg  "libogg-0")
            find_library(RuntimeLib_flu  "libfluidsynth-1")
            find_library(RuntimeLib_port  "libportaudio-2")
            find_library(RuntimeLib_snd  "libsndfile-1")
            find_library(RuntimeLib_vorb  "libvorbis-0")
            find_library(RuntimeLib_vorb_enc  "libvorbisenc-2")
            find_library(RuntimeLib_vorb_f  "libvorbisfile-3")
            find_library(RuntimeLib_mod  "libmodplug-1")
            find_library(RuntimeLib_mpeg  "smpeg2")
            set(RuntimeLib_SDL_SOUND
                ${RuntimeLib_SDL_SND}
                ${RuntimeLib_flac}
                ${RuntimeLib_ogg}
                ${RuntimeLib_flu}
                ${RuntimeLib_port}
                ${RuntimeLib_snd}
                ${RuntimeLib_vorb}
                ${RuntimeLib_vorb_enc}
                ${RuntimeLib_vorb_f}
                ${RuntimeLib_mod}
                ${RuntimeLib_mpeg})
        endif ()
    endif ()
    install(FILES ${RuntimeLib_GCC_ALL}
            ${RuntimeLib_SDL}
            ${RuntimeLib_SDL_SOUND}
            TYPE BIN)
endif ()
