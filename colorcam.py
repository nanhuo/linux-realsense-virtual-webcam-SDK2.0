
#coding:utf-8

import pyrealsense2 as rs
import fcntl, sys, os
from v4l2 import *
import time
import numpy as np
import cv2
import sys

def start_camera():
	devName = '/dev/video80'
	if len(sys.argv) >= 2:
		devName = sys.argv[1]
	width = 640
	height = 480
	if not os.path.exists(devName):
		print("Warning: device does not exist",devName)
	device = open(devName, 'wb+', buffering=0)

	print(device)
	capability = v4l2_capability()
	print("get capabilities result", (fcntl.ioctl(device, VIDIOC_QUERYCAP, capability)))
	print("capabilities", hex(capability.capabilities))

	fmt = V4L2_PIX_FMT_YUYV

	print("v4l2 driver: ", capability.driver)
	format = v4l2_format()
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT
	format.fmt.pix.pixelformat = fmt
	format.fmt.pix.width = width
	format.fmt.pix.height = height
	format.fmt.pix.field = V4L2_FIELD_NONE
	format.fmt.pix.bytesperline = width * 2
	format.fmt.pix.sizeimage = width * height * 2
	format.fmt.pix.colorspace = V4L2_COLORSPACE_JPEG

	print("set format result", (fcntl.ioctl(device, VIDIOC_S_FMT, format)))
	#Note that format.fmt.pix.sizeimage and format.fmt.pix.bytesperline 
	#may have changed at this point

	pipeline = rs.pipeline()
	config = rs.config()
	config.enable_stream(rs.stream.depth, 640, 480, rs.format.z16, 30)
	# config.enable_stream(rs.stream.color, 640, 480, rs.format.yuyv, 30)
	config.enable_stream(rs.stream.color, 640, 480, rs.format.rgb8, 30)
	
	pipeline.start(config)

	while True:
		frames = pipeline.wait_for_frames()
		depth_frame = frames.get_depth_frame()
		color_frame = frames.get_color_frame()
		color_data = color_frame.get_data()

		color_nparray = np.asanyarray(color_data)
		depth_nparray= np.asanyarray(depth_frame.get_data())

        # convert rgb to yuyv
		yuv = cv2.cvtColor(color_nparray, cv2.COLOR_RGB2YUV)
		buf = np.zeros((format.fmt.pix.height, 2*format.fmt.pix.width), dtype=np.uint8)
		for i in range(format.fmt.pix.height):
			buf[i,::2] = yuv[i,:,0]
			buf[i,1::4] = yuv[i,::2,1]
			buf[i,3::4] = yuv[i,::2,2] 

		device.write(buf.tostring())
        

if __name__=="__main__":
	start_camera()

