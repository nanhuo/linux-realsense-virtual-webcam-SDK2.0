# linux-realsense-virtual-webcam-SDK2.0

* Experimental virtual webcam for the Intel Realsense SR300 using Intel® RealSense™ SDK 2.0.
* Both C++ and Python3 are supported.


## Requirements

### Hardware
* Intel RealSense camera https://software.intel.com/en-us/realsense/

### Software
 * v4l2loopback https://github.com/umlaeute/v4l2loopback
 * v4l2(For python) https://pypi.python.org/pypi/v4l2
 * librealsense https://github.com/IntelRealSense/librealsense

## Getting started

### C++

```
sh build.sh
./build/colorcam
```

### Python

```
python colorcam.py
```


## Reference

 - [linux-realsense-virtual-webcam](https://github.com/msaunby/linux-realsense-virtual-webcam) - linux-realsense-virtual-webcam for sdk 1.*
 - [v4l2](https://pypi.python.org/pypi/v4l2) - v4l2 wrapper for python