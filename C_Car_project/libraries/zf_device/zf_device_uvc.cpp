#include "zf_device_uvc.h"


#include <opencv2/imgproc/imgproc.hpp>  // for cv::cvtColor
#include <opencv2/highgui/highgui.hpp> // for cv::VideoCapture
#include <opencv2/opencv.hpp>

#include <iostream> // for std::cerr
#include <fstream>  // for std::ofstream
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include <atomic>

using namespace cv;

cv::Mat frame_rgb;      // 构建opencv对象 彩色
cv::Mat frame_rgay;     // 构建opencv对象 灰度

uint8_t *rgay_image;    // 灰度图像数组指针

VideoCapture cap;

int8 uvc_camera_init(const char *path)
{
    cap.open(path);

    if(!cap.isOpened())
    {
        printf("find uvc camera error.\r\n");
        return -1;
    }
    else
    {
        printf("find uvc camera Successfully.\r\n");
    }

    // 关键优化：设置摄像头输出格式为灰度（MJPEG格式）
    cap.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M','J','P','G'));  // 使用MJPEG压缩格式，减少数据传输量

    // 设置分辨率和帧率
    cap.set(CAP_PROP_FRAME_WIDTH, UVC_WIDTH);     // 设置摄像头宽度
    cap.set(CAP_PROP_FRAME_HEIGHT, UVC_HEIGHT);    // 设置摄像头高度
    cap.set(CAP_PROP_FPS, UVC_FPS);               // 设置帧率

    // 优化：减少缓冲区大小，降低延迟
    cap.set(CAP_PROP_BUFFERSIZE, 1);              // 最小缓冲区，减少延迟

    // 禁用自动曝光和白平衡以提高性能
    cap.set(CAP_PROP_AUTO_EXPOSURE, 0.25);        // 手动曝光模式
    cap.set(CAP_PROP_EXPOSURE, -5);               // 固定曝光值（根据环境调整）

    // 打印实际配置
    printf("Camera config: %dx%d @ %d fps\r\n",
           (int)cap.get(CAP_PROP_FRAME_WIDTH),
           (int)cap.get(CAP_PROP_FRAME_HEIGHT),
           (int)cap.get(CAP_PROP_FPS));

    return 0;
}


int8 wait_image_refresh()
{
    try
    {
        // 阻塞式等待图像刷新（使用grab+retrieve可能更快，但此处保持简单）
        cap >> frame_rgb;

        if (frame_rgb.empty())
        {
            std::cerr << "未获取到有效图像帧" << std::endl;
            return -1;
        }
    }
    catch (const cv::Exception& e)
    {
        std::cerr << "OpenCV 异常: " << e.what() << std::endl;
        return -1;
    }

    // 优化：使用更快的颜色转换（如果摄像头支持灰度输出可以跳过这一步）
    // 方案1: 标准转换（当前使用）
    cv::cvtColor(frame_rgb, frame_rgay, cv::COLOR_BGR2GRAY);

    // 方案2: 如果需要更快，可以使用近似算法（取绿色通道）
    // frame_rgay = frame_rgb.clone();  // 如果摄像头已输出灰度

    // cv对象转指针
    rgay_image = reinterpret_cast<uint8_t *>(frame_rgay.ptr(0));

    return 0;
}



