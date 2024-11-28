import cv2
import numpy as np

from rgbCameraDevice import RgbCamera
from thermalCameraDevice import ThermalCamera

class CameraMux():

	CAMERA_FEED_RGB = 0
	CAMERA_FEED_THERMAL_ABS = 1
	CAMERA_FEED_THERMAL_NORM = 2
	CAMERA_FEED_HYBRID = 3

	THERMAL_COLOR_MAP_DEFAULT = 255

	def __init__(self, deviceRgb=0, deviceThermal=2):
		self.rgbCam = RgbCamera(deviceRgb)
		self.thermalCam = ThermalCamera(deviceThermal)
		self.zoomEnabled = False
		self.thermalColorMap = self.THERMAL_COLOR_MAP_DEFAULT
		self.cameraSelector = self.CAMERA_FEED_RGB
	
	
	def setCameraZoom(self, zoomEnabled):
		self.zoomEnabled = zoomEnabled
		
		
	def setCameraFeed(self, cameraSelector):
		self.cameraSelector = cameraSelector


	def setThermalColorMap(self, thermalColorMap):
		self.thermalColorMap = thermalColorMap


	def getFrame(self):
			
		if self.cameraSelector == self.CAMERA_FEED_THERMAL_NORM or self.cameraSelector == self.CAMERA_FEED_THERMAL_ABS:

			if self.cameraSelector == self.CAMERA_FEED_THERMAL_NORM:
				ret, frame = self.thermalCam.readNormalized()
			else:
				ret, frame = self.thermalCam.readAbsolut()

			if not ret:
				return False, None

			if self.thermalColorMap != self.THERMAL_COLOR_MAP_DEFAULT:
				frame = cv2.applyColorMap(frame, self.thermalColorMap)

			# encode as a jpeg image and return it
			return True, cv2.imencode('.jpg', frame)[1].tobytes()

		elif self.cameraSelector == self.CAMERA_FEED_HYBRID:
			retThermal, frameThermal = self.thermalCam.readAbsolut()
			retRgb, frameRgb = self.rgbCam.read()
			
			if not (retThermal and retRgb):
				return False, None
				
			# TODO configurable offsets
			xOffset = 10
			yOffset = -30
			height, width, channels = frameRgb.shape
			frameRgb = frameRgb[int(height/3)+yOffset:int(height*2/3)+yOffset, int(width/3)+xOffset:int(width*2/3)+xOffset]
			frameRgb = cv2.resize(frameRgb, (854, 480))
			
			# create mask from thermal image
			# TODO make threshold adjustable 
			_, mask = cv2.threshold(frameThermal, 80 ,255, cv2.THRESH_TOZERO)
			mask = cv2.split(mask)[0]

			if self.thermalColorMap != self.THERMAL_COLOR_MAP_DEFAULT:
				frameThermal = cv2.applyColorMap(frameThermal, self.thermalColorMap)
			
			maskedFrameThermal = cv2.bitwise_and(frameThermal, frameThermal, mask=mask.astype('uint8'))
			
			alpha = 0.5
			dst = cv2.addWeighted(maskedFrameThermal, 1 , frameRgb, 1, 0)
			
			return (retThermal and retRgb), cv2.imencode('.jpg', dst)[1].tobytes()

		else:
			
			retRgb, frameRgb = self.rgbCam.read(self.zoomEnabled)
			
			return retRgb, cv2.imencode('.jpg', frameRgb)[1].tobytes()
		# TODO configurable aim offsets
		offsetMuxX = -40
		offsetMuxY = 120
		
		# shift the frame by aim offset
		frameMux = self.shiftFrame(frameMux, offsetMuxX, offsetMuxY)

	def shiftFrame(self, frame, offsetX, offsetY):
		
		width  = frame.shape[1]
		height = frame.shape[0]
		
		# calculate the scales required for the additional offset
		scaleX = (width+abs(offsetX))/width
		scaleY = (height+abs(offsetY))/height
		
		# select the greater scale factor
		scale = scaleX if scaleX > scaleY else scaleY
		offsetX = offsetX if scaleX > scaleY else (offsetX + ((scale*width)-width)/2)
		offsetY = offsetY if scaleY > scaleX else (offsetY + ((scale*height)-height)/2)
				
		# shift image to the offset
		frame = cv2.resize(frame, (int(width*scale), int(height*scale)))
		frame = frame[int(offsetY if offsetY > 0 else 0):int(height+offsetY if offsetY > 0 else height), int(offsetX if offsetX > 0 else 0):int(width+offsetX if offsetX > 0 else width)]
		
		return frame
