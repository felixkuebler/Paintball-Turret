import cv2

from rgbCameraDevice import RgbCamera
from thermalCameraDevice import ThermalCamera

class CameraMux():

	CAMERA_FEED_RGB = 0
	CAMERA_FEED_THERMAL = 1
	CAMERA_FEED_HYBRID = 2

	def __init__(self, deviceRgb=0, deviceThermal=2):
		self.rgbCam = RgbCamera(deviceRgb)
		self.thermalCam = ThermalCamera(deviceThermal)
		self.thermalColorMap = cv2.COLORMAP_BONE
		self.cameraSelector = self.CAMERA_FEED_RGB


	def __del__(self):
		self.rgbCam.release()
		self.thermalCam.release()


	def setCameraFeed(self, cameraFeed):
		self.cameraSelector = cameraFeed


	def setThermalColorMap(self, thermalColorMap):
		self.thermalColorMap = thermalColorMap


	def getFrame(self):
			
		if self.cameraSelector == self.CAMERA_FEED_THERMAL:
			ret, frame = self.thermalCam.read()
			frame = cv2.applyColorMap(frame, self.thermalColorMap)

			# encode as a jpeg image and return it
			return cv2.imencode('.jpg', frame)[1].tobytes()

		elif self.cameraSelector == self.CAMERA_FEED_HYBRID:
			return self.thermalCam.getFrame()

		return self.rgbCam.getFrame()