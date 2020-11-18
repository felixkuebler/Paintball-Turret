import sys
import time
from arduinoSerialDevice import ArduinoSerialDevice

class Turret():

	arduinoBottom = 0
	arduinoTop = 0

	def __init__(self, arduinoBottomPort, arduinoTopPort):

		self.arduinoBottom = ArduinoSerialDevice(port=arduinoBottomPort, baudrate=9600)
		self.arduinoTop = ArduinoSerialDevice(port=arduinoTopPort, baudrate=9600)
		time.sleep(2)

		# attach motor controlls to pin 5 and 6
		self.arduinoBottom.motorAttach(5,6)
		self.arduinoBottom.motorSetGearRatio(10)
		self.arduinoBottom.motorInvertDir(1)
		self.arduinoBottom.motorSetRange(18000) # for bottom 270°
		self.arduinoBottom.motorSetMinSpeed(75)
		self.arduinoBottom.motorSetMaxSpeed(125)

		self.arduinoBottom.motorSetMotionMode(1) # for speed controlle


		# set io 4 to output
		self.arduinoTop.pinModeOutput(4)

		# attach motor controlls to pin 5 and 6
		self.arduinoTop.motorAttach(5,6)
		self.arduinoTop.motorSetGearRatio(5)
		self.arduinoTop.motorInvertDir(0)
		self.arduinoTop.motorSetRange(9000) # for top 90°
		self.arduinoTop.motorSetMinSpeed(100)
		self.arduinoTop.motorSetMaxSpeed(200)

		self.arduinoTop.motorSetMotionMode(1) # for speed controlle


	def setPositionX(self, pos):
		posScaled = int(100*pos)
		self.arduinoBottom.motorWritePosition(posScaled)
		print('Write Position X: %s ' % posScaled, file=sys.stdout, flush=True)


	def setPositionY(self, pos):
		posScaled = int(100*pos)
		self.arduinoTop.motorWritePosition(posScaled)
		print('Write Position Y: %s' % posScaled, file=sys.stdout, flush=True)


	def getPositionX(self):
		pos = self.arduinoBottom.motorRead()/100
		print('Read Position X: %s ' % pos, file=sys.stdout, flush=True)
		return pos


	def getPositionY(self):
		pos = self.arduinoTop.motorRead()/100
		print('Read Position Y: %s' % pos, file=sys.stdout, flush=True)
		return pos


	def resetPostionX(self):
		self.arduinoBottom.motorResetPosition()
		print('Reset Position X', file=sys.stdout, flush=True)


	def resetPostionY(self):
		self.arduinoTop.motorResetPosition()
		print('Reset Position Y', file=sys.stdout, flush=True)


	def isAtTargetX(self):
		isAtTarget = self.arduinoBottom.motorIsAtTarget() == True
		print('Is at target X: %s' % isAtTarget, file=sys.stdout, flush=True)
		return isAtTarget


	def isAtTargetY(self):
		isAtTarget = self.arduinoTop.motorIsAtTarget() == True
		print('Is at target Y: %s' % isAtTarget, file=sys.stdout, flush=True)
		return isAtTarget


	def setSpeedX(self, speed):
		speedScaled = int(speed)
		self.arduinoBottom.motorWriteSpeed(speedScaled)
		print('Write Speed X: %s ' % speedScaled, file=sys.stdout, flush=True)


	def setSpeedY(self, speed):
		speedScaled = int(speed)
		self.arduinoTop.motorWriteSpeed(speedScaled)
		print('Write Speed Y: %s' % speedScaled, file=sys.stdout, flush=True)


	def setMaxSpeedX(self, speed):
		speedScaled = int(speed)
		self.arduinoBottom.motorSetMaxSpeed(speedScaled)
		print('Set max Speed X: %s' % speedScaled, file=sys.stdout, flush=True)


	def setMaxSpeedY(self, speed):
		speedScaled = int(speed)
		self.arduinoTop.motorSetMaxSpeed(speedScaled)
		print('Set max Speed Y: %s' % speedScaled, file=sys.stdout, flush=True)


	def setMinSpeedX(self, speed):
		speedScaled = int(speed)
		self.arduinoBottom.motorSetMinSpeed(speedScaled)
		print('Set min Speed X: %s' % speedScaled, file=sys.stdout, flush=True)


	def setMinSpeedY(self, speed):
		speedScaled = int(speed)
		self.arduinoTop.motorSetMinSpeed(speedScaled)
		print('Set min Speed Y: %s' % speedScaled, file=sys.stdout, flush=True)


	def trigger(self):
		self.arduinoTop.digitalWrite(4,1)
		self.arduinoTop.digitalWrite(4,0)
		print('Trigger is pushed.', file=sys.stdout, flush=True)
