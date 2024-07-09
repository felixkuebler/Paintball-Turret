import os
from time import sleep

import cv2 
import numpy as np


class HikMicro:

	resolution = (192, 256) # the raw width is 344px but the usable width is only 192px
	temperatureMin = -20
	temperatureMax = 350


class ThermalCamera:

	def __init__(self, device=2, size=HikMicro.resolution):
		self.height, self.width = size
		self.filename = 'thermalFrame.raw'
		self.device = device


	def __del__(self):
		os.system('rm ' + self.filename)


	def read(self):
		try:
			# grap frame from camera
			os.system('v4l2-ctl -d /dev/video' + str(self.device) + ' --stream-mmap --stream-count=1 --stream-to=' + self.filename + ' >nul 2>&1')

			# load 16 bit raw image from file
			rawFrame = np.fromfile(self.filename, dtype=np.uint16, offset=0, count=self.width*self.height).reshape((self.height,self.width)) << 5
			cv2.imshow('rawFrame', rawFrame)

			# create 3 channel bgr image
			rgbFrame = cv2.merge([rawFrame, rawFrame, rawFrame]) 

			# normalize raw data
			rgbFrame = cv2.normalize(rgbFrame, None, alpha=0, beta=np.iinfo(np.uint16).max, norm_type=cv2.NORM_MINMAX)
			
			# convert to 8 bit image
			rgbFrame = (rgbFrame/256).astype('uint8')

			frame = cv2.resize(rgbFrame, (640, 480))

		except Exception as e:
			raise RuntimeError('Could not start camera.')
			return False, None

		return True, frame


	def getFrame(self):

		# read current frame
		ret, img = self.read()

		# encode as a jpeg image and return it
		return cv2.imencode('.jpg', img)[1].tobytes()
