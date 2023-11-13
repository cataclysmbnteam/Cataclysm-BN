if (BUILD_TESTING)
    file(GLOB CATACLYSM_BN_TEST_SOURCES
            ${CMAKE_SOURCE_DIR}/tests/*.cpp)

    # Enabling benchmarks
    add_definitions(-DCATCH_CONFIG_ENABLE_BENCHMARKING)

    # TODO: build MO files required for tests

    # This needs to include catch.h with different macro switch
    set_source_files_properties(test_main.cpp PROPERTIES SKIP_PRECOMPILE_HEADERS ON)

    if (TILES)
        add_executable(cata_test-tiles ${CATACLYSM_BN_TEST_SOURCES})
        target_link_libraries(cata_test-tiles PRIVATE cataclysm-tiles-common)
        add_test(
            NAME test-tiles
            COMMAND sh -c "$<TARGET_FILE:cata_test-tiles> --rng-seed time"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

        if (NOT "${CMAKE_EXPORT_COMPILE_COMMANDS}")
            target_precompile_headers(cata_test-tiles PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/pch/tests-pch.hpp)
        endif ()

        set_target_properties( cata_test-tiles PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" )
    endif ()

    if (CURSES)
        add_executable(cata_test ${CATACLYSM_BN_TEST_SOURCES})
        target_link_libraries(cata_test PRIVATE cataclysm-common)
        add_test(
            NAME test
            COMMAND sh -c "$<TARGET_FILE:cata_test> --rng-seed time"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

        if (NOT "${CMAKE_EXPORT_COMPILE_COMMANDS}")
            target_precompile_headers(cata_test PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/pch/tests-pch.hpp)
        endif ()
    endif ()
endif ()
