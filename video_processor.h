
#pragma once

#include <string>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

class VideoProcessor {
public:
    VideoProcessor();
    ~VideoProcessor();

    bool open(const std::string &inputFile);
    bool decodeAndSaveFrame(const std::string &outputFile);
    void close();

private:
    AVFormatContext *formatCtx = nullptr;
    AVCodecContext *decoderCtx = nullptr;
    SwsContext *swsCtx = nullptr;
    int videoStreamIndex = 0;
};

