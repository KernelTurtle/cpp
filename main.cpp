#include <ncurses.h>
#include <dirent.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <regex>
#include <chrono>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

std::string GetCurrentWorkingDir() {
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

std::vector<std::string> listFiles(const std::string& path, const std::string& extension) {
    std::vector<std::string> files;
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string file_name = entry->d_name;
            // Skip dot files
            if (file_name[0] == '.') {
                continue;
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
            // Skip dot directories
            if (dir_name[0] == '.') {
                continue;
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
    // Remove ".cpp" extension
    std::string base_name = file_name.substr(0, file_name.find_last_of("."));

    // Append " iteration" to the base name
    return base_name + " iteration";
}

void initColors() {
    start_color();
    // Define color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // White text on blue background
    init_pair(2, COLOR_BLACK, COLOR_CYAN);   // Black text on cyan background
    init_pair(3, COLOR_WHITE, COLOR_BLACK);  // White text on black background (menu title)
    init_pair(4, COLOR_BLACK, COLOR_WHITE);  // Black text on grey background (menu items)
    init_pair(5, COLOR_WHITE, COLOR_RED);    // White text on red background (highlight)
    init_pair(6, COLOR_BLACK, COLOR_BLUE);   // Black text on blue background (bottom menu)
}

void drawMenu(WINDOW* menu_win, int highlight, const std::vector<std::string>& items, const std::string& title, bool format_items = false) {
    int x = 2, y = 2;
    
    wbkgd(menu_win, COLOR_PAIR(4));  // Grey background for the menu
    wattron(menu_win, COLOR_PAIR(3));
    mvwprintw(menu_win, 1, 2, title.c_str());  // Print the title at the top
    wattroff(menu_win, COLOR_PAIR(3));
    
    for (size_t i = 0; i < items.size(); ++i) {
        std::string display_name = format_items ? formatFileName(items[i]) : items[i];
        if (static_cast<int>(i) == highlight) {
            wattron(menu_win, A_REVERSE | COLOR_PAIR(5));  // Highlight with reverse video and red background
            mvwprintw(menu_win, y, x, display_name.c_str());
            wattroff(menu_win, A_REVERSE | COLOR_PAIR(5));
        } else {
            mvwprintw(menu_win, y, x, display_name.c_str());
        }
        y++;
    }

    // Draw borders for the menu
    wattron(menu_win, COLOR_PAIR(1));
    box(menu_win, 0, 0);
    wattroff(menu_win, COLOR_PAIR(1));

    wrefresh(menu_win);
}

std::string tuiSelectItem(const std::vector<std::string>& items, const std::string& title, bool format_items = false) {
    initscr();
    clear();
    noecho();
    cbreak();   // Disable line buffering, pass on everything
    curs_set(0);
    keypad(stdscr, TRUE);   // Enable arrow keys

    initColors();  // Initialize colors

    // Set the entire background to blue
    bkgd(COLOR_PAIR(1));

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

std::string runCppFileWithOutput(const std::string& cpp_file_path) {
    std::string output_file = cpp_file_path.substr(0, cpp_file_path.find_last_of(".")) + "_output";
    std::string compile_command = "clang++ " + cpp_file_path + " -o " + output_file;
    std::string output;
    if (system(compile_command.c_str()) == 0) {
        // Measure the runtime
        auto start_time = std::chrono::high_resolution_clock::now();
        
        FILE* pipe = popen(output_file.c_str(), "r");
        if (!pipe) {
            output = "Error executing program!";
        } else {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                output += buffer;
            }
            pclose(pipe);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();

        output += "\nExecution time: " + std::to_string(duration) + " nanoseconds\n";
    } else {
        output = "Compilation failed for " + cpp_file_path + "\n";
    }
    return output;
}

void displayScrollableContent(WINDOW* win, const std::vector<std::string>& content, int start_line, int max_lines) {
    int line_num = 2; // Starting line for content display
    for (int i = 0; i < max_lines && (i + start_line) < (int)content.size(); ++i) {
        mvwprintw(win, line_num++, 2, content[i + start_line].c_str());
    }
}

void drawBottomMenu(WINDOW* bottom_win) {
    wattron(bottom_win, COLOR_PAIR(6));
    mvwprintw(bottom_win, 1, 2, "<Help> <Back> <Exit>");
    wattroff(bottom_win, COLOR_PAIR(6));
    wrefresh(bottom_win);
}

void displayHelp() {
    WINDOW* help_win = newwin(10, 60, (LINES - 10) / 2, (COLS - 60) / 2);
    wbkgd(help_win, COLOR_PAIR(4));
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

void displayCodeAndOutput(const std::string& file_content, const std::string& program_output) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    int height = LINES - 6;  // Reserve space for the bottom menu
    int width = (COLS / 2) - 2;
    int startx = 2;
    int starty = 2;

    // Create two windows side by side
    WINDOW* code_win = newwin(height, width, starty, startx);
    WINDOW* output_win = newwin(height, width, starty, startx + width + 2);
    WINDOW* bottom_win = newwin(3, COLS, LINES - 3, 0);

    wbkgd(code_win, COLOR_PAIR(4));
    box(code_win, 0, 0);
    mvwprintw(code_win, 1, 2, "Executed Code:");

    wbkgd(output_win, COLOR_PAIR(4));
    box(output_win, 0, 0);
    mvwprintw(output_win, 1, 2, "Program Output:");

    wbkgd(bottom_win, COLOR_PAIR(6));
    drawBottomMenu(bottom_win);

    int code_start_line = 0;
    int output_start_line = 0;
    int max_lines = height - 3;  // Reserve space for borders and titles

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

    // Display the initial lines of code and output
    displayScrollableContent(code_win, code_lines, code_start_line, max_lines);
    displayScrollableContent(output_win, output_lines, output_start_line, max_lines);

    wrefresh(code_win);
    wrefresh(output_win);

    int ch;
    while ((ch = wgetch(code_win)) != 'q') {
        switch (ch) {
            case 'k':
            case KEY_UP:
                if (code_start_line > 0) code_start_line--;
                if (output_start_line > 0) output_start_line--;
                break;
            case 'j':
            case KEY_DOWN:
                if (code_start_line + max_lines < (int)code_lines.size()) code_start_line++;
                if (output_start_line + max_lines < (int)output_lines.size()) output_start_line++;
                break;
            case 'h':  // Help
                displayHelp();
                break;
            case 'b':  // Back
                if (displayConfirmation()) {
                    return;  // Exit the function to go back to the previous menu
                }
                break;
            case 'q':  // Exit
                return;  // Exit the loop and program
            default:
                break;
        }

        // Clear the windows and redraw the visible lines
        werase(code_win);
        werase(output_win);
        box(code_win, 0, 0);
        mvwprintw(code_win, 1, 2, "Executed Code:");
        box(output_win, 0, 0);
        mvwprintw(output_win, 1, 2, "Program Output:");

        displayScrollableContent(code_win, code_lines, code_start_line, max_lines);
        displayScrollableContent(output_win, output_lines, output_start_line, max_lines);

        wrefresh(code_win);
        wrefresh(output_win);
    }

    endwin();
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

    // Read the content of the executed file
    std::string file_content = readFileContent(chosen_file_path);

    // Display the code and output side by side with scrolling functionality
    displayCodeAndOutput(file_content, program_output);
}

int main() {
    std::string cpp_folder = GetCurrentWorkingDir();  // Start in the current working directory
    tuiSelectAndRun(cpp_folder);
    return 0;
}
