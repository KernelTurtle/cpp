// Copyright 2024 Keys
#include "colors/colors.hpp"

#include <ncurses.h>

void InitColors() {
  start_color();
  // Define color pairs
  init_pair(1, COLOR_WHITE, COLOR_BLUE);  // White text on blue background
  init_pair(2, COLOR_BLACK, COLOR_CYAN);  // Black text on cyan background
  init_pair(3, COLOR_WHITE,
            COLOR_BLACK);  // White text on black background (menu title)
  init_pair(4, COLOR_BLACK,
            COLOR_WHITE);  // Black text on grey background (menu items)
  init_pair(5, COLOR_WHITE,
            COLOR_RED);  // White text on red background (highlight)
  init_pair(6, COLOR_BLACK,
            COLOR_BLUE);  // Black text on blue background (bottom menu)
  init_pair(7, COLOR_YELLOW, COLOR_WHITE);   // Yellow text (for keywords)
  init_pair(8, COLOR_MAGENTA, COLOR_WHITE);  // Magenta text (for strings)
  init_pair(9, COLOR_GREEN, COLOR_WHITE);    // Green text (for comments)
  init_pair(10, COLOR_CYAN, COLOR_WHITE);    // Cyan text (for numbers)
}
