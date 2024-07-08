# import the opencv library 

import os
from time import sleep

import cv2 
import numpy as np


class VideoCaptureUYCY:
	def __init__(self, device, size):
		self.height, self.width = size
		self.filename = "thermalFrame.raw"
		self.device = device

	def read(self):
		try:
			# grap frame from camera
			os.system("v4l2-ctl -d " + self.device + " --stream-mmap --stream-count=1 --stream-to=" + self.filename + " >nul 2>&1")

			# load 16 bit raw image from file
			rawFrame = np.fromfile(self.filename, dtype=np.uint16, offset=0, count=self.width*self.height).reshape((self.height,self.width)) << 5
			cv2.imshow('rawFrame', rawFrame)

			# create 3 channel bgr image
			rgbFrame = cv2.merge([rawFrame, rawFrame, rawFrame]) 

			# convert to 8 bit image
			rgbFrame = (rgbFrame/256).astype('uint8')

		except Exception as e:
			print(str(e))
			return False, None
		return True, rgbFrame



# define a video capture object 
rgbCapture = cv2.VideoCapture(0)


class HikMicro:
	resolution = (192, 256) # the raw width is 344px but the usable width is only 192px
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