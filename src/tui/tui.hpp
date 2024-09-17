// Copyright 2024 Keys
#ifndef SRC_TUI_TUI_HPP_
#define SRC_TUI_TUI_HPP_

#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif

#include <string>
#include <vector>

void TuiSelectAndRun(const std::string& cpp_folder);
void DrawMenu(WINDOW* menu_win, int highlight,
              const std::vector<std::string>& items, const std::string& title,
              bool format_items = false);
std::string TuiSelectItem(const std::vector<std::string>& items,
                          const std::string& title, bool format_items = false);
void DisplayCodeAndOutput(const std::string& file_content,
                          const std::string& program_output,
                          const std::string& file_path);
void DisplayScrollableContent(WINDOW* win,
                              const std::vector<std::string>& content,
                              int start_line, int max_lines);
void DrawBottomMenu(WINDOW* bottom_win, int highlight);
void DisplayHelp();
bool DisplayConfirmation();
void InitColors();
void ApplyBlurEffect();

#endif  // SRC_TUI_TUI_HPP_
