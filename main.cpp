
#include "video_processor.h"
#include "memory_logger.h"

#include <thread>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>

void prepareTestDirectory(const std::string &testDir) {
    struct stat info;
    if (stat(testDir.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
        std::string command = "rm -rf " + testDir;
        if (system(command.c_str()) != 0) {
            std::cerr << "Error removing existing directory: " << testDir << std::endl;
            exit(-1);
        }
    }
    if (mkdir(testDir.c_str(), 0755) != 0) {
        std::cerr << "Error creating directory: " << testDir << std::endl;
        exit(-1);
    }
}

void processVideo(const std::string &inputFile, const std::string &outputFileBase, int threadId, int repeats) {
    for (int i = 0; i < repeats; ++i) {
        std::string outputFile = outputFileBase + "_thread" + std::to_string(threadId) + "_repeat" + std::to_string(i) + ".jpg";
        VideoProcessor videoProcessor;
        if (!videoProcessor.open(inputFile)) {
            std::cerr << "Thread " << threadId << ": Failed to open input file" << std::endl;
            continue;
        }

        if (!videoProcessor.decodeAndSaveFrame(outputFile)) {
            std::cerr << "Thread " << threadId << ": Failed to save frame as JPEG" << std::endl;
        }

       // videoProcessor.close();
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <inputThreadsNumber> <RepeatesNumber>" << std::endl << "For example: ./test-frame 6 100" << std::endl;
        return -1;
    }

 //   av_log_set_level(AV_LOG_TRACE);

    printf("libavformat version: %s\n", av_version_info());

    const std::string inputFile = "input.mp4"; // default source video
    const std::string outputFileBase = "output"; // prefix of out snapshots

    int inputThreadsNumber = std::stoi(argv[1]);
    int repeatsNumber = std::stoi(argv[2]);

    if (inputThreadsNumber <= 0 || repeatsNumber <= 0) {
        std::cerr << "Error: inputThreadsNumber and RepeatesNumber must be positive integers." << std::endl;
        return -1;
    }

    MemoryLogger::logHeapMemoryUsage("Start");

    std::vector<std::thread> threads;

    const std::string testDirectory = "./test_output";
    prepareTestDirectory(testDirectory);

    for (int i = 0; i < inputThreadsNumber; ++i) {
        threads.emplace_back(processVideo, inputFile, testDirectory + "/" + outputFileBase, i, repeatsNumber);
    }

    for (auto &t : threads) {
        t.join();
    }

    MemoryLogger::logHeapMemoryUsage("Finish");

    return 0;
}
