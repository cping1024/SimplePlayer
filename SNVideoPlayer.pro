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


INCLUDEPATH +=/home/sensenets/workspace/SN_Pose/3rdparty/ffmpeg/include
LIBS +=/home/sensenets/workspace/SN_Pose/3rdparty/ffmpeg/lib/libavcodec.so\
    /home/sensenets/workspace/SN_Pose/3rdparty/ffmpeg/lib/libavfilter.so\
    /home/sensenets/workspace/SN_Pose/3rdparty/ffmpeg/lib/libavformat.so\
    /home/sensenets/workspace/SN_Pose/3rdparty/ffmpeg/lib/libswscale.so\
    /home/sensenets/workspace/SN_Pose/3rdparty/ffmpeg/lib/libavutil.so\

INCLUDEPATH +=/usr/include/opencv2\
              /usr/include/opencv
LIBS+=/usr/lib/x86_64-linux-gnu/libopencv_highgui.so\
      /usr/lib/x86_64-linux-gnu/libopencv_core.so\
      /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so\

