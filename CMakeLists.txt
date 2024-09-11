cmake_minimum_required(VERSION 3.10)
project(LeetCode)

set(CMAKE_CXX_STANDARD 17)

# Include the directories for header files
include_directories(src)

# Add the executable and include the source files
add_executable(LeetCode
    src/main.cpp
    src/tui/tui.cpp
    src/file_handler/file_handler.cpp
    src/syntax_highlighting/syntax_highlighting.cpp
    src/splash_screen/splash_screen.cpp
    src/utils/utils.cpp
    src/colors/colors.cpp
)

# Link the ncurses library
target_link_libraries(LeetCode ncurses)
