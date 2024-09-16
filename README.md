# LeetViewer

## Overview

LeetViewer is a terminal-based application built using `ncurses` that allows users to navigate through directories, select and run C++ files, and view their output directly in the terminal. The application features a Vim-like command mode, syntax highlighting for C++ code, and a splash screen with animated ASCII art.

## Features

- **Splash Screen**: An engaging splash screen with animated ASCII art.
- **File Navigation**: Browse directories and select `.cpp` files to run.
- **Vim-like Mode**: Toggle between command and insert modes.
- **Syntax Highlighting**: C++ code is displayed with syntax highlighting.
- **Tab Navigation**: Use the Tab key to cycle through menu items.
- **Cross-platform Support**: Runs on both Unix-like systems and Windows.

## Prerequisites

- **CMake**: Make sure CMake is installed on your system to build the project.
- **ncurses**: The project uses `ncurses` for terminal manipulation. Ensure it is installed on your system.

### Installing Dependencies

#### On Ubuntu/Debian:

```bash
sudo apt-get update
sudo apt-get install libncurses5-dev libncursesw5-dev cmake g++
```

#### On Fedora:

```bash
sudo dnf install ncurses-devel cmake g++
```

#### On Windows:

For Windows, you may need to use a package manager like `vcpkg` or `MSYS2` to install `ncurses` and CMake. Alternatively, using WSL (Windows Subsystem for Linux) might be easier for running this project.

## Building the Project

1. Clone the repository:
   ```bash
   git clone https://github.com/keys-i/LeetCode.git
   cd LeetCode
   ```

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:
   ```bash
   cmake ..
   ```
4. Build the project:
   ```bash
   make
   ```

5. Run the executable:
   ```bash
   ./TUIProject
   ```

## Filfe Structure

The project is organized into the following directories and files:

```plaintext
- src/
  - main.cpp              # Entry point of the application
  - tui.cpp               # TUI logic for menu and code display
  - tui.hpp               # Header file for TUI functions
  - file_handler.cpp      # Functions for file and directory operations
  - file_handler.hpp      # Header file for file handler functions
  - syntax_highlighting.cpp  # Syntax highlighting logic for C++
  - syntax_highlighting.hpp  # Header file for syntax highlighting
  - splash_screen.cpp     # Splash screen animation
  - splash_screen.hpp     # Header file for splash screen
  - utils.cpp             # Utility functions
  - utils.hpp             # Header file for utility functions
  - colors.cpp            # Color initialization
  - colors.hpp            # Header file for color functions
- CMakeLists.txt          # CMake configuration file
- README.md               # This README file
```

## Usage

1. **Start the Application**: After building, run the `TUIProject` executable.

2. **Splash Screen**: Enjoy the splash screen animation that appears on startup.

3. **Select a Problem Directory**: Navigate through directories to select the one containing your C++ files.

4. **Choose a C++ File**: Pick a `.cpp` file from the list.

5. **View and Execute**: The code is displayed with syntax highlighting. The output of the file is also shown in the terminal.

6. **Vim-like Navigation**:
   - Use `j` and `k` or arrow keys to navigate.
   - Press `i` to enter insert mode (for custom input).
   - Press `Esc` to return to command mode.
   - Press `q` to quit the application.

## Key Bindings

- **Navigation**: Use `j/k` or arrow keys to move through the list.
- **Select Item**: Press `Enter` to select.
- **Insert Mode**: Press `i` to enter insert mode.
- **Command Mode**: Press `Esc` to return to command mode.
- **Help**: Press `h` to open the help menu.
- **Back**: Press `b` to go back to the previous menu.
- **Quit**: Press `q` to exit the application.

## Contributing

Feel free to fork this project, submit pull requests, or report issues. Contributions are welcome!

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Author

Developed by [![GitHub](https://img.shields.io/badge/GitHub-Profile-blue?logo=github)](https://github.com/keys-i).
