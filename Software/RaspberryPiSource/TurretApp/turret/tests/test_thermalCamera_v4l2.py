#https://github.com/stonecontagion/v4l2-python-example/blob/master/cam.py

#sudo apt-get install libv4l-dev
#pip3 install v4l2-python3

import os
from time import sleep

import cv2 
import numpy as np

from fcntl import ioctl
import v4l2
import mmap
import struct


class HikMicro:
    resolution = (192, 256) # the raw width is 344px but the usable width is only 192px
    temperatureMin = -20
    temperatureMax = 350


class ThermalCamera():

    def __init__(self, device, size):
        self.height, self.width = size
        self.device = device

        self.numBuffers = 10

        # open camera device
        self.fd = os.open(self.device, os.O_RDWR, 0)
        
        # initialize camera device
        cap = v4l2.v4l2_capability()
        fmt = v4l2.v4l2_format()
        
        ioctl(self.fd, v4l2.VIDIOC_QUERYCAP, cap)
        
        if not (cap.capabilities & v4l2.V4L2_CAP_VIDEO_CAPTURE):
            raise Exception("{} is not a video capture device".format(self.device))
        
        fmt.type = v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE
        ioctl(self.fd, v4l2.VIDIOC_G_FMT, fmt)

        # initialize buffer
        req = v4l2.v4l2_requestbuffers()
        
        req.count = self.numBuffers
        req.type = v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE
        req.memory = v4l2.V4L2_MEMORY_MMAP
        
        try:
            ioctl(self.fd, v4l2.VIDIOC_REQBUFS, req)
        except Exception:
            raise Exception("video buffer request failed")
        
        if req.count < 2:
            raise Exception("Insufficient buffer memory on {}".format(self.device))

        self.buffers = []
        for i in range(req.count):
            buf = v4l2.v4l2_buffer()
            buf.type = v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE
            buf.memory = v4l2.V4L2_MEMORY_MMAP
            buf.index = i
            
            ioctl(self.fd, v4l2.VIDIOC_QUERYBUF, buf)

            buf.buffer =  mmap.mmap(self.fd, buf.length, mmap.PROT_READ, mmap.MAP_SHARED, offset=buf.m.offset)
            self.buffers.append(buf)


        for buf in self.buffers:
            ioctl(self.fd, v4l2.VIDIOC_QBUF, buf)

        videoType = v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE
        ioctl(self.fd, v4l2.VIDIOC_STREAMON, struct.pack('I', videoType))

        
    
    def readRaw(self):

        buf = self.buffers[0]
        ioctl(self.fd, v4l2.VIDIOC_DQBUF, buf)

        frameBuffer = self.buffers[buf.index].buffer
        frameData = frameBuffer.read(buf.bytesused)

        try:
            rawFrame = np.fromstring(frameData, dtype=np.uint16, count=256*192).reshape((192,256)) << 5
            frameBuffer.seek(0)
        except Exception as e:
            print (e)
            return False, None


        ioctl(self.fd, v4l2.VIDIOC_QBUF, buf)

        return True, rawFrame


    def read(self):
        ret, rawFrame = self.readRaw()

        if not ret:
            return ret, None

        # create 3 channel bgr image
        rgbFrame = cv2.merge([rawFrame, rawFrame, rawFrame]) 

        # normalize raw data
        rgbFrame = cv2.normalize(rgbFrame, None, alpha=0, beta=np.iinfo(np.uint16).max, norm_type=cv2.NORM_MINMAX)
        
        # convert to 8 bit image
        rgbFrame = (rgbFrame/256).astype('uint8')

        rgbFrame = cv2.resize(rgbFrame, (854, 480))

        return ret, rgbFrame
        

rgbCapture = cv2.VideoCapture(0)
thermalCapture = ThermalCamera("/dev/video2", HikMicro.resolution)


while (True):

    rgbRet, rgbFrame = rgbCapture.read()
    if rgbRet:
        cv2.imshow('RGB', rgbFrame)

    retThermal, thermalFrame = thermalCapture.read()
    if retThermal:

        #thermalFrame = cv2.rotate(thermalFrame, cv2.ROTATE_90_CLOCKWISE)
        thermalFrame = cv2.cvtColor(thermalFrame, cv2.COLOR_BGR2GRAY)

        thermalFrame = cv2.resize(thermalFrame, (640, 480))

        cv2.imshow('Grey', thermalFrame)

        thermalFrame = cv2.applyColorMap(thermalFrame, cv2.COLORMAP_JET)

        cv2.imshow('Thermal', thermalFrame)



    # the 'q' button is set as the 
    # quitting button you may use any 
    # desired button of your choice 
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

rgbCapture.release()
#thermalCapture.release()

cv2.destroyAllWindows()