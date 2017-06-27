#include "sn_avdecoder.h"
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace std::chrono;
#include <sn_video_packet_list.h>

SN_AVDecoder::SN_AVDecoder()
    :m_bDecoderInit(false)
    ,m_bDecodeThreadInit(false)
{
    initMember();
    list_.reset(new SNVideoPacketList(100));
}

SN_AVDecoder::~SN_AVDecoder()
{
//    if(NULL != m_pReader)
//    {
//        delete m_pReader;
//        m_pReader = NULL;
//    }
}

void SN_AVDecoder::setStreamUrl(const std::string &p_strUrl)
{
    m_strUrl = p_strUrl;

//    if(NULL != m_pReader){
//        m_pReader->initReader(p_strUrl);
//    }
}

void SN_AVDecoder::setVideoSize(int p_nWidth, int p_nHeight)
{
    //m_nWidth = p_nWidth;
    //m_nHeight = p_nHeight;
}

void SN_AVDecoder::setCallBack(DecodeCallBack fun, void *args)
{
    m_pCallBack = fun;
    m_pCaller = args;
}

int SN_AVDecoder::open()
{
    int nRet = initDecode();
    if(nRet != 0)
        return nRet;

    return 0;
}

void SN_AVDecoder::close()
{
    unInitDecode();
}

int SN_AVDecoder::start()
{
//    if(NULL == m_pReader)
//    {
//        std::cout << "no stream reader" << std::endl;
//        return -1;
//    }

//    int nRet = m_pReader->start();
//    if(nRet != 0)
//        return nRet;

    if(!m_bDecodeThreadInit)
    {
        int nRet = initDecodeThread();
        if(nRet != 0)
            return nRet;
    }

    return 0;
}

void SN_AVDecoder::stop()
{
    //todo
}

void SN_AVDecoder::decode()
{
    while(!exit_)
    {
        AVPacket* packet = list_->Pop();
        if (!packet) {
            continue;
        }
        //steady_clock::time_point start = steady_clock::now();
        int get_frame = -1;
        int nRet = avcodec_decode_video2(m_pCodecCtx, m_pDecodeFrame, &get_frame, packet);
//        steady_clock::time_point stop = steady_clock::now();
//        milliseconds time = duration_cast<milliseconds>(stop - start);
//        std::cout << "decode frame ret:" << nRet << ", time :" << time.count() << "ms" << std::endl;

        if(nRet < 0){
            continue;
        }

        if(get_frame) {
            //std::cout << "decode a frame succeed!" << std::endl;
            sws_scale(m_pSwsCtx, (const unsigned char* const*)m_pDecodeFrame->data, m_pDecodeFrame->linesize, 0, m_pCodecCtx->height, m_pOutputFrame->data, m_pOutputFrame->linesize);

            //if need ,excute callback
            if(m_pCallBack)
                m_pCallBack(m_pBuffer, m_pOutputFrame, m_pCaller, m_nWidth, m_nHeight);
        }

        av_free_packet(packet);
    }
}

void SN_AVDecoder::readFrame()
{
    while(!exit_) {

        AVPacket *packet = new AVPacket();
        av_init_packet(packet);
        if (!packet) {
            continue;
        }

        steady_clock::time_point start = steady_clock::now();
        int ret = av_read_frame(m_pFormatCtx, packet);
        steady_clock::time_point stop = steady_clock::now();
        milliseconds time = duration_cast<milliseconds>(stop - start);

        if (ret == 0 && packet->stream_index == video_index_) {
            list_->Push(packet);
        }

        /// sleep 10ms
        usleep(25* 1000);
    }
}

void SN_AVDecoder::initMember()
{
    m_bDecoderInit = false;

    m_pFormatCtx = NULL;
    m_pCodec = NULL;
    m_pCodecCtx = NULL;
    m_pCodecParserCtx = NULL;
    m_pDecodeFrame = NULL;
    m_pOutputFrame = NULL;
    m_pBuffer = NULL;
    m_pSwsCtx = NULL;

    //m_pReader = NULL;
    m_pCallBack = NULL;

    m_bDecodeThreadInit = false;
}

int SN_AVDecoder::initDecode()
{
    //register codecs,mux,demux and so on
    av_register_all();
    //init net lib
    avformat_network_init();
    //get avformatContext
    m_pFormatCtx = avformat_alloc_context();

    //open a input stream
    AVDictionary* options = NULL;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "3500000", 0);
    if(avformat_open_input(&m_pFormatCtx, m_strUrl.c_str(), NULL, &options)!=0)
    {
        std::cout << ("Couldn't open input stream") << std::endl;
        return -1;
    }

    //find stream info
    if(avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
    {
        std::cout << ("Couldn't find stream information") << std::endl;
        return -1;
    }

    //get video stream
    //int videoindex = -1;
    for(int ix = 0; ix < m_pFormatCtx->nb_streams; ++ix)
    {
        if(m_pFormatCtx->streams[ix]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_index_ = ix;
            break;
        }
    }

    //not find video stream
    if(video_index_ == -1)
    {
        std::cout << ("Didn't find a video stream.") << std::endl;
        return -1;
    }

    //
    m_pCodecCtx = m_pFormatCtx->streams[video_index_]->codec;
    m_pCodec= avcodec_find_decoder(m_pCodecCtx->codec_id);
    if(NULL == m_pCodec)
    {
        std::cout << ("Codec not found") << std::endl;
        return -1;
    }

    //open decorder
    if(avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0)
    {
        std::cout << ("Could not open codec.") << std::endl;
        return -1;
    }


    m_pDecodeFrame = av_frame_alloc();
    m_pOutputFrame = av_frame_alloc();


    m_nWidth = m_pCodecCtx->width;
    m_nHeight = m_pCodecCtx->height;

    std::cout << "m_pCodecCtx w:" << m_nWidth << "h:" << m_nHeight << std::endl;

    m_pBuffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_BGR24, m_nWidth, m_nHeight, 1));

    av_image_fill_arrays(m_pOutputFrame->data, m_pOutputFrame->linesize, m_pBuffer
                                             , AV_PIX_FMT_BGR24, m_nWidth, m_nHeight, 1);

    m_pSwsCtx = sws_getContext(m_pCodecCtx->width, m_pCodecCtx->height, AV_PIX_FMT_YUV420P
                                                 , m_nWidth, m_nHeight, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
    if(NULL == m_pSwsCtx)
    {
        std::cout << "can't create sws converter!" << std::endl;
        return -1;
    }
    return 0;
}

void SN_AVDecoder::unInitDecode()
{
    av_parser_close(m_pCodecParserCtx);
    avcodec_close(m_pCodecCtx);
    av_free(m_pCodecCtx);
    //todo
}

int SN_AVDecoder::initDecodeThread()
{
    std::thread decode_thread(&SN_AVDecoder::decode, this);
    if (decode_thread.joinable()) {
        decode_thread.detach();
    }

    std::thread read_thread(&SN_AVDecoder::readFrame, this);
    if (read_thread.joinable()) {
        read_thread.detach();
    }

    m_bDecodeThreadInit = true;
    //pthread_attr_destroy(&m_objAttr);
    return 0;
}

void SN_AVDecoder::unInitDecodeThread()
{
    //pthread_cancel(m_nThreadId);
}


