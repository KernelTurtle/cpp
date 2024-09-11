// Copyright 2024 Keys
#include "splash_screen/splash_screen.hpp"

#include <ncurses.h>

#include <chrono>  // NOLINT [build/c++11]
#include <cmath>
#include <cstring>
#include <thread>  // NOLINT [build/c++11]
#include <utility>

#include "../colors/colors.hpp"

void DisplayBigText(int x, int y) {
  const char* text[] = {
      "    dP                                    a88888b.                    ",
      "    88                                   d8'   `88                   ",
      "    88        .d8888b. .d8888b. .d8888b. 88        88d888b. 88d888b.  ",
      "    88        88ooood8 Y8ooooo. Y8ooooo. 88        88'  `88 88'  `88  ",
      "    88        88.  ...       88       88 Y8.   .88 88.  .88 88.  .88  ",
      "    88888888P `88888P' `88888P' `88888P'  Y88888P' 88Y888P' 88Y888P'  ",
      "                                                88       88           ",
      "                                                dP       dP           "};

  for (int i = 0; i < 7; ++i) {
    attron(A_BOLD);                     // Make the text bold
    mvprintw(y + i, x, "%s", text[i]);  // Print each line of the big text
    attroff(A_BOLD);
  }
}

void RenderSplashScreen() {
  initscr();
  noecho();
  curs_set(FALSE);
  start_color();

  InitColors();
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

    DisplayBigText(text_x, text_y);

    refresh();

    A += 0.04;
    B += 0.02;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed = end_time - start_time;
    std::this_thread::sleep_for(frame_duration - elapsed);
  }
  endwin();
}
