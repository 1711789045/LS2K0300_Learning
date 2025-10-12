#include "zf_device_uvc.h"

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <cstring>
#include <cerrno>

using namespace std;

// V4L2 相关变量
static int camera_fd = -1;
static void* buffer_start = nullptr;
static size_t buffer_length = 0;

uint8_t *rgay_image;    // 灰度图像数组指针
static uint8_t gray_buffer[UVC_HEIGHT * UVC_WIDTH];  // 静态灰度缓冲区

int8 uvc_camera_init(const char *path)
{
    // 1. 打开摄像头设备（阻塞模式）
    camera_fd = open(path, O_RDWR);  // 移除 O_NONBLOCK，使用阻塞模式
    if (camera_fd < 0)
    {
        printf("Failed to open camera device: %s (errno: %d)\r\n", path, errno);
        return -1;
    }
    printf("Camera device opened: %s (fd=%d)\r\n", path, camera_fd);

    // 2. 查询设备能力
    struct v4l2_capability cap;
    if (ioctl(camera_fd, VIDIOC_QUERYCAP, &cap) < 0)
    {
        printf("Failed to query camera capabilities\r\n");
        close(camera_fd);
        return -1;
    }
    printf("Camera: %s\r\n", cap.card);

    // 3. 设置图像格式为 YUYV
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = UVC_WIDTH;
    fmt.fmt.pix.height = UVC_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;  // YUYV 格式（最快）
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(camera_fd, VIDIOC_S_FMT, &fmt) < 0)
    {
        printf("Failed to set video format\r\n");
        close(camera_fd);
        return -1;
    }

    printf("Format set: %dx%d YUYV\r\n", fmt.fmt.pix.width, fmt.fmt.pix.height);

    // 4. 设置帧率
    struct v4l2_streamparm parm;
    memset(&parm, 0, sizeof(parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.timeperframe.numerator = 1;
    parm.parm.capture.timeperframe.denominator = UVC_FPS;

    if (ioctl(camera_fd, VIDIOC_S_PARM, &parm) < 0)
    {
        printf("Warning: Failed to set frame rate\r\n");
    }
    else
    {
        // 读取实际设置的帧率
        ioctl(camera_fd, VIDIOC_G_PARM, &parm);
        uint32_t actual_fps = parm.parm.capture.timeperframe.denominator / parm.parm.capture.timeperframe.numerator;
        printf("Frame rate: requested=%d, actual=%u fps\r\n", UVC_FPS, actual_fps);

        if(actual_fps < UVC_FPS)
        {
            printf("⚠️  Warning: Camera does not support %d fps, running at %u fps\r\n", UVC_FPS, actual_fps);
        }
    }

    // 5. 请求缓冲区（使用 mmap 方式，最快）
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 1;  // 只使用1个缓冲区，最小延迟
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(camera_fd, VIDIOC_REQBUFS, &req) < 0)
    {
        printf("Failed to request buffers\r\n");
        close(camera_fd);
        return -1;
    }

    // 6. 映射缓冲区到用户空间
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = 0;

    if (ioctl(camera_fd, VIDIOC_QUERYBUF, &buf) < 0)
    {
        printf("Failed to query buffer\r\n");
        close(camera_fd);
        return -1;
    }

    buffer_length = buf.length;
    buffer_start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, camera_fd, buf.m.offset);

    if (buffer_start == MAP_FAILED)
    {
        printf("Failed to mmap buffer\r\n");
        close(camera_fd);
        return -1;
    }

    printf("Buffer mapped: %zu bytes\r\n", buffer_length);

    // 7. 将缓冲区入队
    if (ioctl(camera_fd, VIDIOC_QBUF, &buf) < 0)
    {
        printf("Failed to queue buffer\r\n");
        munmap(buffer_start, buffer_length);
        close(camera_fd);
        return -1;
    }

    // 8. 开始采集
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(camera_fd, VIDIOC_STREAMON, &type) < 0)
    {
        printf("Failed to start streaming\r\n");
        munmap(buffer_start, buffer_length);
        close(camera_fd);
        return -1;
    }

    printf("Camera streaming started successfully!\r\n");
    return 0;
}


int8 wait_image_refresh()
{
    // 1. 出队缓冲区（获取新图像） - 阻塞模式会自动等待
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(camera_fd, VIDIOC_DQBUF, &buf) < 0)
    {
        cerr << "Failed to dequeue buffer: errno=" << errno << endl;
        return -1;
    }

    // 2. 快速 YUYV 转灰度（只取 Y 分量，零开销！）
    // YUYV 格式: Y0 U0 Y1 V0 Y2 U1 Y3 V1 ...
    // Y 就是亮度（灰度），直接提取即可

    const uint8_t* yuyv_data = static_cast<uint8_t*>(buffer_start);
    const int total_pixels = UVC_WIDTH * UVC_HEIGHT;

    // 每2个字节提取1个Y值（YUYV格式）
    for (int i = 0; i < total_pixels; i++)
    {
        gray_buffer[i] = yuyv_data[i * 2];  // 只取 Y 分量
    }

    rgay_image = gray_buffer;

    // 3. 将缓冲区重新入队（准备下一帧）
    if (ioctl(camera_fd, VIDIOC_QBUF, &buf) < 0)
    {
        cerr << "Failed to requeue buffer" << endl;
        return -1;
    }

    return 0;
}



