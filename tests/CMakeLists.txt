if (BUILD_TESTING)
    file(GLOB CATACLYSM_BN_TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/*.cpp)

    # Enabling benchmarks
    add_definitions(-DCATCH_CONFIG_ENABLE_BENCHMARKING)

    # TODO: build MO files required for tests

    if (TILES)
        add_executable(cata_test-tiles ${CATACLYSM_BN_TEST_SOURCES})
        if (LUA)
            target_compile_definitions(cata_test-tiles PUBLIC LUA)
        endif ()
        target_link_libraries(cata_test-tiles cataclysm-tiles-common)
        add_test(
            NAME test-tiles
            COMMAND sh -c "$<TARGET_FILE:cata_test-tiles> --rng-seed time"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif ()

    if (CURSES)
        add_executable(cata_test ${CATACLYSM_BN_TEST_SOURCES})
        if (LUA)
            target_compile_definitions(cata_test PUBLIC LUA)
        endif ()
        target_link_libraries(cata_test cataclysm-common)
        add_test(
            NAME test
            COMMAND sh -c "$<TARGET_FILE:cata_test> --rng-seed time"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif ()
endif ()

# vim:noet
