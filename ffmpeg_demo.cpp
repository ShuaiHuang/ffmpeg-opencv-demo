#include <iostream>
#include "opencv2/opencv.hpp"

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
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

    av_dump_format(format_ctx_ptr, 0, FILE_PATH.c_str(), 0);

    cv::namedWindow("Video");

    AVFrame *frame_ptr = av_frame_alloc();

    AVFrame *frame_bgr_ptr = av_frame_alloc();
    int frame_size = avpicture_get_size(AV_PIX_FMT_BGR24, codec_ctx_ptr->width, codec_ctx_ptr->height);
    uint8_t *out_buffer = (uint8_t *)av_malloc(frame_size);
    avpicture_fill((AVPicture *)frame_bgr_ptr, out_buffer, AV_PIX_FMT_BGR24, codec_ctx_ptr->width, codec_ctx_ptr->height);

    struct SwsContext *img_convert_ctx;
    img_convert_ctx = sws_getContext(codec_ctx_ptr->width,
        codec_ctx_ptr->height, 
        codec_ctx_ptr->pix_fmt, 
        codec_ctx_ptr->width, 
        codec_ctx_ptr->height,
        AV_PIX_FMT_BGR24, //设置sws_scale转换格式为BGR24，这样转换后可以直接用OpenCV显示图像了
        SWS_BICUBIC, 
        NULL, NULL, NULL);


    AVPacket *packet_ptr = (AVPacket*)av_malloc(sizeof(AVPacket));
    int got_picture = 0;
    while (av_read_frame(format_ctx_ptr, packet_ptr) >= 0)
    {
        if (packet_ptr->stream_index == video_idx)
        {
            int ret = avcodec_decode_video2(codec_ctx_ptr, frame_ptr, &got_picture, packet_ptr);
            if (ret < 0)
            {
                std::cout << "decode fail" << std::endl;
                return -1;
            }
            if (got_picture)
            {
                sws_scale(img_convert_ctx, 
                        (const uint8_t* const*)frame_ptr->data, 
                        frame_ptr->linesize,
                        0,
                        codec_ctx_ptr->height,
                        frame_bgr_ptr->data,
                        frame_bgr_ptr->linesize);

                // std::cout << "succeed to decode frame: " << codec_ctx_ptr->frame_number << std::endl;
                cv::Mat img(codec_ctx_ptr->height, codec_ctx_ptr->width, CV_8UC3);
                img.data = (uchar*)frame_bgr_ptr->data[0];
                cv::imshow("Video", img);
                cv::waitKey(20);
            }
        }
        av_free_packet(packet_ptr);
    }

    cv::destroyWindow("Video");

    av_frame_free(&frame_ptr);
    av_frame_free(&frame_bgr_ptr);
    avcodec_close(codec_ctx_ptr);
    avformat_close_input(&format_ctx_ptr);
    return 0;
}