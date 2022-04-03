MESSAGE(STATUS "Process LANGUAGES variable --\n")

IF (LANGUAGES STREQUAL none)
    MESSAGE(STATUS "Not compiling any languages.\n")
    return()
ENDIF()

FIND_PACKAGE(Gettext)

IF (NOT GETTEXT_FOUND)
    MESSAGE(FATAL_ERROR
            "Gettext not found. Install gettext package or disable
             compilation of language files with: -DLANGUAGES=none \nSee INSTALL file for details and more info\n"
    )
ENDIF (NOT GETTEXT_FOUND)

IF (LANGUAGES STREQUAL all)
    set(LANGUAGES "")
    file (GLOB PO_FILES
      ${CMAKE_SOURCE_DIR}/lang/po/*.po
    )
    foreach (PO_FILE ${PO_FILES})
      get_filename_component(LANG ${PO_FILE} NAME_WE)
      list(APPEND LANGUAGES ${LANG})
    endforeach ()
    list(REMOVE_DUPLICATES LANGUAGES)
    # End of special case
ENDIF()

foreach (LANG ${LANGUAGES})
    MESSAGE(STATUS "Add translation for ${LANG}: ${LANG}.po")
endforeach ()
MESSAGE("\n")

add_custom_target (
    translations_prepare
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Auto-Compile translation on release builds only
IF (RELEASE)
    add_custom_target (
      translations_compile
      ALL
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      DEPENDS translations_prepare
    )
ELSE (RELEASE)
    add_custom_target (
      translations_compile
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      DEPENDS translations_prepare
    )
ENDIF (RELEASE)

foreach (LANG ${LANGUAGES})
    add_custom_command (
      TARGET translations_prepare
      PRE_BUILD
      COMMAND ${CMAKE_COMMAND}
              -E make_directory ${CMAKE_SOURCE_DIR}/lang/mo/${LANG}/LC_MESSAGES
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    add_custom_command (
      TARGET translations_compile
      PRE_BUILD
      COMMAND 
      ${GETTEXT_MSGFMT_EXECUTABLE} -f ${CMAKE_SOURCE_DIR}/lang/po/${LANG}.po
             -o ${CMAKE_SOURCE_DIR}/lang/mo/${LANG}/LC_MESSAGES/cataclysm-bn.mo
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    IF(RELEASE)
      IF (${CMAKE_SYSTEM_NAME} MATCHES Windows)
        #install(DIRECTORY ${CMAKE_SOURCE_DIR}/lang/mo/${LANG} DESTINATION ${DATA_PREFIX})
      ELSE (${CMAKE_SYSTEM_NAME} MATCHES Windows)
        install(DIRECTORY ${CMAKE_SOURCE_DIR}/lang/mo/${LANG} DESTINATION ${LOCALE_DIR})
      ENDIF (${CMAKE_SYSTEM_NAME} MATCHES Windows)
    ENDIF(RELEASE)
endforeach()

