
#include "jpeg_encoder.h"
#include <iostream>
#include <fstream>

JPEGEncoder::JPEGEncoder() : jpegCtx(nullptr), jpegCodec(nullptr) {
    jpegCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!jpegCodec) {
        std::cerr << "Could't find MJPEG codec" << std::endl;
    }
}

JPEGEncoder::~JPEGEncoder() {
    if (jpegCtx) {
        avcodec_free_context(&jpegCtx);
    }
}

bool JPEGEncoder::encodeAndSaveToFile(AVFrame *frame, int width, int height, const std::string &outputFile) {
    const AVCodec *jpegCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!jpegCodec) {
        std::cerr << "Could't find MJPEG codec" << std::endl;
        return false;
    }

    AVCodecContext *jpegCtx = avcodec_alloc_context3(jpegCodec);
    if (!jpegCtx) {
        std::cerr << "Could't allocate MJPEG codec context" << std::endl;
        return false;
    }

    jpegCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    jpegCtx->width = width;
    jpegCtx->height = height;
    jpegCtx->time_base = {1, 25};

    if (avcodec_open2(jpegCtx, jpegCodec, nullptr) < 0) {
        std::cerr << "Could not open MJPEG codec" << std::endl;
        avcodec_free_context(&jpegCtx);
        return false;
    }

    AVPacket packet;
    av_init_packet(&packet);
    packet.data = nullptr;
    packet.size = 0;

    if (avcodec_send_frame(jpegCtx, frame) < 0) {
        std::cerr << "Error sending frame" << std::endl;
        av_packet_unref(&packet);
        avcodec_free_context(&jpegCtx);
        return false;
    }

    if (avcodec_receive_packet(jpegCtx, &packet) == 0) {
        std::ofstream file(outputFile, std::ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<char *>(packet.data), packet.size);
            file.close();
        }
        av_packet_unref(&packet);
    } else {
        std::cerr << "Err receive packet from encoder" << std::endl;
    }

    avcodec_free_context(&jpegCtx);
    return true;
}