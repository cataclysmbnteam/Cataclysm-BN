MESSAGE(STATUS "Process LANGUAGES variable --\n")

FIND_PACKAGE(Gettext)

IF (NOT GETTEXT_FOUND)
    MESSAGE(FATAL_ERROR
            "Gettext not found. Install gettext package or disable
             localization with: -DLOCALIZE=OFF \nSee INSTALL file for details and more info\n"
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
    # Special case for English. See explanation below.
    list(APPEND LANGUAGES en)
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
    IF (${LANG} STREQUAL en)
      # English is special: we do not actually need translation for English, but
      # due to a libintl bug (https://savannah.gnu.org/bugs/index.php?58006),
      # gettext would be extremely slow on MinGW targets if we do not compile a
      # .mo file.
      add_custom_command (
        TARGET translations_prepare
        PRE_BUILD
        COMMAND msgen ${CMAKE_SOURCE_DIR}/lang/po/cataclysm-bn.pot --output-file=${CMAKE_SOURCE_DIR}/lang/po/en.po
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      )
    ENDIF (${LANG} STREQUAL en)
    add_custom_command (
      TARGET translations_compile
      PRE_BUILD
      COMMAND 
      ${GETTEXT_MSGFMT_EXECUTABLE} -f ${CMAKE_SOURCE_DIR}/lang/po/${LANG}.po
             -o ${CMAKE_SOURCE_DIR}/lang/mo/${LANG}/LC_MESSAGES/cataclysm-dda.mo
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

