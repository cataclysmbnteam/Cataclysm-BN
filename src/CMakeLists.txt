# Cataclysm BN client
cmake_minimum_required(VERSION 3.16)

include(SetupLibrary.cmake)

set(MAIN_CPP ${CMAKE_SOURCE_DIR}/src/main.cpp)
set(MESSAGES_CPP ${CMAKE_SOURCE_DIR}/src/messages.cpp)
set(RESOURCE_RC ${CMAKE_SOURCE_DIR}/src/resource.rc)

file(GLOB CATACLYSM_BN_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

list(REMOVE_ITEM CATACLYSM_BN_SOURCES ${MAIN_CPP} ${MESSAGES_CPP})

file(GLOB CATACLYSM_BN_HEADERS ${CMAKE_SOURCE_DIR}/src/*.h)

if (LUA)
    file(GLOB LUA_C_SOURCES ${CMAKE_SOURCE_DIR}/src/lua/*.c)
else ()
    set(LUA_C_SOURCES "")
endif ()

# Get GIT version strings
add_custom_target(
    get_version
    DEPENDS ${CMAKE_SOURCE_DIR}/src/version.h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/src/version.h
    COMMAND
        ${CMAKE_COMMAND} -D SRC=${CMAKE_SOURCE_DIR}/src/version.h.in -D
        DST=${CMAKE_SOURCE_DIR}/src/version.h -D GIT_EXECUTABLE=${GIT_EXECUTABLE} -P
        ${CMAKE_SOURCE_DIR}/src/version.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# Build tiles version if requested
if (TILES)
    setup_library(cataclysm-tiles-common)

    if (WIN32)
        add_definitions(-DUSE_WINMAIN)
        add_executable(cataclysm-tiles WIN32 ${MAIN_CPP} ${MESSAGES_CPP} ${RESOURCE_RC})
    else ()
        add_executable(cataclysm-tiles ${MAIN_CPP} ${MESSAGES_CPP})
    endif ()

    target_link_libraries(cataclysm-tiles cataclysm-tiles-common)
    target_compile_definitions(cataclysm-tiles-common PUBLIC TILES)

    if (NOT DYNAMIC_LINKING)
        # SDL, SDL_Image, SDL_ttf deps are required for static build
        target_include_directories(
            cataclysm-tiles-common
            PUBLIC ${FREETYPE_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS} ${JPEG_INCLUDE_DIR}
                   ${ZLIB_INCLUDE_DIRS} ${BZIP2_INCLUDE_DIR})
        target_link_libraries(cataclysm-tiles-common ${FREETYPE_LIBRARIES} ${PNG_LIBRARIES}
                              ${JPEG_LIBRARIES} ${ZLIB_LIBRARIES} ${BZIP2_LIBRARIES})
    endif ()

    target_include_directories(
        cataclysm-tiles-common PUBLIC ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIRS}
                                      ${SDL2_TTF_INCLUDE_DIRS})
    target_link_libraries(cataclysm-tiles-common ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES}
                          ${SDL2_TTF_LIBRARIES})

    if (SOUND)
        target_compile_definitions(cataclysm-tiles-common PUBLIC SDL_SOUND)
        target_include_directories(cataclysm-tiles-common PUBLIC ${OGGVORBIS_INCLUDE_DIR})
        target_link_libraries(cataclysm-tiles-common ${OGG_LIBRARY})
        target_link_libraries(cataclysm-tiles-common ${VORBIS_LIBRARY})
        target_link_libraries(cataclysm-tiles-common ${VORBISFILE_LIBRARY})
        target_include_directories(cataclysm-tiles-common PUBLIC ${SDL2_MIXER_INCLUDE_DIRS})
        target_link_libraries(cataclysm-tiles-common ${SDL2_MIXER_LIBRARIES})
    endif ()

    if (RELEASE)
        install(TARGETS cataclysm-tiles DESTINATION ${BIN_PREFIX})
    endif ()

endif ()

# Build curses version if requested
if (CURSES)
    setup_library(cataclysm-common)

    if (WIN32)
        add_executable(cataclysm ${MAIN_CPP} ${MESSAGES_CPP} ${RESOURCE_RC})
    else ()
        add_executable(cataclysm ${MAIN_CPP} ${MESSAGES_CPP})
    endif ()

    target_link_libraries(cataclysm cataclysm-common)

    target_include_directories(cataclysm-common PUBLIC ${CURSES_INCLUDE_DIR})
    target_link_libraries(cataclysm-common ${CURSES_LIBRARIES})

    if (RELEASE)
        install(TARGETS cataclysm DESTINATION ${BIN_PREFIX})
    endif ()

endif ()

if (MINGW AND NOT RELEASE)
    # Try to Install shared libraries for dev builds
    # Note: It is specific to MSYS2 and uses hardcoded versions so
    # probably it will fail if you run it :)

    # GCC-specific libraries
    find_library(RuntimeLib_GCC_S_DW2_1 "gcc_s_dw2-1")
    find_library(RuntimeLib_STDC_PP_6 "stdc++-6")
    find_library(RuntimeLib_WINPTHREAD_1 "winpthread-1")
    set(RuntimeLib_GCC_ALL ${RuntimeLib_GCC_S_DW2_1} ${RuntimeLib_STDC_PP_6}
                           ${RuntimeLib_WINPTHREAD_1})

    if (TILES)
        # SDL2 can have a varius deps. Here you are the MSYS2 ones...
        find_library(RuntimeLib_SDL2 "SDL2")
        find_library(RuntimeLib_SDL2_IMG "SDL2_image")
        find_library(RuntimeLib_png "libpng16-16")
        find_library(RuntimeLib_jpeg "libjpeg-8")
        find_library(RuntimeLib_jbig "libjbig-0")
        find_library(RuntimeLib_tiff "libtiff-5")
        find_library(RuntimeLib_webp "libwebp-5")
        find_library(RuntimeLib_lzma "liblzma-5")
        find_library(RuntimeLib_bz2 "libbz2-1")
        find_library(RuntimeLib_zlib "zlib1")
        find_library(RuntimeLib_hb "libharfbuzz-0")
        find_library(RuntimeLib_SDL2_TTF "SDL2_ttf")
        find_library(RuntimeLib_ft "libfreetype-6")
        find_library(RuntimeLib_glib "libglib-2.0-0")
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
            find_library(RuntimeLib_SDL_SND "SDL2_mixer")
            find_library(RuntimeLib_flak "libFLAC-8")
            find_library(RuntimeLib_ogg "libogg-0")
            find_library(RuntimeLib_flu "libfluidsynth-1")
            find_library(RuntimeLib_port "libportaudio-2")
            find_library(RuntimeLib_snd "libsndfile-1")
            find_library(RuntimeLib_vorb "libvorbis-0")
            find_library(RuntimeLib_vorb_enc "libvorbisenc-2")
            find_library(RuntimeLib_vorb_f "libvorbisfile-3")
            find_library(RuntimeLib_mod "libmodplug-1")
            find_library(RuntimeLib_mpeg "smpeg2")
            set(RuntimeLib_SDL_SOUND
                ${RuntimeLib_SDL_SND}
                ${RuntimeLib_flak}
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

    install(FILES ${RuntimeLib_GCC_ALL} ${RuntimeLib_SDL} ${RuntimeLib_SDL_SOUND}
            DESTINATION ${BIN_PREFIX})
endif ()

# vim:noet
