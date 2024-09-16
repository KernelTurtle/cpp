// Copyright 2024 Keys
#include "tui/tui.hpp"

#include <ncurses.h>

#include <algorithm>
#include <chrono>  // NOLINT [build/c++11]
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>  // NOLINT [build/c++11]

#include "colors/colors.hpp"
#include "file_handler/file_handler.hpp"
#include "syntax_highlighting/syntax_highlighting.hpp"
#include "utils/utils.hpp"

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

void TuiSelectAndRun(const std::string& cpp_folder) {
  std::vector<std::string> problems = ListDirectories(cpp_folder);
  if (problems.empty()) {
    std::cerr << "No directories found in " << cpp_folder << std::endl;
    return;
  }

  std::string chosen_problem =
      TuiSelectItem(problems, "Select a Problem", false);
  std::string chosen_problem_path = cpp_folder + "/" + chosen_problem;

  std::vector<std::string> cpp_files = ListFiles(chosen_problem_path, ".cpp");
  if (cpp_files.empty()) {
    std::cerr << "No .cpp files found in " << chosen_problem_path << std::endl;
    return;
  }

  std::string chosen_file = TuiSelectItem(cpp_files, "Select a File", true);
  std::string chosen_file_path = chosen_problem_path + "/" + chosen_file;

  std::string program_output = RunCppFileWithOutput(chosen_file_path);
  std::string file_content = ReadFileContent(chosen_file_path);

  DisplayCodeAndOutput(file_content, program_output, chosen_file_path);
}

void drawMenu(WINDOW* menu_win, int highlight,
              const std::vector<std::string>& items, const std::string& title,
              bool format_items) {
  int x = 2, y = 2;

  wbkgd(menu_win, COLOR_PAIR(4));  // Grey background for the menu
  wattron(menu_win, COLOR_PAIR(3));
  mvwprintw(menu_win, 1, 2, "%s", title.c_str());  // Print the title at the top
  wattroff(menu_win, COLOR_PAIR(3));

  for (size_t i = 0; i < items.size(); ++i) {
    std::string display_name =
        format_items ? FormatFileName(items[i]) : items[i];
    if (static_cast<int>(i) == highlight) {
      wattron(menu_win,
              A_REVERSE |
                  COLOR_PAIR(
                      5));  // Highlight with reverse video and red background
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

std::string TuiSelectItem(const std::vector<std::string>& items,
                          const std::string& title, bool format_items) {
  initscr();
  clear();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr, TRUE);

  InitColors();  // Initialize colors

  // Set the entire background to blue
  bkgd(COLOR_PAIR(1));

  applyBlurEffect();  // Apply blur effect before displaying the selection menu

  int startx = (COLS - 50) / 2;
  int starty = (LINES - 20) / 2;

  WINDOW* menu_win = newwin(20, 50, starty, startx);
  keypad(menu_win, TRUE);  // Enable arrow keys for the window

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

void DrawBottomMenu(WINDOW* bottom_win, int highlight) {
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
      wattron(bottom_win,
              A_REVERSE | COLOR_PAIR(5));  // Highlight the current selection
    }
    mvwprintw(bottom_win, 1, startx + i * (max_length + 4), "%s",
              menu_items[i].c_str());
    wattroff(bottom_win, A_REVERSE | COLOR_PAIR(5));
  }
  wrefresh(bottom_win);
}

void DisplayHelp() {
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

bool DisplayConfirmation() {
  WINDOW* confirm_win = newwin(5, 40, (LINES - 5) / 2, (COLS - 40) / 2);
  wbkgd(confirm_win, COLOR_PAIR(4));
  box(confirm_win, 0, 0);
  mvwprintw(confirm_win, 1, 2, "Are you sure you want to go back? (y/n)");

  wrefresh(confirm_win);
  int ch = wgetch(confirm_win);
  delwin(confirm_win);
  return ch == 'y' || ch == 'Y';
}

void DisplayCodeAndOutput(const std::string& file_content,
                          const std::string& program_output,
                          const std::string& file_path) {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);

  InitColors();  // Initialize colors

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
  WINDOW* input_win =
      newwin(half_height, width, half_height + 1, startx + width + 2);
  WINDOW* bottom_win = newwin(3, COLS, LINES - 3, 0);
  WINDOW* status_win =
      newwin(1, COLS, 0, 0);  // Status bar window for mode indicator

  // Extract the directory and file name without the .cpp extension
  size_t last_slash = file_path.find_last_of("/\\");
  std::string directory = file_path.substr(0, last_slash);
  std::string file_name = file_path.substr(last_slash + 1);
  size_t second_last_slash = directory.find_last_of("/\\");
  std::string directory_name = directory.substr(second_last_slash + 1);
  size_t last_dot = file_name.find_last_of(".");
  std::string file_name_without_ext = file_name.substr(0, last_dot);

  // Remove underscores from directory and file names
  directory_name.erase(
      std::remove(directory_name.begin(), directory_name.end(), '_'),
      directory_name.end());
  file_name_without_ext.erase(std::remove(file_name_without_ext.begin(),
                                          file_name_without_ext.end(), '_'),
                              file_name_without_ext.end());

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
  DrawBottomMenu(bottom_win, highlight);

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
  DisplayScrollableContent(code_win, code_lines, code_start_line, max_lines);
  DisplayScrollableContent(output_win, output_lines, output_start_line,
                           half_height - 2);

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
          if (code_start_line + max_lines < static_cast<int>(code_lines.size()))
            code_start_line++;
          if (output_start_line + half_height - 2 <
              static_cast<int>(output_lines.size()))
            output_start_line++;
        }
        break;
      case 'h':  // Help
        if (!in_insert_mode) {
          DisplayHelp();

          // Reset the background to blue and refresh all windows
          bkgd(COLOR_PAIR(1));
          clear();
          refresh();
          wrefresh(code_win);
          wrefresh(output_win);
          wrefresh(input_win);
          DrawBottomMenu(bottom_win, highlight);
        }
        break;
      case 'b':  // Back
        if (!in_insert_mode) {
          if (DisplayConfirmation()) {  // Add confirmation before going back
            endwin();
            TuiSelectAndRun(
                GetCurrentWorkingDir());  // Restart folder selection
                                          // with current directory
            return;  // Exit the function to prevent continuing in current
                     // context
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
    mvwprintw(code_win, 0, 2, "Executed %s: ",
              display_name.c_str());  // Redraw label inside the box
    box(output_win, 0, 0);
    mvwprintw(output_win, 0, 2,
              "Program Output:");  // Redraw label inside the box
    box(input_win, 0, 0);
    mvwprintw(input_win, 0, 2, "Custom Input:");

    DisplayScrollableContent(code_win, code_lines, code_start_line, max_lines);
    DisplayScrollableContent(output_win, output_lines, output_start_line,
                             half_height - 2);
    mvwprintw(input_win, 1, 2, "%s", input_text.c_str());

    wrefresh(code_win);
    wrefresh(output_win);
    wrefresh(input_win);
  }

  endwin();
}

void DisplayScrollableContent(WINDOW* win,
                              const std::vector<std::string>& content,
                              int start_line, int max_lines) {
  int line_num = 1;  // Starting line for content display
  for (int i = 0;
       i < max_lines && (i + start_line) < static_cast<int>(content.size());
       ++i) {
    std::string line = content[i + start_line];

    // Apply syntax highlighting for the entire line
    HighlightSyntax(win, line, line_num);

    line_num++;
    if (line_num > max_lines) {
      break;
    }
  }

  wrefresh(win);
}
