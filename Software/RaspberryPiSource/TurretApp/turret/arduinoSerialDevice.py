import serial
import ctypes
import time

class ArduinoSerialDevice():

	CMD_TRIGGER_PRIMARY = ctypes.c_ubyte(1)
	CMD_TRIGGER_SECONDARY = ctypes.c_ubyte(2)

	CMD_MOTOR_PITCH_WRITE_POSITION_ABSOLUTE = ctypes.c_ubyte(10)
	CMD_MOTOR_PITCH_WRITE_POSITION_RELATIVE = ctypes.c_ubyte(11)
	CMD_MOTOR_PITCH_WRITE_SPEED = ctypes.c_ubyte(12)
	CMD_MOTOR_PITCH_READ_POSITION = ctypes.c_ubyte(13)

	CMD_MOTOR_YAW_WRITE_POSITION_ABSOLUTE = ctypes.c_ubyte(20)
	CMD_MOTOR_YAW_WRITE_POSITION_RELATIVE = ctypes.c_ubyte(21)
	CMD_MOTOR_YAW_WRITE_SPEED = ctypes.c_ubyte(22)
	CMD_MOTOR_YAW_READ_POSITION = ctypes.c_ubyte(23)
	
	CMD_MOTOR_WRITE_POSITION_ABSOLUTE = ctypes.c_ubyte(30)
	CMD_MOTOR_WRITE_POSITION_RELATIVE = ctypes.c_ubyte(31)
	CMD_MOTOR_WRITE_SPEED = ctypes.c_ubyte(32)
	CMD_MOTOR_READ_POSITION = ctypes.c_ubyte(33)

	CMD_TERMINATOR = ('#').encode()


	arduinoDev = 0
	serialLock = False;


	def __init__(self, port='/dev/ttyUSB0', baudrate=115200):
		self.arduinoDev = serial.Serial(port=port, baudrate=baudrate)
		self.serialLock = False;

		# delay untlins serial connection has been established
		time.sleep(2)


	def lockDevice(self):
		while self.serialLock:
			pass

		self.serialLock = True


	def unlockDevice(self):
		self.serialLock = False


	def triggerPrimary(self, value):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_TRIGGER_PRIMARY)
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(self.CMD_TERMINATOR)
		self.unlockDevice()


	def motorPitchWritePositionAbsolute(self, position):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_MOTOR_PITCH_WRITE_POSITION_ABSOLUTE)
		self.arduinoDev.write(ctypes.c_int32(position))
		self.arduinoDev.write(self.CMD_TERMINATOR)
		self.unlockDevice()


	def motorPitchWritePositionRelative(self, position):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_MOTOR_PITCH_WRITE_POSITION_RELATIVE)
		self.arduinoDev.write(ctypes.c_int32(position))
		self.arduinoDev.write(self.CMD_TERMINATOR)
		self.unlockDevice()


	def motorPitchWriteSpeed(self, speed):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_MOTOR_PITCH_WRITE_SPEED)
		self.arduinoDev.write(ctypes.c_int16(speed))
		self.arduinoDev.write(self.CMD_TERMINATOR)
		self.unlockDevice()


	def motorWriteSpeed(self, speedPitch, speedYaw):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_MOTOR_WRITE_SPEED)
		self.arduinoDev.write(ctypes.c_int16(speedPitch))
		self.arduinoDev.write(ctypes.c_int16(speedYaw))
		self.arduinoDev.write(self.CMD_TERMINATOR)
		self.unlockDevice()


	def motorPitchReadPosition(self):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_MOTOR_PITCH_READ_POSITION)
		self.arduinoDev.write(self.CMD_TERMINATOR)

		byteStream = self.arduinoDev.read(4)
		self.unlockDevice()

		return ctypes.c_int32(int.from_bytes(byteStream, 'little')).value


	def motorYawWritePositionAbsolute(self, position):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_MOTOR_YAW_WRITE_POSITION_ABSOLUTE)
		self.arduinoDev.write(ctypes.c_int32(position))
		self.arduinoDev.write(self.CMD_TERMINATOR)
		self.unlockDevice()


	def motorYawWritePositionRelative(self, position):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_MOTOR_YAW_WRITE_POSITION_RELATIVE)
		self.arduinoDev.write(ctypes.c_int32(position))
		self.arduinoDev.write(self.CMD_TERMINATOR)
		self.unlockDevice()


	def motorYawWriteSpeed(self, speed):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_MOTOR_YAW_WRITE_SPEED)
		self.arduinoDev.write(ctypes.c_int16(speed))
		self.arduinoDev.write(self.CMD_TERMINATOR)
		self.unlockDevice()


	def motorYawReadPosition(self):
		self.lockDevice()
		self.arduinoDev.write(self.CMD_MOTOR_YAW_READ_POSITION)
		self.arduinoDev.write(self.CMD_TERMINATOR)

		byteStream = self.arduinoDev.read(4)
		self.unlockDevice()

		return ctypes.c_int32(int.from_bytes(byteStream, 'little')).value
