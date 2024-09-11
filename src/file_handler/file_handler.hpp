// Copyright 2024 Keys
#ifndef SRC_FILE_HANDLER_FILE_HANDLER_HPP_
#define SRC_FILE_HANDLER_FILE_HANDLER_HPP_

#include <string>
#include <vector>

std::string GetCurrentWorkingDir();
std::vector<std::string> ListFiles(const std::string& path,
                                   const std::string& extension);
std::vector<std::string> ListDirectories(const std::string& path);
std::string ReadFileContent(const std::string& file_path);
std::string RunCppFileWithOutput(const std::string& cpp_file_path);

#endif  // SRC_FILE_HANDLER_FILE_HANDLER_HPP_
