// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2017 Intel Corporation. All Rights Reserved.

/* Include the librealsense C header files */
#include <librealsense2/rs.hpp>
#include <librealsense2/h/rs_pipeline.h>
#include <librealsense2/h/rs_frame.h>
#include <linux/videodev2.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define ROUND_UP_2(num)  (((num)+1)&~1)
#define ROUND_UP_4(num)  (((num)+3)&~3)
#define ROUND_UP_8(num)  (((num)+7)&~7)
#define ROUND_UP_16(num) (((num)+15)&~15)
#define ROUND_UP_32(num) (((num)+31)&~31)
#define ROUND_UP_64(num) (((num)+63)&~63)

#define VIDEO_DEVICE "/dev/video8"

# define FRAME_WIDTH 640
# define FRAME_HEIGHT 480
# define STREAM_INDEX 0
# define FPS 30

# define FRAME_FORMAT V4L2_PIX_FMT_YUYV

static int debug=0;

int format_properties(const unsigned int format,
        const unsigned int width,
        const unsigned int height,
        size_t*linewidth, size_t*framewidth)
        {
    size_t lw, fw;
        lw = (ROUND_UP_2 (width) * 2);
        fw = lw * height;

      if(linewidth)*linewidth=lw;
      if(framewidth)*framewidth=fw;

      return 1;
}

void print_format(struct v4l2_format*vid_format) {
  printf("  vid_format->type                =%d\n", vid_format->type );
  printf("  vid_format->fmt.pix.width       =%d\n", vid_format->fmt.pix.width );
  printf("  vid_format->fmt.pix.height      =%d\n", vid_format->fmt.pix.height );
  printf("  vid_format->fmt.pix.pixelformat =%d\n", vid_format->fmt.pix.pixelformat);
  printf("  vid_format->fmt.pix.sizeimage   =%d\n", vid_format->fmt.pix.sizeimage );
  printf("  vid_format->fmt.pix.field       =%d\n", vid_format->fmt.pix.field );
  printf("  vid_format->fmt.pix.bytesperline=%d\n", vid_format->fmt.pix.bytesperline );
  printf("  vid_format->fmt.pix.colorspace  =%d\n", vid_format->fmt.pix.colorspace );
}

/* Function calls to librealsense may raise errors of type rs2_error */
rs2_error * e = 0;
void check_error()
{
    if(e)
    {
        printf("rs2_error was raised when calling %s(%s):\n", rs2_get_failed_function(e), rs2_get_failed_args(e));
        printf("    %s\n", rs2_get_error_message(e));
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char**argv)
{
    // This first section relates only to v4l2loopback, no RealSense code here.

    struct v4l2_capability vid_caps;
    struct v4l2_format vid_format;

    size_t framesize;
    size_t linewidth;

    __u8*buffer;

    const char*video_device=VIDEO_DEVICE;
    int fdwr = 0;
    int ret_code = 0;

    int i;

    if(argc>1) {
        video_device=argv[1];
        printf("using output device: %s\n", video_device);
    }

    fdwr = open(video_device, O_RDWR);
    assert(fdwr >= 0);

    ret_code = ioctl(fdwr, VIDIOC_QUERYCAP, &vid_caps);
    assert(ret_code != -1);

    memset(&vid_format, 0, sizeof(vid_format));

    ret_code = ioctl(fdwr, VIDIOC_G_FMT, &vid_format);
    if(debug)print_format(&vid_format);

    vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    vid_format.fmt.pix.width = FRAME_WIDTH;
    vid_format.fmt.pix.height = FRAME_HEIGHT;
    vid_format.fmt.pix.pixelformat = FRAME_FORMAT;
    vid_format.fmt.pix.sizeimage = framesize;
    vid_format.fmt.pix.field = V4L2_FIELD_NONE;
    vid_format.fmt.pix.bytesperline = linewidth;
    vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

    if(debug)print_format(&vid_format);
    ret_code = ioctl(fdwr, VIDIOC_S_FMT, &vid_format);

    assert(ret_code != -1);

    if(debug)printf("frame: format=%d\tsize=%lu\n", FRAME_FORMAT, framesize);
    print_format(&vid_format);

    if(!format_properties(vid_format.fmt.pix.pixelformat,
                                                vid_format.fmt.pix.width, vid_format.fmt.pix.height,
                                                &linewidth,
                                                &framesize)) {
        printf("unable to guess correct settings for format '%d'\n", FRAME_FORMAT);
    }
    buffer=(__u8*)malloc(sizeof(__u8)*framesize);

    memset(buffer, 0, framesize);

    write(fdwr, buffer, framesize);

    // Now the RealSense code.

    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_COLOR, FRAME_WIDTH, FRAME_HEIGHT, RS2_FORMAT_YUYV, FPS);

    rs2::pipeline pipe;
    pipe.start(cfg);

    printf("Copying frame data to loopback device.\nCTRL-C to quit.\n");
    for(;;)
    {
        rs2::frameset frames = pipe.wait_for_frames();
        rs2::frame frame = frames.first(RS2_STREAM_COLOR);
        if (frame)
            buffer = (__u8*)frame.get_data();
            write(fdwr, buffer, framesize);
    }

    return EXIT_SUCCESS;
}