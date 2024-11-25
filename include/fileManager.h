#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

class VirtualFileSystem {
private:
    std::string baseDir;

public:
    // Constructor declaration
    VirtualFileSystem(const std::string& basePath);

    // Member function declaration
    std::string readFile(const std::string& relativePath);
};
#endif