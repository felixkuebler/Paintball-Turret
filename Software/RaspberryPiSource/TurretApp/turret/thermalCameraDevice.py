import os
from time import sleep

import cv2 
import numpy as np


class HikMicro:

	resolution = (192, 256) # the raw width is 344px but the usable width is only 192px
	temperatureMin = -20
	temperatureMax = 350

	temperatureBody = 37


class ThermalCamera:

	def __init__(self, device=2, size=HikMicro.resolution):
		self.height, self.width = size
		self.filename = 'thermalFrame.raw'
		self.device = device


	def __del__(self):
		os.system('rm ' + self.filename)


	def readRaw(self):
		# grap frame from camera
		os.system('v4l2-ctl -d /dev/video' + str(self.device) + ' --stream-mmap --stream-count=1 --stream-to=' + self.filename + ' >nul 2>&1')

		# check if a file was generated
		if os.path.isfile(self.filename):

			# load 16 bit raw image from file
			rawFrame = np.fromfile(self.filename, dtype=np.uint16, count=self.width*self.height).reshape((self.height,self.width)) << 4

		else:
			#raise RuntimeError('Could not start camera.')
			return False, None

		return True, rawFrame


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

		rgbFrame = cv2.resize(rgbFrame, (640, 480))

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

		rgbFrame = cv2.resize(rgbFrame, (640, 480))

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