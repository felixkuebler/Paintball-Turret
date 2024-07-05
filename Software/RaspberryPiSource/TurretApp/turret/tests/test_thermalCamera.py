# import the opencv library 

import os
from time import sleep

import cv2 
import numpy as np


class VideoCaptureUYCY:
	def __init__(self, device, size):
		self.height, self.width = size
		self.shape = (int(self.height*1.5), self.width)
		self.framelen = (int) (self.width * self.height * 2)
		self.filename = "thermalFrame.raw"
		self.device = device

	def read_raw(self):
		try:
			os.system("v4l2-ctl -d " + self.device + " --stream-mmap --stream-count=1 --stream-to=" + self.filename + " >nul 2>&1")

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




# define a video capture object 
rgbCapture = cv2.VideoCapture(0)


class HikMicro:
	resolution = (344, 256)
	temperatureMin = -20
	temperatureMax = 350

thermalCapture = VideoCaptureUYCY("/dev/video2", HikMicro.resolution)




while (True):

	rgbRet, rgbFrame = rgbCapture.read()
	if rgbRet:
		cv2.imshow('RGB', rgbFrame)

	retThermal, thermalFrame = thermalCapture.read()
	if retThermal:

		#thermalFrame = cv2.rotate(thermalFrame, cv2.ROTATE_90_CLOCKWISE)
		thermalFrame = cv2.cvtColor(thermalFrame, cv2.COLOR_BGR2GRAY)

		thermalFrame = cv2.bitwise_not(thermalFrame)
		thermalFrame = cv2.resize(thermalFrame, (640, 480))


		contrast = 1
		brightness = -30
		thermalFrame = cv2.addWeighted(thermalFrame, contrast, np.zeros(thermalFrame.shape, thermalFrame.dtype), 0, brightness) 


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