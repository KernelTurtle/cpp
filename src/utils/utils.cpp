// Copyright 2024 Keys
#include "utils/utils.hpp"

#include <algorithm>
#include <string>

std::string FormatFileName(const std::string& file_name) {
  std::string base_name = file_name.substr(
      0, file_name.find_last_of("."));  // Remove ".cpp" extension
  std::replace(base_name.begin(), base_name.end(), '_', ' ');
  return base_name + " iteration";  // Append " iteration" to the base name
}
