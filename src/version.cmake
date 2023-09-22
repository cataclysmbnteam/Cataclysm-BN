if (GIT_EXECUTABLE)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always --dirty --match "[0-9A-Z]*.[0-9A-Z]*"
        OUTPUT_VARIABLE VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
else ()
    message(WARNING "Git binary not found. \
            Build version will be set to NULL. \
            Install Git package or use -DGIT_BINARY to set path to git binary.")
    set(VERSION "NULL")
endif ()

configure_file(${SRC} ${DST} @ONLY)
