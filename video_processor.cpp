
#include "video_processor.h"
#include "jpeg_encoder.h"

#include <iostream>
#include <string>

VideoProcessor::VideoProcessor() : formatCtx(nullptr), decoderCtx(nullptr), videoStreamIndex(-1), swsCtx(nullptr) {}

VideoProcessor::~VideoProcessor() {
    close();
}

bool VideoProcessor::open(const std::string &inputFile) {
    avformat_network_init();

    if (avformat_open_input(&formatCtx, inputFile.c_str(), nullptr, nullptr) != 0) {
        std::cerr << "Couldn't open input file" << std::endl;
        return false;
    }

    if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
        std::cerr << "Couldn't find stream info" << std::endl;
        return false;
    }

    const AVCodec *decoder = nullptr;
    for (unsigned int i = 0; i < formatCtx->nb_streams; i++) {
        if (formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            decoder = avcodec_find_decoder(formatCtx->streams[i]->codecpar->codec_id);
            break;
        }
    }

    if (videoStreamIndex == -1 || !decoder) {
        std::cerr << "Couldn't find video stream or decoder" << std::endl;
        return false;
    }

    decoderCtx = avcodec_alloc_context3(decoder);
    if (!decoderCtx) {
        std::cerr << "Not allocate decoder context" << std::endl;
        return false;
    }

    avcodec_parameters_to_context(decoderCtx, formatCtx->streams[videoStreamIndex]->codecpar);

    if (avcodec_open2(decoderCtx, decoder, nullptr) < 0) {
        std::cerr << "Not open decoder" << std::endl;
        return false;
    }

    return true;
}

bool VideoProcessor::decodeAndSaveFrame(const std::string &outputFile) {
    AVPacket packet;
    AVFrame *frame = av_frame_alloc();

    while (av_read_frame(formatCtx, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(decoderCtx, &packet) == 0) {
                while (avcodec_receive_frame(decoderCtx, frame) == 0) {
                    if (!swsCtx) {
                        swsCtx = sws_getContext(frame->width, frame->height, decoderCtx->pix_fmt,
                                                frame->width, frame->height, AV_PIX_FMT_YUVJ420P,
                                                SWS_BICUBIC, nullptr, nullptr, nullptr);
                    }

                    if (!JPEGEncoder::encodeAndSaveToFile(frame, frame->width, frame->height, outputFile)) {
                        std::cerr << "Failed to save frame as JPEG" << std::endl;
                        av_packet_unref(&packet);
                        av_frame_free(&frame);
                        return false;
                    }

                    std::cout << "Saved first frame to " << outputFile << std::endl;
                    av_packet_unref(&packet);
                    av_frame_free(&frame);
                    return true;
                }
            }
        }
        av_packet_unref(&packet);
    }

    av_frame_free(&frame);
    return false;
}

void VideoProcessor::close() {
    if (formatCtx) {
        avformat_close_input(&formatCtx);
        formatCtx = nullptr;
    }
    if (decoderCtx) {
        avcodec_free_context(&decoderCtx);
        decoderCtx = nullptr;
    }
    if (swsCtx) {
        sws_freeContext(swsCtx);
        swsCtx = nullptr;  /// THIs line sometimes reduce memory 'reachable' amount
    }
}
