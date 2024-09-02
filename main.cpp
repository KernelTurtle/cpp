// Copyright 2024 KernelTurtle
#include <ncurses.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <regex>   // NOLINT
#include <chrono>  // NOLINT
#include <fstream>
#include <sstream>
#include <thread>  // NOLINT
#include <unordered_set>

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

void tuiSelectAndRun(const std::string& cpp_folder);
void drawList(WINDOW* win, const std::vector<std::string>& items, int highlight);
void applyBlurEffect(WINDOW* win, const std::vector<std::string>& items);
void initColors();

std::string GetCurrentWorkingDir() {
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    return std::string(buff);
}

std::vector<std::string> listFiles(const std::string& path, const std::string& extension) {
    std::vector<std::string> files;
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string file_name = entry->d_name;
            if (file_name[0] == '.') {
                continue;  // Skip dot files
            }
            if (file_name.find(extension) != std::string::npos) {
                files.push_back(file_name);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error opening directory: " << path << std::endl;
    }

    return files;
}

std::vector<std::string> listDirectories(const std::string& path) {
    std::vector<std::string> directories;
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string dir_name = entry->d_name;
            if (dir_name[0] == '.') {
                continue;  // Skip dot directories
            }
            if (entry->d_type == DT_DIR) {
                directories.push_back(dir_name);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error opening directory: " << path << std::endl;
    }

    return directories;
}

std::string formatFileName(const std::string& file_name) {
    std::string base_name = file_name.substr(0, file_name.find_last_of("."));  // Remove ".cpp" extension
    std::replace(base_name.begin(), base_name.end(), '_', ' ');
    return base_name + " iteration";  // Append " iteration" to the base name
}

void initColors() {
    start_color();
    // Define color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLUE);     // White text on blue background
    init_pair(2, COLOR_BLACK, COLOR_CYAN);     // Black text on cyan background
    init_pair(3, COLOR_WHITE, COLOR_BLACK);    // White text on black background (menu title)
    init_pair(4, COLOR_BLACK, COLOR_WHITE);    // Black text on grey background (menu items)
    init_pair(5, COLOR_WHITE, COLOR_RED);      // White text on red background (highlight)
    init_pair(6, COLOR_BLACK, COLOR_BLUE);     // Black text on blue background (bottom menu)
    init_pair(7, COLOR_YELLOW, COLOR_WHITE);   // Yellow text (for keywords)
    init_pair(8, COLOR_MAGENTA, COLOR_WHITE);  // Magenta text (for strings)
    init_pair(9, COLOR_GREEN, COLOR_WHITE);    // Green text (for comments)
    init_pair(10, COLOR_CYAN, COLOR_WHITE);    // Cyan text (for numbers)
}

std::unordered_set<std::string> cppKeywords = {
    // Basic types
    "int", "float", "double", "char", "bool", "void", "short", "long", "signed", "unsigned", "wchar_t", "char16_t",
    "char32_t", "auto", "decltype", "nullptr",

    // Control structures
    "if", "else", "for", "while", "do", "switch", "case", "default", "break", "continue", "return",

    // Modifiers
    "const", "static", "volatile", "mutable", "inline", "explicit", "extern", "register", "thread_local",

    // Storage classes
    "auto", "static", "register", "extern", "thread_local",

    // Functions
    "void", "return", "inline", "noexcept", "constexpr",

    // Exception handling
    "try", "catch", "throw", "noexcept", "dynamic_cast", "static_cast", "reinterpret_cast", "const_cast", "typeid",

    // Object-oriented programming
    "class", "struct", "namespace", "public", "private", "protected", "virtual", "override", "final", "friend", "this",
    "operator", "template", "typename", "new", "delete", "sizeof", "alignof", "alignas",

    // Standard Library types and functions
    "std", "cout", "cin", "cerr", "clog", "endl", "string", "vector", "map", "set", "unordered_map", "unordered_set",
    "pair", "tuple", "array", "deque", "list", "queue", "stack", "priority_queue", "bitset", "shared_ptr", "unique_ptr",
    "weak_ptr", "make_shared", "make_unique", "make_pair", "make_tuple",

    // C++11/14/17/20 features
    "override", "final", "constexpr", "decltype", "nullptr", "alignof", "alignas", "static_assert", "concept",
    "requires", "co_await", "co_yield", "co_return", "import", "module", "export", "char8_t",

    // Memory management
    "new", "delete", "malloc", "free", "realloc", "calloc",

    // Input/Output
    "cin", "cout", "cerr", "clog", "printf", "scanf", "fgets", "puts", "getchar", "putchar",

    // Preprocessor
    "include", "define", "undef", "if", "ifdef", "ifndef", "else", "elif", "endif", "pragma", "error", "warning",

    // Operators
    "and", "and_eq", "bitand", "bitor", "compl", "not", "not_eq", "or", "or_eq", "xor", "xor_eq",

    // Miscellaneous
    "asm", "goto", "typedef", "typeid", "namespace", "using", "std", "sizeof", "alignof", "alignas", "static_assert",
    "constexpr",

    // Common Standard Libraries
    "iostream", "fstream", "sstream", "iomanip", "ios", "istream", "ostream", "streambuf", "locale", "algorithm",
    "functional", "iterator", "numeric", "utility", "memory", "limits", "exception", "stdexcept", "cassert", "cctype",
    "cerrno", "cfloat", "ciso646", "climits", "clocale", "cmath", "csetjmp", "csignal", "cstdarg", "cstddef", "cstdio",
    "cstdint", "cstdlib", "cstring", "ctime", "cwchar", "cwctype", "array", "bitset", "deque", "list", "map", "queue",
    "set", "stack", "vector", "unordered_map", "unordered_set", "complex", "random", "ratio", "regex", "chrono",
    "thread", "mutex", "atomic", "condition_variable", "future", "filesystem", "shared_mutex"
};



void highlightSyntax(WINDOW* win, const std::string& line, int line_num) {
    int x = 2;  // Starting x position for the text
    std::string word;
    bool in_string = false;
    bool in_comment = false; //NOLINT

    for (size_t i = 0; i < line.length(); ++i) {
        char ch = line[i];

        // Handle comments
        if (!in_string && ch == '/' && i + 1 < line.length() && line[i + 1] == '/') {
            in_comment = true;
            wattron(win, COLOR_PAIR(9));  // Highlight comments in green
            mvwprintw(win, line_num, x, "%s", line.substr(i).c_str());
            wattroff(win, COLOR_PAIR(9));
            break;  // End processing for this line
        }

        // Handle strings
        if (ch == '"') {
            in_string = !in_string;
            wattron(win, COLOR_PAIR(8));  // Highlight strings in magenta
            mvwprintw(win, line_num, x, "%c", ch);
            wattroff(win, COLOR_PAIR(8));
            x += 1;
            continue;
        }

        if (in_string) {
            mvwprintw(win, line_num, x, "%c", ch);
            x += 1;
            continue;
        }

        // Handle keywords and numbers
        if (isspace(ch) || ispunct(ch)) {
            if (!word.empty()) {
                if (cppKeywords.find(word) != cppKeywords.end()) {
                    wattron(win, COLOR_PAIR(7));  // Highlight keywords in yellow
                } else if (std::regex_match(word, std::regex("^[0-9]+$"))) {
                    wattron(win, COLOR_PAIR(10));  // Highlight numbers in cyan
                }
                mvwprintw(win, line_num, x - word.length(), "%s", word.c_str());
                wattroff(win, COLOR_PAIR(7));
                wattroff(win, COLOR_PAIR(10));
                word.clear();
            }
            mvwprintw(win, line_num, x, "%c", ch);
        } else {
            word += ch;
        }
        x += 1;
    }

    if (!word.empty()) {
        if (cppKeywords.find(word) != cppKeywords.end()) {
            wattron(win, COLOR_PAIR(7));  // Highlight keywords in yellow
        } else if (std::regex_match(word, std::regex("^[0-9]+$"))) {
            wattron(win, COLOR_PAIR(10));  // Highlight numbers in cyan
        }
        mvwprintw(win, line_num, x - word.length(), "%s", word.c_str());
        wattroff(win, COLOR_PAIR(7));
        wattroff(win, COLOR_PAIR(10));
    }
}

void applyBlurEffect() {
    attron(A_DIM);  // Apply dim attribute
    for (int i = 0; i < LINES; ++i) {
        for (int j = 0; j < COLS; ++j) {
            mvprintw(i, j, " ");  // Fill the screen with spaces to apply dim effect
        }
    }
    attroff(A_DIM);  // Turn off dim attribute
    refresh();
}

void displayScrollableContent(WINDOW* win, const std::vector<std::string>& content, int start_line, int max_lines) {
    int line_num = 1;  // Starting line for content display
    for (int i = 0; i < max_lines && (i + start_line) < static_cast<int>(content.size()); ++i) {
        std::string line = content[i + start_line];

        // Apply syntax highlighting for the entire line
        highlightSyntax(win, line, line_num);

        line_num++;
        if (line_num > max_lines) {
            break;
        }
    }

    wrefresh(win);
}

void drawBottomMenu(WINDOW* bottom_win, int highlight) {
    const std::vector<std::string> menu_items = {"<Help>", "<Back>", "<Exit>"};
    int num_items = menu_items.size();
    int max_length = 0;
    for (const auto& item : menu_items) {
        if (static_cast<int>(item.length()) > max_length) {
            max_length = item.length();
        }
    }

    int startx = (COLS - (max_length * num_items + (num_items - 1) * 4)) / 2;

    wbkgd(bottom_win, COLOR_PAIR(6));  // Set blue background for the bottom menu
    for (int i = 0; i < num_items; ++i) {
        if (i == highlight) {
            wattron(bottom_win, A_REVERSE | COLOR_PAIR(5));  // Highlight the current selection
        }
        mvwprintw(bottom_win, 1, startx + i * (max_length + 4), "%s", menu_items[i].c_str());
        wattroff(bottom_win, A_REVERSE | COLOR_PAIR(5));
    }
    wrefresh(bottom_win);
}

void drawMenu(WINDOW* menu_win, int highlight, const std::vector<std::string>& items,
              const std::string& title, bool format_items = false) {
    int x = 2, y = 2;

    wbkgd(menu_win, COLOR_PAIR(4));  // Grey background for the menu
    wattron(menu_win, COLOR_PAIR(3));
    mvwprintw(menu_win, 1, 2, "%s", title.c_str());  // Print the title at the top
    wattroff(menu_win, COLOR_PAIR(3));

    for (size_t i = 0; i < items.size(); ++i) {
        std::string display_name = format_items ? formatFileName(items[i]) : items[i];
        if (static_cast<int>(i) == highlight) {
            wattron(menu_win, A_REVERSE | COLOR_PAIR(5));  // Highlight with reverse video and red background
            mvwprintw(menu_win, y, x, "%s", display_name.c_str());
            wattroff(menu_win, A_REVERSE | COLOR_PAIR(5));
        } else {
            mvwprintw(menu_win, y, x, "%s", display_name.c_str());
        }
        y++;
    }

    // Draw borders for the menu
    wattron(menu_win, COLOR_PAIR(1));
    box(menu_win, 0, 0);
    wattroff(menu_win, COLOR_PAIR(1));

    wrefresh(menu_win);
}

std::string readFileContent(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return "Error opening file!";
    }

    std::string content, line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    file.close();
    return content;
}

void displayHelp() {
    applyBlurEffect();  // Apply blur effect before displaying the help menu

    WINDOW* help_win = newwin(10, 60, (LINES - 10) / 2, (COLS - 60) / 2);
    wbkgd(help_win, COLOR_PAIR(4));  // White background for the help menu
    box(help_win, 0, 0);
    mvwprintw(help_win, 1, 2, "Help Menu:");
    mvwprintw(help_win, 3, 2, "j/k or Arrow Keys: Navigate");
    mvwprintw(help_win, 4, 2, "Enter: Select");
    mvwprintw(help_win, 5, 2, "h: Open this help menu");
    mvwprintw(help_win, 6, 2, "b: Back to previous menu");
    mvwprintw(help_win, 7, 2, "q: Exit the program");
    mvwprintw(help_win, 9, 2, "Press any key to close this menu...");

    wrefresh(help_win);
    wgetch(help_win);
    delwin(help_win);

    // Reset the background to blue after closing the help menu
    bkgd(COLOR_PAIR(1));
    clear();
    refresh();
}

bool displayConfirmation() {
    WINDOW* confirm_win = newwin(5, 40, (LINES - 5) / 2, (COLS - 40) / 2);
    wbkgd(confirm_win, COLOR_PAIR(4));
    box(confirm_win, 0, 0);
    mvwprintw(confirm_win, 1, 2, "Are you sure you want to go back? (y/n)");

    wrefresh(confirm_win);
    int ch = wgetch(confirm_win);
    delwin(confirm_win);
    return ch == 'y' || ch == 'Y';
}

void displayCodeAndOutput(const std::string& file_content, const std::string& program_output,
                          const std::string& file_path) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    initColors();  // Initialize colors

    // Set the entire background to blue
    bkgd(COLOR_PAIR(1));
    clear();
    refresh();

    int height = LINES - 6;  // Reserve space for the bottom menu
    int half_height = height / 2;
    int width = (COLS / 2) - 2;
    int startx = 2;

    // Create a window for code, program output, and custom input
    WINDOW* code_win = newwin(height, width, 1, startx);
    WINDOW* output_win = newwin(half_height, width, 1, startx + width + 2);
    WINDOW* input_win = newwin(half_height, width, half_height + 1, startx + width + 2);
    WINDOW* bottom_win = newwin(3, COLS, LINES - 3, 0);
    WINDOW* status_win = newwin(1, COLS, 0, 0);  // Status bar window for mode indicator

    // Extract the directory and file name without the .cpp extension
    size_t last_slash = file_path.find_last_of("/\\");
    std::string directory = file_path.substr(0, last_slash);
    std::string file_name = file_path.substr(last_slash + 1);
    size_t second_last_slash = directory.find_last_of("/\\");
    std::string directory_name = directory.substr(second_last_slash + 1);
    size_t last_dot = file_name.find_last_of(".");
    std::string file_name_without_ext = file_name.substr(0, last_dot);

    // Remove underscores from directory and file names
    directory_name.erase(std::remove(directory_name.begin(), directory_name.end(), '_'), directory_name.end());
    file_name_without_ext.erase(std::remove(file_name_without_ext.begin(),
                                file_name_without_ext.end(), '_'), file_name_without_ext.end());

    // Combine directory name and file name for display
    std::string display_name = directory_name + "/" + file_name_without_ext;

    // Draw the top border lower and place the labels inside
    wbkgd(code_win, COLOR_PAIR(4));
    box(code_win, 0, 0);
    mvwprintw(code_win, 0, 2, "Executed %s", display_name.c_str());

    wbkgd(output_win, COLOR_PAIR(4));
    box(output_win, 0, 0);
    mvwprintw(output_win, 0, 2, "Program Output:");

    wbkgd(input_win, COLOR_PAIR(4));
    box(input_win, 0, 0);
    mvwprintw(input_win, 0, 2, "Custom Input:");

    // Track the highlight index for the bottom menu
    int highlight = 0;

    // Draw the bottom menu
    drawBottomMenu(bottom_win, highlight);

    // Refresh the windows to show the boxes, labels, and initial content
    wrefresh(code_win);
    wrefresh(output_win);
    wrefresh(input_win);

    // Set up the content for display
    int code_start_line = 0;
    int output_start_line = 0;
    int max_lines = height - 3;  // Reserve space for borders and labels

    std::vector<std::string> code_lines;
    std::vector<std::string> output_lines;
    std::string line;

    std::istringstream ss_code(file_content);
    while (std::getline(ss_code, line)) {
        code_lines.push_back(line);
    }

    std::istringstream ss_output(program_output);
    while (std::getline(ss_output, line)) {
        output_lines.push_back(line);
    }

    // Display the initial lines of code and output with syntax highlighting
    displayScrollableContent(code_win, code_lines, code_start_line, max_lines);
    displayScrollableContent(output_win, output_lines, output_start_line, half_height - 2);

    std::string input_text;
    bool in_insert_mode = false;
    int ch;

    // Display the initial status
    mvwprintw(status_win, 0, 2, "Mode: COMMAND");
    wrefresh(status_win);

    while ((ch = wgetch(code_win)) != 'q') {
        switch (ch) {
            case 'k':
            case KEY_UP:
                if (!in_insert_mode) {
                    if (code_start_line > 0) code_start_line--;
                    if (output_start_line > 0) output_start_line--;
                }
                break;
            case 'j':
            case KEY_DOWN:
                if (!in_insert_mode) {
                    if (code_start_line + max_lines < static_cast<int>(code_lines.size())) code_start_line++;
                    if (output_start_line + half_height - 2 < static_cast<int>(output_lines.size()))
                        output_start_line++;
                }
                break;
            case 'h':  // Help
                if (!in_insert_mode) {
                    displayHelp();

                    // Reset the background to blue and refresh all windows
                    bkgd(COLOR_PAIR(1));
                    clear();
                    refresh();
                    wrefresh(code_win);
                    wrefresh(output_win);
                    wrefresh(input_win);
                    drawBottomMenu(bottom_win, highlight);
                }
                break;
            case 'b':  // Back
                if (!in_insert_mode) {
                    if (displayConfirmation()) {  // Add confirmation before going back
                        endwin();
                        tuiSelectAndRun(GetCurrentWorkingDir());  // Restart folder selection with current directory
                        return;  // Exit the function to prevent continuing in current context
                    }
                }
                break;
            case 'i':  // Enter insert mode (Vim-like)
                in_insert_mode = true;
                mvwprintw(status_win, 0, 2, "Mode: INSERT ");
                wrefresh(status_win);
                break;
            case 27:  // Escape key to exit insert mode (Vim-like)
                in_insert_mode = false;
                mvwprintw(status_win, 0, 2, "Mode: COMMAND");
                wrefresh(status_win);
                break;
            case 'o':  // Exit and run the code
                if (!in_insert_mode) {
                    // Process the input here
                    mvwprintw(input_win, 1, 2, "Input accepted: %s", input_text.c_str());
                    input_text.clear();
                }
                break;
            default:
                if (in_insert_mode) {
                    input_text.push_back(ch);
                    mvwprintw(input_win, 1, 2, "%s", input_text.c_str());
                }
                break;
        }

        // Clear the windows and redraw the visible lines with syntax highlighting
        werase(code_win);
        werase(output_win);
        werase(input_win);
        box(code_win, 0, 0);
        mvwprintw(code_win, 0, 2, "Executed %s: ", display_name.c_str());  // Redraw label inside the box
        box(output_win, 0, 0);
        mvwprintw(output_win, 0, 2, "Program Output:");  // Redraw label inside the box
        box(input_win, 0, 0);
        mvwprintw(input_win, 0, 2, "Custom Input:");

        displayScrollableContent(code_win, code_lines, code_start_line, max_lines);
        displayScrollableContent(output_win, output_lines, output_start_line, half_height - 2);
        mvwprintw(input_win, 1, 2, "%s", input_text.c_str());

        wrefresh(code_win);
        wrefresh(output_win);
        wrefresh(input_win);
    }

    endwin();
}

std::string runCppFileWithOutput(const std::string& cpp_file_path) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        return "Error getting current working directory!";
    }

    std::string temp_dir_template = "/tmp/tuiXXXXXX";
    char temp_dir[1024];
    snprintf(temp_dir, sizeof(temp_dir), "%s", temp_dir_template.c_str());

    if (mkdtemp(temp_dir) == nullptr) {
        return "Error creating temporary directory!";
    }

    std::string binary_file = std::string(temp_dir) + "/program";  // Binary file path
    std::string output_file = std::string(temp_dir) + "/output";    // Output file path
    std::string compile_command = "clang++ \"" + cpp_file_path + "\" -o \"" + binary_file + "\"";

    std::string output;

    if (system(compile_command.c_str()) == 0) {
        auto start_time = std::chrono::high_resolution_clock::now();

        FILE* pipe = popen((binary_file + " > " + output_file).c_str(), "r");
        if (!pipe) {
            output = "Error executing program!";
        } else {
            pclose(pipe);

            std::ifstream output_stream(output_file);
            std::string program_output((std::istreambuf_iterator<char>(output_stream)),
                                        std::istreambuf_iterator<char>());
            output += program_output;

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
            output += "\nExecuted in " + std::to_string(duration) + " nanoseconds\n";
        }
    } else {
        output = "Compilation failed for " + cpp_file_path + "\n";
    }

    std::string cleanup_command = "rm -rf " + std::string(temp_dir);
    system(cleanup_command.c_str());

    return output;
}

std::string tuiSelectItem(const std::vector<std::string>& items, const std::string& title, bool format_items = false) {
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    initColors();  // Initialize colors

    // Set the entire background to blue
    bkgd(COLOR_PAIR(1));

    applyBlurEffect();  // Apply blur effect before displaying the selection menu

    int startx = (COLS - 50) / 2;
    int starty = (LINES - 20) / 2;

    WINDOW* menu_win = newwin(20, 50, starty, startx);
    keypad(menu_win, TRUE);   // Enable arrow keys for the window

    int highlight = 0;
    int choice = 0;
    int c;

    while (1) {
        drawMenu(menu_win, highlight, items, title, format_items);
        c = wgetch(menu_win);
        switch (c) {
            case 'k':
            case KEY_UP:
                if (highlight > 0) highlight--;
                break;
            case 'j':
            case KEY_DOWN:
                if (highlight < static_cast<int>(items.size() - 1)) highlight++;
                break;
            case 10:  // Enter key
                choice = highlight;
                break;
            default:
                break;
        }
        if (c == 10) break;
    }
    endwin();

    return items[choice];
}

void tuiSelectAndRun(const std::string& cpp_folder) {
    std::vector<std::string> problems = listDirectories(cpp_folder);
    if (problems.empty()) {
        std::cerr << "No directories found in " << cpp_folder << std::endl;
        return;
    }

    std::string chosen_problem = tuiSelectItem(problems, "Select a Problem", false);
    std::string chosen_problem_path = cpp_folder + "/" + chosen_problem;

    std::vector<std::string> cpp_files = listFiles(chosen_problem_path, ".cpp");
    if (cpp_files.empty()) {
        std::cerr << "No .cpp files found in " << chosen_problem_path << std::endl;
        return;
    }

    std::string chosen_file = tuiSelectItem(cpp_files, "Select a File", true);
    std::string chosen_file_path = chosen_problem_path + "/" + chosen_file;

    std::string program_output = runCppFileWithOutput(chosen_file_path);
    std::string file_content = readFileContent(chosen_file_path);

    displayCodeAndOutput(file_content, program_output, chosen_file_path);
}

void displayBigText(int x, int y) {
    const char* text[] = {
        "    dP                                    a88888b.                    ",
        "    88                                   d8'   `88                   ",
        "    88        .d8888b. .d8888b. .d8888b. 88        88d888b. 88d888b.  ",
        "    88        88ooood8 Y8ooooo. Y8ooooo. 88        88'  `88 88'  `88  ",
        "    88        88.  ...       88       88 Y8.   .88 88.  .88 88.  .88  ",
        "    88888888P `88888P' `88888P' `88888P'  Y88888P' 88Y888P' 88Y888P'  ",
        "                                                88       88           ",
        "                                                dP       dP           "
    };

    for (int i = 0; i < 7; ++i) {
        attron(A_BOLD);  // Make the text bold
        mvprintw(y + i, x, "%s", text[i]);  // Print each line of the big text
        attroff(A_BOLD);
    }
}

void renderSplashScreen() {
    initscr();
    noecho();
    curs_set(FALSE);
    start_color();

    initColors();
    bkgd(COLOR_PAIR(1));

    int donut_x = (COLS - 10) / 3;
    int donut_y = (LINES - donut_x + 32) / 1.8;

    int text_x = COLS / 2;
    int text_y = (LINES + 10) / 2;

    float A = 0, B = 0;
    int iterations = 500;

    auto frame_duration = std::chrono::milliseconds(33);  // Approx. 30 FPS
    auto start_time = std::chrono::high_resolution_clock::now();

    while (iterations--) {
        float i, j;
        int k;
        float z[1760];
        char b[1760];

        memset(b, 32, 1760);
        memset(z, 0, 7040);
        for (j = 0; j < 6.28; j += 0.07) {
            for (i = 0; i < 6.28; i += 0.02) {
                float c = sin(i);
                float d = cos(j);
                float e = sin(A);
                float f = sin(j);
                float g = cos(A);
                float h = d + 2;
                float D = 1 / (c * h * e + f * g + 5);
                float l = cos(i);
                float m = cos(B);
                float n = sin(B);
                float t = c * h * g - f * e;
                int x = donut_x + 30 * D * (l * h * m - t * n);
                int y = donut_y + 15 * D * (l * h * n + t * m);
                int o = x + 80 * y;
                int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n);
                if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    z[o] = D;
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }

        clear();
        for (k = 0; k < 1760; k++) {
            move(k / 80, k % 80);
            addch(b[k]);
        }

        displayBigText(text_x, text_y);

        refresh();

        A += 0.04;
        B += 0.02;

        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed = end_time - start_time;
        std::this_thread::sleep_for(frame_duration - elapsed);
    }
    endwin();
}

int main() {
    renderSplashScreen();

    std::string cpp_folder = GetCurrentWorkingDir();
    tuiSelectAndRun(cpp_folder);
    return 0;
}
