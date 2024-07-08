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


	def read(self):
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

			frame = cv2.cvtColor(uyvy, cv2.COLOR_YUV2BGR_UYVY)

			#frame = cv2.rotate(frame, cv2.ROTATE_90_CLOCKWISE)

			frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

			frame = cv2.bitwise_not(frame)

			frame = cv2.resize(frame, (640, 480))

		except Exception as e:
			raise RuntimeError('Could not start camera.')
			return False, None

		return True, frame


	def getFrame(self):

		# read current frame
		ret, img = self.read()

		# encode as a jpeg image and return it
		return cv2.imencode('.jpg', img)[1].tobytes()
