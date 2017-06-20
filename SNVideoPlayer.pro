#-------------------------------------------------
#
# Project created by QtCreator 2017-06-19T20:12:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SNVideoPlayer
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    sn_video_frame_provider.cpp \
    sn_video_packet_list.cpp \
    sn_avdecoder.cpp

HEADERS  += mainwindow.h \
    sn_video_frame_provider.h \
    sn_video_packet_list.h \
    sn_avdecoder.h

FORMS    += mainwindow.ui

###############################opencv########################################

INCLUDEPATH += C:\Users\allen\deps\opencv\include\opencv2\
              C:\Users\allen\deps\opencv\\include\opencv\
             C:\Users\allen\deps\opencv\include
LIBS+=C:\Users\allen\deps\opencv\lib\libopencv_core2410.dll\
      C:\Users\allen\deps\opencv\lib\libopencv_highgui2410.dll\
      C:\Users\allen\deps\opencv\lib\libopencv_imgproc2410.dll\

###############################ffmpeg########################################

INCLUDEPATH += C:\Users\allen\deps\ffmpeg\include
LIBS += C:\Users\allen\deps\ffmpeg\lib\SN_avcodec.dll\
        C:\Users\allen\deps\ffmpeg\lib\SN_avfilter.dll\
        C:\Users\allen\deps\ffmpeg\lib\SN_avformat.dll\
        C:\Users\allen\deps\ffmpeg\lib\SN_swscale.dll\
        C:\Users\allen\deps\ffmpeg\lib\SN_avutil.dll\
