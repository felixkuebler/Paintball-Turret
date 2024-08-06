import cv2

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
			
			# create mask from thermal image
			# TODO make threshold adjustable 
			_, mask = cv2.threshold(frameThermal, 170 ,255, cv2.THRESH_BINARY)
			
			if self.thermalColorMap != self.THERMAL_COLOR_MAP_DEFAULT:
				frameThermal = cv2.applyColorMap(frameThermal, self.thermalColorMap)
			
			#masked = cv2.bitwise_and(frameThermal, frameThermal, mask=mask)
			
			alpha = 0.5
			dst = cv2.addWeighted(frameThermal, alpha , frameRgb, 1-alpha, 0)
			
			return (retThermal and retRgb), cv2.imencode('.jpg', dst)[1].tobytes()

		else:
			
			retRgb, frameRgb = self.rgbCam.read(self.zoomEnabled)
			
			return retRgb, cv2.imencode('.jpg', frameRgb)[1].tobytes()

				
