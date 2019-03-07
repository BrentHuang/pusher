#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QVideoFrame>
#include <QMediaRecorder>

class Camera : public QWidget
{
    Q_OBJECT

public:
    Camera();
    virtual ~Camera();

private slots:
    void OnRecordButton();
    void OnImageCaptured(int id, const QImage& preview);
    void OnVideoFrame(const QVideoFrame& frame);
    void DisplayCameraError(QCamera::Error error);

private:
    QCamera*             camera_;       // 摄像头
    QCameraViewfinder*   viewfinder_;   // 取景器
    QCameraImageCapture* capturer_; // 获取摄像头当前帧
    QMediaRecorder* recorder_;
};

#endif // CAMERA_H
