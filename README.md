# linux-realsense-virtual-webcam-SDK2.0
# Experimental virtual webcam for the Intel Realsense SR300 using Intel® RealSense™ SDK 2.0


For Intel® RealSense™ SDK 1.\*, please refer to https://github.com/msaunby/linux-realsense-virtual-webcam .

## To use this you'll need

### Hardware
* Intel RealSense camera https://software.intel.com/en-us/realsense/

I'm presently using the Intel SR300 camera on a Dell XPS13 running Ubuntu 16.04

### Software
 * v4l2loopback https://github.com/umlaeute/v4l2loopback
 * librealsense https://github.com/IntelRealSense/librealsense

## Getting started

If you haven't already, build and install the librealsense examples and check
that they work correctly with your hardware.   If they don't then fix that
first, before trying this.

## colorcam

A simple first test to create a virtual video device that copies the
RealSense colour camera.
