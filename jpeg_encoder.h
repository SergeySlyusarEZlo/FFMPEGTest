#pragma once
#include <string>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

class JPEGEncoder {
public:
    JPEGEncoder();
    ~JPEGEncoder();

    bool static encodeAndSaveToFile(AVFrame *frame, int width, int height, const std::string &outputFile);
private:
    AVCodecContext *jpegCtx;
    const AVCodec *jpegCodec;
};
