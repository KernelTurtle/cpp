// Copyright 2024 Keys
#include "file_handler/file_handler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>  // NOLINT [build/c++11]

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#include <io.h>
#define GetCurrentDir _getcwd
#else
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

std::string GetCurrentWorkingDir() {
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    return std::string(buff);
}

std::vector<std::string> ListFiles(const std::string& path, const std::string& extension) {
    std::vector<std::string> files;

#ifdef _WIN32
    std::string search_path = path + "\\*" + extension;
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::string file_name = fd.cFileName;
            if (file_name[0] != '.') {
                files.push_back(file_name);
            }
        } while (FindNextFile(hFind, &fd) != 0);
        FindClose(hFind);
    } else {
        std::cerr << "Error opening directory: " << path << std::endl;
    }

#else
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string file_name = entry->d_name;
            if (file_name[0] != '.' && file_name.find(extension) != std::string::npos) {
                files.push_back(file_name);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error opening directory: " << path << std::endl;
    }
#endif

    return files;
}

std::vector<std::string> ListDirectories(const std::string& path) {
    std::vector<std::string> directories;

#ifdef _WIN32
    std::string search_path = path + "\\*";
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::string dir_name = fd.cFileName;
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && dir_name[0] != '.') {
                directories.push_back(dir_name);
            }
        } while (FindNextFile(hFind, &fd) != 0);
        FindClose(hFind);
    } else {
        std::cerr << "Error opening directory: " << path << std::endl;
    }

#else
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((entry = readdir(dir)) != nullptr) {
            std::string dir_name = entry->d_name;
            if (entry->d_type == DT_DIR && dir_name[0] != '.') {
                directories.push_back(dir_name);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error opening directory: " << path << std::endl;
    }
#endif

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
    std::string temp_dir;
    std::string compile_command;
    std::string binary_file;
    std::string output_file;

#ifdef _WIN32
    char temp_path[MAX_PATH];
    GetTempPath(MAX_PATH, temp_path);
    temp_dir = std::string(temp_path) + "tuiXXXXXX";
    binary_file = temp_dir + "\\program.exe";  // Binary file path for Windows
    output_file = temp_dir + "\\output.txt";   // Output file path for Windows
    compile_command = "g++ \"" + cpp_file_path + "\" -o \"" + binary_file + "\"";
    _mkdir(temp_dir.c_str());
#else
    temp_dir = "/tmp/tuiXXXXXX";
    char temp_dir_template[1024];
    snprintf(temp_dir_template, sizeof(temp_dir_template), "%s", temp_dir.c_str());

    if (mkdtemp(temp_dir_template) == nullptr) {
        return "Error creating temporary directory!";
    }

    temp_dir = std::string(temp_dir_template);
    binary_file = temp_dir + "/program";   // Binary file path for Linux
    output_file = temp_dir + "/output";    // Output file path for Linux
    compile_command = "clang++ \"" + cpp_file_path + "\" -o \"" + binary_file + "\"";
#endif

    std::string output;

    if (system(compile_command.c_str()) == 0) {
        auto start_time = std::chrono::high_resolution_clock::now();

#ifdef _WIN32
        std::string exec_command = binary_file + " > \"" + output_file + "\"";
#else
        std::string exec_command = binary_file + " > " + output_file;
#endif

        FILE* pipe = popen(exec_command.c_str(), "r");
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

#ifdef _WIN32
    std::string cleanup_command = "rmdir /S /Q \"" + temp_dir + "\"";
#else
    std::string cleanup_command = "rm -rf " + temp_dir;
#endif

    system(cleanup_command.c_str());

    return output;
}
