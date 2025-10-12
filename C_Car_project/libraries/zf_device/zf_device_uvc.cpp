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
static uint8_t gray_buffer[UVC_HEIGHT * UVC_WIDTH];  // 静态灰度缓冲区

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
        // 快速获取图像（grab+retrieve 模式）
        if (!cap.grab())
        {
            std::cerr << "未能抓取图像帧" << std::endl;
            return -1;
        }

        if (!cap.retrieve(frame_rgb))
        {
            std::cerr << "未能解码图像帧" << std::endl;
            return -1;
        }

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

    // 激进优化：手动快速 BGR 转灰度（避免 OpenCV cvtColor 的开销）
    // 使用整数运算的近似公式: Gray = (B + 2*G + R) / 4
    // 比标准公式 (0.114*B + 0.587*G + 0.299*R) 更快，精度损失可接受

    const uint8_t* bgr_data = frame_rgb.ptr<uint8_t>(0);
    const int total_pixels = UVC_WIDTH * UVC_HEIGHT;

    // 方案1: 最快速的近似算法（推荐用于实时性要求高的场景）
    for (int i = 0; i < total_pixels; i++)
    {
        int idx = i * 3;
        // Gray ≈ (B + 2*G + R) >> 2  （右移2位相当于除以4）
        gray_buffer[i] = (bgr_data[idx] + (bgr_data[idx + 1] << 1) + bgr_data[idx + 2]) >> 2;
    }

    // 方案2: 如果上面还不够快，可以只取绿色通道（最快但精度最低）
    // for (int i = 0; i < total_pixels; i++)
    // {
    //     gray_buffer[i] = bgr_data[i * 3 + 1];  // 只取 G 通道
    // }

    // 方案3: 标准 OpenCV 转换（最慢但最准确）- 已弃用
    // cv::cvtColor(frame_rgb, frame_rgay, cv::COLOR_BGR2GRAY);
    // rgay_image = reinterpret_cast<uint8_t *>(frame_rgay.ptr(0));

    // 指向自定义灰度缓冲区
    rgay_image = gray_buffer;

    return 0;
}



