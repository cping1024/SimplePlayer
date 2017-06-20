#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QRect>
#include <QImage>
#include <QPainter>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("SNPlayer"));
    translateUi();
    //setFormStyle();
    connectSignal();
    decoder = new SN_AVDecoder();
}

MainWindow::~MainWindow()
{
    if (decoder) {
        delete decoder;
        decoder = NULL;
    }

    delete ui;
}

//void MainWindow::paintEvent(QPaintEvent *event)
//{
//    QPainter painter(this);
//    QRect rect = ui->videodisplaywidget->rect();

//    std::lock_guard<std::mutex> lk(mutex_);
//    if (!video_image_.isNull()) {
//        QImage img = video_image_.scaled(rect.width(), rect.height());
//        painter.drawImage(rect, img);
//    }

//    painter.end();
//}

void MainWindow::translateUi()
{
    ui->scanpushButton->setText(tr("浏览"));
    ui->startpushButton->setText(tr("播放"));
    ui->stoppushButton->setText(tr("停止"));
    ui->urllabel->setText(tr("URL:"));
}

void MainWindow::connectSignal()
{
    connect(ui->startpushButton, SIGNAL(clicked()), this, SLOT(onStartBtnClicked()));
    connect(ui->stoppushButton, SIGNAL(clicked()), this, SLOT(onStopBtnClicked()));
    connect(ui->scanpushButton, SIGNAL(clicked()), this, SLOT(onScanBtnClicked()));
}

void MainWindow::setFormStyle()
{
    QPalette palette;
    palette.setBrush(QPalette::Background,\
           QBrush(QPixmap("C:\\Users\allen\\code\\build-SNVideoPlayer-Desktop_Qt_5_5_1_MinGW_32bit-Debug\\debug\\video_bk.jpg")));
    setPalette(palette);
}

void MainWindow::DecodeCallBack(void *buffer, void *frame, void *caller, int width, int height)
{
    if (!buffer ||!frame || !caller) {
        return;
    }

    MainWindow* window = static_cast<MainWindow*>(caller);
    if (!window) {
        return;
    }

    window->handleDecodeCallBack(buffer, frame, width, height);
}

void MainWindow::handleDecodeCallBack(void *buffer, void *frame, int width, int height)
{
    AVFrame *avframe = static_cast<AVFrame*>(frame);
    if (!avframe) {
        return ;
    }

    cv::Mat image = cv::Mat(height, width, CV_8UC3, buffer, avframe->linesize[0]);
    if (!image.data) {
        return;
    }
    cv::imshow("frame", image);
    cv::waitKey(1);


//    cv::cvtColor(image, image, CV_BGR2RGB);
//    QImage img = QImage((const unsigned char*)image.data, image.cols, image.rows, image.cols * image.channels(), QImage::Format_RGB888);
//    if (!img.isNull()) {
//        QRect rect = ui->videodisplaylabel->geometry();
//        img = img.scaled(rect.width(), rect.height());
//        ui->videodisplaylabel->setPixmap(QPixmap::fromImage(img));
//    }
    /*
    std::lock_guard<std::mutex> lk(mutex_);
    video_image_ = QImage((const unsigned char*)image.data, image.cols, image.rows, image.cols * image.channels(), QImage::Format_RGB888);
    if (video_image_.isNull()) {
        return;
    }

    /// send signal and update UI
    update();
    */
}

int MainWindow::startVideo()
{
    if (started) {
        return 0;
    }

    QString qurl = ui->urllineEdit->text();
    if (qurl.isEmpty()) {
        return -1;
    }

    decoder->setStreamUrl(qurl.toStdString());
    decoder->setCallBack(DecodeCallBack, (void*)this);

    int ret = decoder->open();
    if (ret != 0) {
        return ret;
    }

    ret = decoder->start();
    if (ret != 0){
        return ret;
    }

    started = true;
    return 0;
}

int MainWindow::stopVideo()
{
    if (!decoder) {
        return -1;
    }

    /// stop stream reader
    decoder->stop();

    ///close decpde
    decoder->close();

    /// set 'started' flag
    started = false;
    return 0;
}

void MainWindow::onStartBtnClicked()
{
    startVideo();
}

void MainWindow::onStopBtnClicked()
{
    //stopVideo();
}

void MainWindow::onScanBtnClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), "./","");
    if (filename.isEmpty()) {
        return;
    }

    ui->urllineEdit->setText(filename);
}
