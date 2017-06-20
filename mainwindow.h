#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <sn_avdecoder.h>
#include <mutex>
#include <QImage>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    //void paintEvent(QPaintEvent* event);

private:
    void translateUi();
    void connectSignal();
    void setFormStyle();

    static void DecodeCallBack(void* buffer, void* frame, void* caller, int width, int height);
    void handleDecodeCallBack(void* buffer, void* frame, int width, int height);

    int startVideo();
    int stopVideo();

private slots:
    void onStartBtnClicked();
    void onStopBtnClicked();
    void onScanBtnClicked();

private:
    bool started = false;
    std::mutex mutex_;
    QImage video_image_;
    SN_AVDecoder* decoder = NULL;


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
