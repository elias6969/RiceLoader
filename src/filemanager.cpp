#include "fileManager.h"
#include <fstream> 

VirtualFileSystem::VirtualFileSystem(const std::string& basePath) : baseDir(basePath) {
    if (!fs::exists(baseDir)) {
        std::cerr << "Base directory does not exist: " << fs::absolute(baseDir) << std::endl;
        throw std::runtime_error("Base directory does not exist: " + baseDir);
    }
    std::cout << "Base directory found: " << fs::absolute(baseDir) << std::endl;
}

std::string VirtualFileSystem::readFile(const std::string& relativePath) {
    std::string fullPath = (fs::path(baseDir) / relativePath).string();
    if (!fs::exists(fullPath)) {
        throw std::runtime_error("File not found: " + fullPath);
    }

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + fullPath);
    }

    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}
