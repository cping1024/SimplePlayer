#ifndef SN_AVDECODER_H
#define SN_AVDECODER_H

#include <string>
#include <memory>
#ifdef __cplusplus
extern "C"{
#endif

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#ifdef __cplusplus
}
#endif



typedef void (*DecodeCallBack)(void *buffer, void *frame, void *caller, int width, int height);

class SNVideoPacketList;
class SN_AVDecoder
{
public:
    SN_AVDecoder();
    ~SN_AVDecoder();
    void setStreamUrl(const std::string& p_strUrl);

    //w,h
    void setVideoSize(int p_nWidth, int p_nHeight);

    //decode a frame and callback to show
    void setCallBack(DecodeCallBack fun, void *args);

    //init
    int open();
    void close();

    int start();
    void stop();

private:
    void decode();
    void readFrame();

    void initMember();

    int initDecode();
    void unInitDecode();

    int initDecodeThread();
    void unInitDecodeThread();

private:
    bool                        m_bDecoderInit = false;

    bool exit_ = false;

    int                         m_nWidth = 0;
    int                         m_nHeight = 0;
    int video_index_ = -1;


    std::string                 m_strUrl;
    AVFormatContext             *m_pFormatCtx = NULL;
    AVCodec                     *m_pCodec = NULL;
    AVCodecContext              *m_pCodecCtx = NULL;
    AVCodecParserContext        *m_pCodecParserCtx = NULL;
    AVFrame                     *m_pDecodeFrame = NULL;
    AVFrame                     *m_pOutputFrame = NULL;
    unsigned char               *m_pBuffer = NULL;
    struct SwsContext           *m_pSwsCtx = NULL;

    //SNVideoFrameReader           *m_pReader = NULL;
    std::shared_ptr<SNVideoPacketList> list_;

    DecodeCallBack              m_pCallBack;
    void                        *m_pCaller;

    bool                        m_bDecodeThreadInit;
};

#endif // SN_AVDECODER_H
