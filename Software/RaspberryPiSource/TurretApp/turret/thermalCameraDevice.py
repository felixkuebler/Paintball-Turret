import os
from time import sleep

import cv2 
import numpy as np


class HikMicro:

	resolution = (344, 256)
	temperatureMin = -20
	temperatureMax = 350


class ThermalCamera:

	def __init__(self, device=2, size=HikMicro.resolution):
		self.height, self.width = size
		self.shape = (int(self.height*1.5), self.width)
		self.framelen = (int) (self.width * self.height * 2)
		self.filename = 'thermalFrame.raw'
		self.device = device


	def __del__(self):
		os.system('rm ' + self.filename)


	def get_frame(self):

		# read current frame
		ret, img = self.read()

		if not ret:
			raise RuntimeError('Could not start camera.')

		#img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
		img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

		img = cv2.bitwise_not(img)
		img = cv2.resize(img, (640, 480))

		# encode as a jpeg image and return it
		return cv2.imencode('.jpg', img)[1].tobytes()


	def read_raw(self):
		try:
			os.system('v4l2-ctl -d /dev/video' + str(self.device) + ' --stream-mmap --stream-count=1 --stream-to=' + self.filename + ' >nul 2>&1')

			file = open(self.filename, 'rb')
			raw = file.read(self.framelen)
			file.close()

			U  = raw[0::4]
			Y1 = raw[1::4]
			V  = raw[2::4]
			Y2 = raw[3::4]
			
			UV = np.empty((self.height*self.width), dtype=np.uint8)
			YY = np.empty((self.height*self.width), dtype=np.uint8)
			
			UV[0::2] = np.fromstring(U,  dtype=np.uint8)
			UV[1::2] = np.fromstring(V,  dtype=np.uint8)
			YY[0::2] = np.fromstring(Y1, dtype=np.uint8)
			YY[1::2] = np.fromstring(Y2, dtype=np.uint8)
			
			UV = UV.reshape((self.height, self.width))
			YY = YY.reshape((self.height, self.width))
			
			uyvy = cv2.merge([UV, YY])

			# crop unnecessary duplicates
			uyvy = uyvy[0:192, 0:256]

		except Exception as e:
			print(str(e))
			return False, None
		return True, uyvy


	def read(self):
		ret, uyvy = self.read_raw()
		if not ret:
			return ret, uyvy
		bgr = cv2.cvtColor(uyvy, cv2.COLOR_YUV2BGR_UYVY)
		return ret, bgr