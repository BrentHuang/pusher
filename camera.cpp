#include "camera.h"
#include <QDebug>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QVideoProbe>
#include <QCameraInfo>

Camera::Camera()
{
    // 获取所连接的摄像头列表
    foreach (const QCameraInfo& camera_info, QCameraInfo::availableCameras())
    {
        qDebug() << camera_info.description() << camera_info.deviceName() << camera_info.orientation() << camera_info.position();
    }

    qDebug() << "default camera: " << QCameraInfo::defaultCamera().description();

//    camera_ = new QCamera(QCameraInfo::defaultCamera(), this);
    if (QCameraInfo::availableCameras().length() > 1)
    {
        camera_ = new QCamera(QCameraInfo::availableCameras().at(1), this);
    }

    viewfinder_ = new QCameraViewfinder(this);
    capturer_ = new QCameraImageCapture(camera_);

    QPushButton* button1 = new QPushButton("Capture");
    QPushButton* button2 = new QPushButton("Record");
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(viewfinder_);
    mainLayout->addWidget(button1);
    mainLayout->addWidget(button2);

    connect(button1, SIGNAL(clicked()), capturer_, SLOT(capture()));
    connect(button2, SIGNAL(clicked()), this, SLOT(OnRecordButton()));
    connect(capturer_, SIGNAL(imageCaptured(int, QImage)), this, SLOT(OnImageCaptured(int, QImage)));
    connect(camera_, SIGNAL(error(QCamera::Error)), this, SLOT(DisplayCameraError(QCamera::Error)));

    viewfinder_->setGeometry(10, 10, 320, 240);

    capturer_->setBufferFormat(QVideoFrame::PixelFormat::Format_YUV420P);

    if (capturer_->isCaptureDestinationSupported(QCameraImageCapture::CaptureToFile))
    {
        qDebug() << "support capture to file";
    }
    if (capturer_->isCaptureDestinationSupported(QCameraImageCapture::CaptureToBuffer))
    {
        qDebug() << "support capture to buffer";
    }

    capturer_->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

    if (camera_->isCaptureModeSupported(QCamera::CaptureStillImage))
    {
        qDebug() << "support capture still image";
    }
    if (camera_->isCaptureModeSupported(QCamera::CaptureVideo))
    {
        qDebug() << "support capture video";
    }
    if (camera_->isCaptureModeSupported(QCamera::CaptureViewfinder))
    {
        qDebug() << "support capture viewfinder";
    }

    camera_->setCaptureMode(QCamera::CaptureStillImage);

//    像素纵横比是指像素的宽 (x) 与高 (y) 之比。正方形像素的比例为 1:1，但非正方形（矩形）像素的高和宽不相同。
//    这一概念类似于帧纵横比，后者是图像的整个宽度与高度之比。通常，电视像素是矩形，计算机像素是正方形。
//    因此，在计算机显示器上看起来合适的图像在电视屏幕上会变形，显示球形图像时尤其明显

    // 采集的视频的参数：分辨率，像素宽高比，最大帧率，最小帧率，像素格式等
//    QCameraViewfinderSettings vf_settings;
//    vf_settings.setResolution(1024, 768);
////    vf_settings.setPixelAspectRatio();
//    vf_settings.setMaximumFrameRate(30);
//    vf_settings.setMinimumFrameRate(15);
//    vf_settings.setPixelFormat(QVideoFrame::Format_YUV420P);

//    camera_->setViewfinderSettings(vf_settings);

    camera_->setViewfinder(viewfinder_);
    camera_->start();

    // 返回支持的取景器分辨率列表
    for (auto resolution : camera_->supportedViewfinderResolutions())
    {
        //dosomething about the resolution
        qDebug() << resolution;
    }

    for (auto frame_rate_range : camera_->supportedViewfinderFrameRateRanges())
    {
        qDebug() << frame_rate_range.minimumFrameRate << frame_rate_range.maximumFrameRate;
    }

    for (auto pixel_format : camera_->supportedViewfinderPixelFormats())
    {
        qDebug() << pixel_format;
    }

    for (auto vf_settings : camera_->supportedViewfinderSettings())
    {
        qDebug() << vf_settings.minimumFrameRate() << vf_settings.maximumFrameRate()
                 << vf_settings.pixelAspectRatio() << vf_settings.pixelFormat()
                 << vf_settings.resolution();
    }

    QVideoProbe* probe = new QVideoProbe(camera_);
    if (probe->setSource(camera_))
    {
        // 一旦有探测到有视频，就触发了ProcessVideoFrame函数
        connect(probe, SIGNAL(videoFrameProbed(QVideoFrame)), this, SLOT(OnVideoFrame(QVideoFrame)));
    }

    recorder_ = new QMediaRecorder(camera_);

    if (!recorder_->setOutputLocation(QUrl()))  // 录制出来的视频文件在“/kkkk.ogg”,  可用totem来播放
    {
        qDebug() << "failed to set output location";
    }
    qDebug() << recorder_->outputLocation();
}

Camera::~Camera()
{
    recorder_->stop();
    camera_->stop();

    delete       camera_;
    delete   viewfinder_;
    delete capturer_;
}

void Camera::OnRecordButton()
{
    recorder_->record();
}

void Camera::OnImageCaptured(int id, const QImage& preview)
{
//    QString savepath = QFileDialog::getSaveFileName(this, "Save Capture", "Capture", "Image png(*.png);;Image jpg(*.jpg);;Image bmp(*.bmp)");
//    if (!savepath.isEmpty())
//    {
//        preview.save(savepath);
//    }
}

void Camera::OnVideoFrame(const QVideoFrame& frame)
{
    QVideoFrame f = frame;
    if (!f.map(QAbstractVideoBuffer::ReadOnly))
    {
        return;
    }

//    qDebug() << "1";

//    一个QVideoFrame代表的就是相机的一帧数据。

//    QVideoFrame::bits() 返回的是一帧图像的起始地址。
//    在调用bits()函数之前还要先判断frame是否map了。所谓map就是将图像数据放到CPU可以寻址的地方。

//    cv::Mat nv21(frame.height() * 3 / 2, frame.width(), CV_8UC1, frame.bits(), static_cast<size_t>(frame.bytesPerLine()));

}

void Camera::DisplayCameraError(QCamera::Error error)
{

}
