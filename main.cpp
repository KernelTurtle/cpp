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
#include <regex>   //NOLINT
#include <chrono>  //NOLINT
#include <fstream>
#include <sstream>
#include <thread>  //NOLINT
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
    std::replace(base_name.begin(), base_name.end(), '_', ' ');
    // Append " iteration" to the base name
    return base_name;
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
    "int", "float", "double", "char", "bool", "void", "if", "else", "for", "while", "do",
    "switch", "case", "default", "break", "continue", "return", "true", "false", "class",
    "struct", "namespace", "public", "private", "protected", "virtual", "override", "const",
    "static", "template", "typename", "include", "iostream", "using", "std", "new", "delete",
    "try", "catch", "throw"
};
void highlightSyntax(WINDOW* win, const std::string& line, int line_num) {
    int x = 2;  // Starting x position for the text
    std::string word;
    bool in_string = false;
    bool in_comment = false;

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
            if (in_string) {
                in_string = false;
                wattron(win, COLOR_PAIR(8));  // Highlight strings in magenta
                mvwprintw(win, line_num, x, "%c", ch);
                wattroff(win, COLOR_PAIR(8));
            } else {
                in_string = true;
                wattron(win, COLOR_PAIR(8));  // Highlight strings in magenta
                mvwprintw(win, line_num, x, "%c", ch);
                wattroff(win, COLOR_PAIR(8));
            }
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
                // Determine color for the word
                if (cppKeywords.find(word) != cppKeywords.end()) {
                    wattron(win, COLOR_PAIR(7));  // Highlight keywords in yellow
                } else if (std::regex_match(word, std::regex("^[0-9]+$"))) {
                    wattron(win, COLOR_PAIR(10));  // Highlight numbers in cyan
                } else {
                    wattron(win, COLOR_PAIR(4));  // Default color
                }
                mvwprintw(win, line_num, x - word.length(), "%s", word.c_str());
                wattroff(win, COLOR_PAIR(7));
                wattroff(win, COLOR_PAIR(10));
                wattroff(win, COLOR_PAIR(4));
                word.clear();
            }
            // Print symbols directly
            if (!in_comment) {
                mvwprintw(win, line_num, x, "%c", ch);
            }
        } else {
            word += ch;
        }
        x += 1;
    }

    // Handle any remaining word after the loop
    if (!word.empty()) {
        if (cppKeywords.find(word) != cppKeywords.end()) {
            wattron(win, COLOR_PAIR(7));  // Highlight keywords in yellow
        } else if (std::regex_match(word, std::regex("^[0-9]+$"))) {
            wattron(win, COLOR_PAIR(10));  // Highlight numbers in cyan
        } else {
            wattron(win, COLOR_PAIR(4));  // Default color
        }
        mvwprintw(win, line_num, x - word.length(), "%s", word.c_str());
        wattroff(win, COLOR_PAIR(7));
        wattroff(win, COLOR_PAIR(10));
        wattroff(win, COLOR_PAIR(4));
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
        highlightSyntax(win, content[i + start_line], line_num++);
    }
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


void drawMenu(
    WINDOW* menu_win,
    int highlight,
    const std::vector<std::string>& items,
    const std::string& title,
    bool format_items = false
) {
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

void displayCodeAndOutput(const std::string& file_content, const std::string& program_output) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    initColors();  // Initialize colors

    // Set the entire background to blue
    bkgd(COLOR_PAIR(1));
    clear();
    refresh();

    int label_space = 2;  // Space for the label inside the box
    int height = LINES - label_space - 2;  // Reserve space for the bottom menu
    int width = (COLS / 2) - 2;
    int startx = 2;
    int starty = label_space - 1;  // Start lower to accommodate the label inside the box

    // Create two windows side by side for code and output
    WINDOW* code_win = newwin(height, width, starty, startx);
    WINDOW* output_win = newwin(height, width, starty, startx + width + 2);
    WINDOW* bottom_win = newwin(3, COLS, LINES - 3, 0);

    // Draw the top border lower and place the labels inside
    wbkgd(code_win, COLOR_PAIR(4));
    box(code_win, 0, 0);
    mvwprintw(code_win, 0, 2, "Executed Code:");

    wbkgd(output_win, COLOR_PAIR(4));
    box(output_win, 0, 0);
    mvwprintw(output_win, 0, 2, "Program Output:");

    // Track the highlight index for the bottom menu
    int highlight = 0;

    // Draw the bottom menu
    drawBottomMenu(bottom_win, highlight);

    // Refresh the windows to show the boxes, labels, and initial content
    wrefresh(code_win);
    wrefresh(output_win);

    // Set up the content for display
    int code_start_line = 0;
    int output_start_line = 0;
    int max_lines = height - label_space - 1;  // Reserve space for borders and labels

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

    // Refresh the windows again to ensure the content is displayed
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
                if (code_start_line + max_lines < static_cast<int>(code_lines.size())) code_start_line++;
                if (output_start_line + max_lines < static_cast<int>(output_lines.size())) output_start_line++;
                break;
            case 'h':  // Help
                displayHelp();

                // Reset the background to blue and refresh all windows
                bkgd(COLOR_PAIR(1));
                clear();
                refresh();
                wrefresh(code_win);
                wrefresh(output_win);
                drawBottomMenu(bottom_win, highlight);
                break;
            case 'b':  // Back
                // Clean up current windows and call tuiSelectAndRun to return to folder selection
                endwin();
                tuiSelectAndRun(GetCurrentWorkingDir());  // Restart folder selection with current directory
                return;  // Exit the function to prevent continuing in current context
            case 'q':  // Exit
                endwin();
                return;  // Exit the loop and program
            case '\t':  // Tab key to move between menu options
                highlight = (highlight + 1) % 3;
                drawBottomMenu(bottom_win, highlight);
                break;
            case '\n':  // Enter key to select the highlighted option
                if (highlight == 0) {  // Help
                    displayHelp();
                } else if (highlight == 1) {  // Back
                    endwin();
                    tuiSelectAndRun(GetCurrentWorkingDir());
                    return;
                } else if (highlight == 2) {  // Exit
                    endwin();
                    return;
                }
                // After any action, reset the background to blue and refresh the windows
                bkgd(COLOR_PAIR(1));
                clear();
                refresh();
                wrefresh(code_win);
                wrefresh(output_win);
                drawBottomMenu(bottom_win, highlight);
                break;
            default:
                break;
        }

        // Clear the windows and redraw the visible lines
        werase(code_win);
        werase(output_win);
        box(code_win, 0, 0);
        mvwprintw(code_win, 0, 2, "Executed Code:");  // Redraw label inside the box
        box(output_win, 0, 0);
        mvwprintw(output_win, 0, 2, "Program Output:");  // Redraw label inside the box

        displayScrollableContent(code_win, code_lines, code_start_line, max_lines);
        displayScrollableContent(output_win, output_lines, output_start_line, max_lines);

        wrefresh(code_win);
        wrefresh(output_win);
    }

    endwin();
}
std::string runCppFileWithOutput(const std::string& cpp_file_path) {
    // Get the current working directory
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        return "Error getting current working directory!";
    }
    std::string cwd_str = std::string(cwd);

    // Create a temporary directory name
    char temp_dir_template[] = "/tmp/tuiXXXXXX";
    char* temp_dir = mkdtemp(temp_dir_template);
    if (temp_dir == nullptr) {
        return "Error creating temporary directory!";
    }

    std::string temp_dir_str = temp_dir;
    std::string binary_file = temp_dir_str + "/program";  // Binary file path
    std::string output_file = temp_dir_str + "/output";    // Output file path
    std::string compile_command = "clang++ " + cpp_file_path + " -o " + binary_file;
    std::string output;

    if (system(compile_command.c_str()) == 0) {
        // Measure the runtime
        auto start_time = std::chrono::high_resolution_clock::now();

        FILE* pipe = popen((binary_file + " > " + output_file).c_str(), "r");
        if (!pipe) {
            output = "Error executing program!";
        } else {
            pclose(pipe);

            // Append program output
            std::ifstream output_stream(output_file);
            std::string program_output((std::istreambuf_iterator<char>(output_stream)),
                                        std::istreambuf_iterator<char>());
            output += program_output;

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
            output += "\nExecution time: " + std::to_string(duration) + " nanoseconds\n";
        }
    } else {
        output = "Compilation failed for " + cpp_file_path + "\n";
    }

    // Clean up temporary directory
    std::string cleanup_command = "rm -rf " + temp_dir_str;
    system(cleanup_command.c_str());

    return output;
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

    // Select a problem directory
    std::string chosen_problem = tuiSelectItem(problems, "Select a Problem", false);
    std::string chosen_problem_path = cpp_folder + "/" + chosen_problem;

    std::vector<std::string> cpp_files = listFiles(chosen_problem_path, ".cpp");
    if (cpp_files.empty()) {
        std::cerr << "No .cpp files found in " << chosen_problem_path << std::endl;
        return;
    }

    // Select a .cpp file
    std::string chosen_file = tuiSelectItem(cpp_files, "Select a File", true);
    std::string chosen_file_path = chosen_problem_path + "/" + chosen_file;

    // Run the selected file and get the output
    std::string program_output = runCppFileWithOutput(chosen_file_path);

    // Read the content of the executed file
    std::string file_content = readFileContent(chosen_file_path);

    // Display the code and output side by side with scrolling functionality
    displayCodeAndOutput(file_content, program_output);
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
    initscr();              // Initialize the window
    noecho();               // Don't echo any keypresses
    curs_set(FALSE);        // Don't display a cursor
    start_color();          // Enable color functionality

    // Fill background with color
    initColors();  // Initialize colors
    bkgd(COLOR_PAIR(1));  // Set the background color to blue

    // Spinning donut animation on the left side
    int donut_x = (COLS - 10)/ 3;  // Left side of the screen
    int donut_y = (LINES - donut_x + 32)/ 1.8;  // Center vertically with some offset

    int text_x = COLS / 2;  // Right half of the screen
    int text_y = (LINES + 10) / 2;  // Center vertically

    float A = 0, B = 0;
    int iterations = 500;  // Number of frames to display

    while (iterations--) {
        float i, j;
        int k;
        float z[1760];
        char b[1760];

        memset(b, 32, 1760);   // Fill buffer with spaces
        memset(z, 0, 7040);    // Fill depth buffer with zeroes
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
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];  // Choose character based on brightness
                }
            }
        }

        clear();  // Clear the screen before drawing
        for (k = 0; k < 1760; k++) {
            move(k / 80, k % 80);  // Move to the position where the character will be printed
            addch(b[k]);           // Print the character
        }

        // Display the big text "LessCpp" on the right side
        displayBigText(text_x, text_y);  // Display the big text

        refresh();  // Refresh the screen to show the updated content

        A += 0.04;  // Increment angle A to spin the donut
        B += 0.02;  // Increment angle B to spin the donut
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed = end_time - start_time;
        std::this_thread::sleep_for(frame_duration - elapsed);
    }
    endwin();  // End ncurses mode
}



int main() {
    renderSplashScreen();

    std::string cpp_folder = GetCurrentWorkingDir();  // Start in the current working directory
    tuiSelectAndRun(cpp_folder);
    return 0;
}
