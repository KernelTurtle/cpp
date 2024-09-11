// Copyright 2024 Keys
#include "syntax_highlighting/syntax_highlighting.hpp"

#include <ncurses.h>

#include <regex>  // NOLINT [build/c++11]
#include <string>

std::unordered_set<std::string> cppKeywords = {"int",
                                               "float",
                                               "double",
                                               "char",
                                               "bool",
                                               "void",
                                               "short",
                                               "long",
                                               "signed",
                                               "unsigned",
                                               "wchar_t",
                                               "char16_t",
                                               "char32_t",
                                               "auto",
                                               "decltype",
                                               "nullptr",
                                               "if",
                                               "else",
                                               "for",
                                               "while",
                                               "do",
                                               "switch",
                                               "case",
                                               "default",
                                               "break",
                                               "continue",
                                               "return",
                                               "const",
                                               "static",
                                               "volatile",
                                               "mutable",
                                               "inline",
                                               "explicit",
                                               "extern",
                                               "register",
                                               "thread_local",
                                               "void",
                                               "return",
                                               "inline",
                                               "noexcept",
                                               "constexpr",
                                               "try",
                                               "catch",
                                               "throw",
                                               "noexcept",
                                               "dynamic_cast",
                                               "static_cast",
                                               "reinterpret_cast",
                                               "const_cast",
                                               "typeid",
                                               "class",
                                               "struct",
                                               "namespace",
                                               "public",
                                               "private",
                                               "protected",
                                               "virtual",
                                               "override",
                                               "final",
                                               "friend",
                                               "this",
                                               "operator",
                                               "template",
                                               "typename",
                                               "new",
                                               "delete",
                                               "sizeof",
                                               "alignof",
                                               "alignas",
                                               "std",
                                               "cout",
                                               "cin",
                                               "cerr",
                                               "clog",
                                               "endl",
                                               "string",
                                               "vector",
                                               "map",
                                               "set",
                                               "unordered_map",
                                               "unordered_set",
                                               "pair",
                                               "tuple",
                                               "array",
                                               "deque",
                                               "list",
                                               "queue",
                                               "stack",
                                               "priority_queue",
                                               "bitset",
                                               "shared_ptr",
                                               "unique_ptr",
                                               "weak_ptr",
                                               "make_shared",
                                               "make_unique",
                                               "make_pair",
                                               "make_tuple",
                                               "override",
                                               "final",
                                               "constexpr",
                                               "decltype",
                                               "nullptr",
                                               "alignof",
                                               "alignas",
                                               "static_assert",
                                               "concept",
                                               "requires",
                                               "co_await",
                                               "co_yield",
                                               "co_return",
                                               "import",
                                               "module",
                                               "export",
                                               "char8_t",
                                               "new",
                                               "delete",
                                               "malloc",
                                               "free",
                                               "realloc",
                                               "calloc",
                                               "cin",
                                               "cout",
                                               "cerr",
                                               "clog",
                                               "printf",
                                               "scanf",
                                               "fgets",
                                               "puts",
                                               "getchar",
                                               "putchar",
                                               "include",
                                               "define",
                                               "undef",
                                               "if",
                                               "ifdef",
                                               "ifndef",
                                               "else",
                                               "elif",
                                               "endif",
                                               "pragma",
                                               "error",
                                               "warning",
                                               "and",
                                               "and_eq",
                                               "bitand",
                                               "bitor",
                                               "compl",
                                               "not",
                                               "not_eq",
                                               "or",
                                               "or_eq",
                                               "xor",
                                               "xor_eq",
                                               "asm",
                                               "goto",
                                               "typedef",
                                               "typeid",
                                               "namespace",
                                               "using",
                                               "std",
                                               "sizeof",
                                               "alignof",
                                               "alignas",
                                               "static_assert",
                                               "constexpr",
                                               "iostream",
                                               "fstream",
                                               "sstream",
                                               "iomanip",
                                               "ios",
                                               "istream",
                                               "ostream",
                                               "streambuf",
                                               "locale",
                                               "algorithm",
                                               "functional",
                                               "iterator",
                                               "numeric",
                                               "utility",
                                               "memory",
                                               "limits",
                                               "exception",
                                               "stdexcept",
                                               "cassert",
                                               "cctype",
                                               "cerrno",
                                               "cfloat",
                                               "ciso646",
                                               "climits",
                                               "clocale",
                                               "cmath",
                                               "csetjmp",
                                               "csignal",
                                               "cstdarg",
                                               "cstddef",
                                               "cstdio",
                                               "cstdint",
                                               "cstdlib",
                                               "cstring",
                                               "ctime",
                                               "cwchar",
                                               "cwctype",
                                               "array",
                                               "bitset",
                                               "deque",
                                               "list",
                                               "map",
                                               "queue",
                                               "set",
                                               "stack",
                                               "vector",
                                               "unordered_map",
                                               "unordered_set",
                                               "complex",
                                               "random",
                                               "ratio",
                                               "regex",
                                               "chrono",
                                               "thread",
                                               "mutex",
                                               "atomic",
                                               "condition_variable",
                                               "future",
                                               "filesystem",
                                               "shared_mutex"};

void HighlightSyntax(WINDOW* win, const std::string& line, int line_num) {
  int x = 2;  // Starting x position for the text
  std::string word;
  bool in_string = false;
  // bool in_comment = false;

  for (size_t i = 0; i < line.length(); ++i) {
    char ch = line[i];

    // Handle comments
    if (!in_string && ch == '/' && i + 1 < line.length() &&
        line[i + 1] == '/') {
      // in_comment = true;
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
