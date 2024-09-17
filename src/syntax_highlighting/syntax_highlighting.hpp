// Copyright 2024 Keys
#ifndef SRC_SYNTAX_HIGHLIGHTING_SYNTAX_HIGHLIGHTING_HPP_
#define SRC_SYNTAX_HIGHLIGHTING_SYNTAX_HIGHLIGHTING_HPP_

#ifdef _WIN32
    #include <curses.h>
#else
    #include <ncurses.h>
#endif

#include <string>
#include <unordered_set>

extern std::unordered_set<std::string> cppKeywords;

void HighlightSyntax(WINDOW* win, const std::string& line, int line_num);

#endif  // SRC_SYNTAX_HIGHLIGHTING_SYNTAX_HIGHLIGHTING_HPP_
