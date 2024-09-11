// Copyright 2024 Keys
#include "file_handler/file_handler.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <chrono>  // NOLINT [build/c++11]
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

std::string GetCurrentWorkingDir() {
    char buff[FILENAME_MAX];
    getcwd(buff, FILENAME_MAX);
    return std::string(buff);
}

std::vector<std::string> ListFiles(const std::string& path, const std::string& extension) {
    std::vector<std::string> files;
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string file_name = entry->d_name;
            if (file_name[0] == '.') {
                continue;  // Skip dot files
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

std::vector<std::string> ListDirectories(const std::string& path) {
    std::vector<std::string> directories;
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string dir_name = entry->d_name;
            if (dir_name[0] == '.') {
                continue;  // Skip dot directories
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

std::string ReadFileContent(const std::string& file_path) {
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

std::string RunCppFileWithOutput(const std::string& cpp_file_path) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        return "Error getting current working directory!";
    }

    std::string temp_dir_template = "/tmp/tuiXXXXXX";
    char temp_dir[1024];
    snprintf(temp_dir, sizeof(temp_dir), "%s", temp_dir_template.c_str());

    if (mkdtemp(temp_dir) == nullptr) {
        return "Error creating temporary directory!";
    }

    std::string binary_file = std::string(temp_dir) + "/program";  // Binary file path
    std::string output_file = std::string(temp_dir) + "/output";    // Output file path
    std::string compile_command = "clang++ \"" + cpp_file_path + "\" -o \"" + binary_file + "\"";

    std::string output;

    if (system(compile_command.c_str()) == 0) {
        auto start_time = std::chrono::high_resolution_clock::now();

        FILE* pipe = popen((binary_file + " > " + output_file).c_str(), "r");
        if (!pipe) {
            output = "Error executing program!";
        } else {
            pclose(pipe);

            std::ifstream output_stream(output_file);
            std::string program_output((std::istreambuf_iterator<char>(output_stream)),
                                        std::istreambuf_iterator<char>());
            output += program_output;

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
            output += "\nExecuted in " + std::to_string(duration) + " nanoseconds\n";
        }
    } else {
        output = "Compilation failed for " + cpp_file_path + "\n";
    }

    std::string cleanup_command = "rm -rf " + std::string(temp_dir);
    system(cleanup_command.c_str());

    return output;
}
