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
		self.thermalColorMap = self.THERMAL_COLOR_MAP_DEFAULT
		self.cameraSelector = self.CAMERA_FEED_RGB


	def setCameraFeed(self, cameraFeed):
		self.cameraSelector = cameraFeed


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
			return self.thermalCam.getFrame()

		return self.rgbCam.getFrame()