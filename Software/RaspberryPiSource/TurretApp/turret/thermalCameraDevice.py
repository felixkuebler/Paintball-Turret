import os
import time
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

	temperatureBody = 37


class ThermalCamera:

	def __init__(self, device=2, size=HikMicro.resolution):
		self.height, self.width = size
		self.device = device
		
		self.numBuffers = 2
		
		self.faultDetected = False
		self.numConsecutiveFaults = 0
		self.lastReadTime = time.time()
		
		# initialize camera device
		self.initDevice()

		# initialize buffer
		self.initBuffers()

		# turn on video stream
		ioctl(self.fd, v4l2.VIDIOC_STREAMON, v4l2.v4l2_buf_type(v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE))
		
		# create a rawFrame member for video frames
		self.rawFrame = None


	def __del__(self):
		self.deinitDevice()


	def initDevice(self):
	
		self.fd = os.open('/dev/video' + str(self.device), os.O_RDWR, 0)
		
		cap = v4l2.v4l2_capability()
		fmt = v4l2.v4l2_format()

		ioctl(self.fd, v4l2.VIDIOC_QUERYCAP, cap)

		if not (cap.capabilities & v4l2.V4L2_CAP_VIDEO_CAPTURE):
			print("{} is not a video capture device".format(self.device), flush=True)
			self.faultDetected = True
			return False

		fmt.type = v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE
		ioctl(self.fd, v4l2.VIDIOC_G_FMT, fmt)
		
		return True
		
		
	def initBuffers(self):
		req = v4l2.v4l2_requestbuffers()

		req.count = self.numBuffers
		req.type = v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE
		req.memory = v4l2.V4L2_MEMORY_MMAP

		try:
			ioctl(self.fd, v4l2.VIDIOC_REQBUFS, req)
		except Exception as e:
			print("Video buffer request failed", flush=True)
			print(e, flush=True)
			self.faultDetected = True
			return False

		self.buffers = []
		for i in range(req.count):
			buf = v4l2.v4l2_buffer()
			buf.type = v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE
			buf.memory = v4l2.V4L2_MEMORY_MMAP
			buf.index = i
			buf.length = self.width*self.height
			
			ioctl(self.fd, v4l2.VIDIOC_QUERYBUF, buf)

			buf.buffer =  mmap.mmap(self.fd, buf.length, mmap.PROT_READ, mmap.MAP_SHARED, offset=buf.m.offset)
			self.buffers.append(buf)

			# queue all buffers
			ioctl(self.fd, v4l2.VIDIOC_QBUF, buf)
			
		# set reference to the current buffer
		self.currBuffer = 0
		
		return True
		
		
	def deinitDevice(self):
	
		try:
			ioctl(self.fd, v4l2.VIDIOC_STREAMOFF, v4l2.v4l2_buf_type(v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE))
		except Exception:
			print("Stop Stream Failed", flush=True)
		
		req = v4l2.v4l2_requestbuffers()

		req.count = 0
		req.type = v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE
		req.memory = v4l2.V4L2_MEMORY_MMAP

		try:
			ioctl(self.fd, v4l2.VIDIOC_REQBUFS, req)
		except Exception as e:
			print("Free buffers failed", flush=True)
			
		os.close(self.fd)
		self.fd = 0
		
		return True
	
	
	def faultRecovery(self):
		
		if self.faultDetected == True:			
			
			if not self.deinitDevice():
				return False
				
			if not self.initDevice():
				return False
				
			if not self.initBuffers():
				return False
				
			ioctl(self.fd, v4l2.VIDIOC_STREAMON, v4l2.v4l2_buf_type(v4l2.V4L2_BUF_TYPE_VIDEO_CAPTURE))
			
			self.rawFrame = None
			
			# reset the fault variables
			self.faultDetected = False
			self.numConsecutiveFaults = 0
			
		return True
			
		
	def readRaw(self):
		# check if last driver access is longer then 1 second	
		currentReadTime = time.time()
		if currentReadTime - self.lastReadTime > 1:
			self.faultDetected = True
				
		self.lastReadTime = currentReadTime
		
		# try to recover a driver fault
		if self.faultRecovery() == False:		
			
			self.numConsecutiveFaults += 1
			
			if (self.numConsecutiveFaults >= 10):
				self.rawFrame = None
				
			return (self.rawFrame is not None), self.rawFrame
			
			
		buf = self.buffers[self.currBuffer % self.numBuffers]
		self.currBuffer += 1
		
		ioctl(self.fd, v4l2.VIDIOC_DQBUF, buf)
			
		frameBuffer = self.buffers[buf.index].buffer
		frameData = frameBuffer.read(buf.bytesused)
		
		# handle corrupt buffer
		if len(frameData) == 0:
			self.faultDetected = True
		else:
			self.rawFrame = np.fromstring(frameData, dtype=np.uint16, count=self.width*self.height).reshape((self.height,self.width))
			frameBuffer.seek(0)
			ioctl(self.fd, v4l2.VIDIOC_QBUF, buf)
			
		return (self.rawFrame is not None), self.rawFrame
			

	def readNormalized(self):

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


	def readAbsolut(self):

		ret, rawFrame = self.readRaw()

		if not ret:
			return ret, None

		bodyTemp = HikMicro.temperatureBody
		bodyTemp = np.interp(bodyTemp, (HikMicro.temperatureMin, HikMicro.temperatureMax), (0, np.iinfo(np.uint16).max))
		
		rawFrame[:][:] = np.interp(rawFrame[:][:], (bodyTemp/2, bodyTemp*2), (0, np.iinfo(np.uint16).max))

		# create 3 channel bgr image
		rgbFrame = cv2.merge([rawFrame, rawFrame, rawFrame]) 

		# convert to 8 bit image
		rgbFrame = (rgbFrame/256).astype('uint8')

		rgbFrame = cv2.resize(rgbFrame, (854, 480))

		return ret, rgbFrame

		
	def read(self):
		return self.readAbsolut()


	def getFrame(self):

		# read current frame
		ret, img = self.read()

		if not ret:
			return ret, None

		# encode as a jpeg image and return it
		return ret, cv2.imencode('.jpg', img)[1].tobytes()