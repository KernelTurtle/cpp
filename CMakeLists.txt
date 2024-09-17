cmake_minimum_required(VERSION 3.10)
project(LeetCode)

set(CMAKE_CXX_STANDARD 17)

# Include the directories for header files
include_directories(src)

# Add the executable and include the source files
add_executable(LeetViewer
    src/main.cpp
    src/tui/tui.cpp
    src/file_handler/file_handler.cpp
    src/syntax_highlighting/syntax_highlighting.cpp
    src/splash_screen/splash_screen.cpp
    src/utils/utils.cpp
    src/colors/colors.cpp
)

# Check if we are on Windows and use PDCurses instead of ncurses
if (WIN32)
    # For Windows, use PDCurses
    set(PDCURSES_DIR "${CMAKE_SOURCE_DIR}/PDCurses/wincon")
    include_directories(${CMAKE_SOURCE_DIR}/PDCurses)

    # Verify if the pdcurses.a file exists
    if (EXISTS "${PDCURSES_DIR}/pdcurses.a")
        add_library(pdcurses STATIC IMPORTED)
        set_target_properties(pdcurses PROPERTIES
            IMPORTED_LOCATION "${PDCURSES_DIR}/pdcurses.a"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/PDCurses"
        )
        target_link_libraries(LeetViewer pdcurses)
    else()
        message(FATAL_ERROR "PDCurses library not found at ${PDCURSES_DIR}/pdcurses.a. Please build PDCurses.")
    endif()

else()
    # For Unix-based systems, use ncurses
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIR})
    target_link_libraries(LeetViewer ${CURSES_LIBRARIES})
endif()
