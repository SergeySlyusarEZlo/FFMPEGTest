//
// Created by sergey on 15.01.2025.
//

#include "memory_logger.h"
#include <fstream>
#include <iostream>
#include <sstream>

void MemoryLogger::logHeapMemoryUsage(const std::string &label) {
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) {
        std::cerr << "Failed to open /proc/self/status" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("RssAnon:") == 0) {
            std::istringstream iss(line);
            std::string key, value, unit;
            iss >> key >> value >> unit;
            std::cout << label << " - Heap memory (RssAnon) usage: " << value << " KB" << std::endl;
        }

        if (line.find("VmRSS:") == 0) {
            std::istringstream iss(line);
            std::string key, value, unit;
            iss >> key >> value >> unit;
            std::cout << label << " - Heap memory (VmRSS) usage: " << value << " KB" << std::endl;
        }
    }
}