#include "zf_device_uvc.h"

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

// OpenCV 摄像头对象
static VideoCapture cap;
static Mat frame_gray;

uint8_t *rgay_image;    // 灰度图像数组指针
static uint8_t gray_buffer[UVC_HEIGHT * UVC_WIDTH];  // 静态灰度缓冲区

int8 uvc_camera_init(const char *path)
{
    // 打开摄像头 (参考龙邱方案：简洁初始化)
    cap.open(0);  // /dev/video0

    if (!cap.isOpened())
    {
        printf("Failed to open camera\r\n");
        return -1;
    }

    printf("Camera opened successfully\r\n");

    // === 参考龙邱方案：只设置必要参数，移除额外开销 ===

    // 1. 设置 MJPEG 格式（支持高帧率的关键）
    cap.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));

    // 2. 设置分辨率
    cap.set(CAP_PROP_FRAME_WIDTH, UVC_WIDTH);
    cap.set(CAP_PROP_FRAME_HEIGHT, UVC_HEIGHT);

    // 3. 设置帧率（龙邱110fps摄像头）
    cap.set(CAP_PROP_FPS, UVC_FPS);

    // ⚠️ 移除以下设置（可能限制性能）：
    // - CAP_PROP_BUFFERSIZE：可能不被V4L2支持
    // - CAP_PROP_AUTO_EXPOSURE / CAP_PROP_EXPOSURE：增加处理开销

    // 读取实际配置
    int actual_width = cap.get(CAP_PROP_FRAME_WIDTH);
    int actual_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    double actual_fps = cap.get(CAP_PROP_FPS);

    printf("Camera config: %dx%d @ %.0f fps\r\n", actual_width, actual_height, actual_fps);

    if (actual_fps < UVC_FPS)
    {
        printf("⚠️  Warning: Requested %d fps, actual %.0f fps\r\n", UVC_FPS, actual_fps);
        printf("   提示: 请确认使用龙邱110fps摄像头并安装了正确的UVC驱动\r\n");
    }
    else
    {
        printf("✅ 摄像头初始化成功: 已达到目标帧率 %d fps\r\n", UVC_FPS);
    }

    return 0;
}


int8 wait_image_refresh()
{
    Mat frame;

    // 读取一帧图像
    cap.read(frame);

    if (frame.empty())
    {
        cerr << "Error: Failed to read frame from camera" << endl;
        return -1;
    }

    // 转换为灰度图
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

    // 拷贝到灰度缓冲区
    memcpy(gray_buffer, frame_gray.data, UVC_WIDTH * UVC_HEIGHT);
    rgay_image = gray_buffer;

    return 0;
}



