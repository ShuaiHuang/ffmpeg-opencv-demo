#include <iostream>
#include "opencv2/opencv.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#ifdef __cplusplus
}
#endif

int main(int argc, char* argv[])
{
    av_register_all();

    AVFormatContext *format_ctx_ptr = avformat_alloc_context();
    std::string FILE_PATH = "../data/let_her_go.mp4";
    if (avformat_open_input(&format_ctx_ptr, FILE_PATH.c_str(), NULL, NULL))
    {
        std::cout << "opencv file error! " << FILE_PATH << std::endl;
        return -1;
    }

    if (avformat_find_stream_info(format_ctx_ptr, NULL) < 0)
    {
        std::cout << "couldn't find stream information." << std::endl;
        return -1;
    }

    int video_idx = -1;
    for (int idx = 0; idx < format_ctx_ptr->nb_streams; idx++)
    {
        if (format_ctx_ptr->streams[idx]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_idx = idx;
            break;
        }
    }
    if (video_idx == -1)
    {
        std::cout << "couldn't find video stream" << std::endl;
        return -1;
    }

    AVCodecContext *codec_ctx_ptr = format_ctx_ptr->streams[video_idx]->codec;
    AVCodec *codec_ptr = avcodec_find_decoder(codec_ctx_ptr->codec_id);
    if (!codec_ptr)
    {
        std::cout << "codec not found" << std::endl;
        return -1;
    }

    if (avcodec_open2(codec_ctx_ptr, codec_ptr, NULL) < 0)
    {
        std::cout << "couldn't open codec" << std::endl;
        return -1;
    }

    AVFrame *av_frame_ptr = nullptr;
    avcodec_close(codec_ctx_ptr);
    avformat_close_input(&format_ctx_ptr);
    return 0;
}