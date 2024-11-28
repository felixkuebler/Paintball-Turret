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
				
		retMux = False
		frameMux = None		
		
		# grep rgb frame
		retRgb, frameRgb = self.rgbCam.read()
		
		if not retRgb:
			return False, None

		# TODO configurable offsets
		offsetRgbX = 40
		offsetRgbY = 0
	   
		# shift the frame by aim offset
		frameRgb = self.shiftFrame(frameRgb, offsetRgbX, offsetRgbY)

		if self.cameraSelector != self.CAMERA_FEED_RGB:

			# select type of thermal frame
			if self.cameraSelector == self.CAMERA_FEED_THERMAL_NORM:
				retThermal, frameThermal = self.thermalCam.readNormalized()
			elif self.cameraSelector == self.CAMERA_FEED_THERMAL_ABS:
				retThermal, frameThermal = self.thermalCam.readAbsolut()
			else:
				retThermal, frameThermal = self.thermalCam.readAbsolut()

			if not retThermal:
				return False, None

			# TODO configurable scale
			# create an thermal image overlay
			scaleThermal = 1.9
			newSizeThermal = (int(frameThermal.shape[1]*scaleThermal), int(frameThermal.shape[0]*scaleThermal))
			frameThermal = cv2.resize(frameThermal, newSizeThermal)

			frameThermalOverlay = np.zeros((frameRgb.shape[0],frameRgb.shape[1],3), dtype=np.uint8)
		
			# place thermal frame at the center of the adapted rgb frame
			offsetThermalX = int(frameThermalOverlay.shape[1]/2-frameThermal.shape[1]/2)
			offsetThermalY = int(frameThermalOverlay.shape[0]/2-frameThermal.shape[0]/2)
			frameThermalOverlay[offsetThermalY:offsetThermalY+frameThermal.shape[0], offsetThermalX:offsetThermalX+frameThermal.shape[1]] = frameThermal
				
			# simple overlay for normaized an absolute
			if self.cameraSelector == self.CAMERA_FEED_THERMAL_NORM or self.cameraSelector == self.CAMERA_FEED_THERMAL_ABS:
			
				# apply color map
				if self.thermalColorMap != self.THERMAL_COLOR_MAP_DEFAULT:
					frameThermalOverlay = cv2.applyColorMap(frameThermalOverlay, self.thermalColorMap)
				
				# TODO configurable alpha
				# blend rgb and thermal
				alpha = 0.9
				blend = cv2.addWeighted(frameThermalOverlay, alpha , frameRgb, 1-alpha, 0)

				# place thermal frame at the center of the adapted rgb frame
				offsetThermalX = int(frameRgb.shape[1]/2-frameThermal.shape[1]/2)
				offsetThermalY = int(frameRgb.shape[0]/2-frameThermal.shape[0]/2)
				frameRgb[offsetThermalY:offsetThermalY+frameThermal.shape[0], offsetThermalX:offsetThermalX+frameThermal.shape[1]] = blend[offsetThermalY:offsetThermalY+frameThermal.shape[0], offsetThermalX:offsetThermalX+frameThermal.shape[1]]
				
				# set result values
				retMux = True
				frameMux = frameRgb
				
			elif self.cameraSelector == self.CAMERA_FEED_HYBRID:
				
				# create mask to hide all low temp values
				_, mask = cv2.threshold(frameThermalOverlay, 80 ,255, cv2.THRESH_TOZERO)
				mask = cv2.split(mask)[0]

				# apply color map
				if self.thermalColorMap != self.THERMAL_COLOR_MAP_DEFAULT:
					frameThermalOverlay = cv2.applyColorMap(frameThermalOverlay, self.thermalColorMap)
				
				# apply mask
				maskedFrameThermal = cv2.bitwise_and(frameThermalOverlay, frameThermalOverlay, mask=mask.astype('uint8'))
				
				# TODO configurable alpha
				# blend rgb and thermal
				alpha = 0.5
				blend = cv2.addWeighted(maskedFrameThermal, 1 , frameRgb, 1, 0)
				
				# set result values
				retMux = True
				frameMux = blend
			
		else:
			
			# set result values
			retMux = retRgb
			frameMux = frameRgb

		#check if a frame was generated
		if retMux == False:
			return retMux, None

		# TODO configurable aim offsets
		offsetMuxX = -40
		offsetMuxY = 120
		
		# shift the frame by aim offset
		frameMux = self.shiftFrame(frameMux, offsetMuxX, offsetMuxY)
		
		# apply zoom to all camera modes
		if self.zoomEnabled:
			frameMux = frameMux[int(frameMux.shape[0]/3):int(frameMux.shape[0]*2/3), int(frameMux.shape[1]/3):int(frameMux.shape[1]*2/3)]
			frameMux = cv2.resize(frameMux, (frameMux.shape[1], frameMux.shape[0]))

		frameMux = cv2.resize(frameMux, (854, 480))
		
		return retMux, cv2.imencode('.jpg', frameMux)[1].tobytes()


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
