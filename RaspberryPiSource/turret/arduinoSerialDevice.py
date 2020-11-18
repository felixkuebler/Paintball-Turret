import serial
import ctypes


JOB_SET_OUTPUT = ctypes.c_ubyte(1)
JOB_SET_INPUT = ctypes.c_ubyte(2)
JOB_DIGITAL_READ = ctypes.c_ubyte(3)
JOB_DIGITAL_WRITE = ctypes.c_ubyte(4)
JOB_ANALOG_READ = ctypes.c_ubyte(5)
JOB_ANALOG_WRITE = ctypes.c_ubyte(6)

JOB_ATTACH_MOTOR = ctypes.c_ubyte(10)
JOB_DETACH_MOTOR = ctypes.c_ubyte(1)
JOB_MOTOR_WRITE = ctypes.c_ubyte(12)
JOB_MOTOR_READ = ctypes.c_ubyte(13)
JOB_MOTOR_AT_TARGET = ctypes.c_ubyte(14)
JOB_MOTOR_OPTION = ctypes.c_ubyte(15)

OPTION_MOTOR_RESET = ctypes.c_ubyte(1)
OPTION_MOTOR_MIN_SPEED = ctypes.c_ubyte(2)
OPTION_MOTOR_MAX_SPEED = ctypes.c_ubyte(3)
OPTION_MOTOR_GEAR_RATIO  = ctypes.c_ubyte(4)
OPTION_MOTOR_INVERT_DIR  = ctypes.c_ubyte(5)
OPTION_MOTOR_MOTION_MODE = ctypes.c_ubyte(6)
OPTION_MOTOR_RANGE = ctypes.c_ubyte(7)




CMD_TERMINATOR = ('#').encode()


class ArduinoSerialDevice():

	arduinoDev = 0
	serialLock = False;


	def __init__(self, port, baudrate, timeout):
		self.arduinoDev = serial.Serial(port=port, baudrate=baudrate, timeout=timeout)
		self.serialLock = False;


	def __init__(self, port, baudrate):
		self.arduinoDev = serial.Serial(port=port, baudrate=baudrate)
		self.serialLock = False;


	def lockDevice(self):
		while self.serialLock:
			pass

		self.serialLock = True


	def unlockDevice(self):
		self.serialLock = False


	def pinModeInput(self, pin):
		self.lockDevice()
		self.arduinoDev.write(JOB_SET_INPUT)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def pinModeOutput(self, pin):
		self.lockDevice()
		self.arduinoDev.write(JOB_SET_OUTPUT)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def digitalWrite(self, pin, value):
		self.lockDevice()
		self.arduinoDev.write(JOB_DIGITAL_WRITE)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def digitalRead(self, pin):
		self.lockDevice()
		self.arduinoDev.write(JOB_DIGITAL_READ)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(CMD_TERMINATOR)

		byteStream = self.arduinoDev.read()
		self.unlockDevice()

		return ctypes.c_byte(int.from_bytes(byteStream, 'little')).value


	def analogWrite(self, pin, value):
		self.lockDevice()
		self.arduinoDev.write(JOB_ANALOG_WRITE)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def analogRead(self, pin):
		self.lockDevice()
		self.arduinoDev.write(JOB_ANALOG_READ)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(CMD_TERMINATOR)

		byteStream = self.arduinoDev.read(2)
		self.unlockDevice()

		return ctypes.c_int16(int.from_bytes(byteStream, 'little')).value


	def motorWrite(self, position):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_WRITE)
		self.arduinoDev.write(ctypes.c_int32(position))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def motorRead(self):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_READ)
		self.arduinoDev.write(CMD_TERMINATOR)

		byteStream = self.arduinoDev.read(4)
		self.unlockDevice()

		return ctypes.c_int32(int.from_bytes(byteStream, 'little')).value


	def motorIsAtTarget(self):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_AT_TARGET)
		self.arduinoDev.write(CMD_TERMINATOR)

		byteStream = self.arduinoDev.read()
		self.unlockDevice()

		return ctypes.c_byte(int.from_bytes(byteStream, 'little')).value


	def motorAttach(self, pin1, pin2):
		self.lockDevice()
		self.arduinoDev.write(JOB_ATTACH_MOTOR)
		self.arduinoDev.write(ctypes.c_ubyte(pin1))
		self.arduinoDev.write(ctypes.c_ubyte(pin2))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def motorDetach(self, pin1, pin2):
		self.lockDevice()
		self.arduinoDev.write(JOB_DETACH_MOTOR)
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def motorResetPosition(self):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_OPTION)
		self.arduinoDev.write(OPTION_MOTOR_RESET)
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def motorSetMaxSpeed(self, value):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_OPTION)
		self.arduinoDev.write(OPTION_MOTOR_MAX_SPEED)
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def motorSetMinSpeed(self, value):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_OPTION)
		self.arduinoDev.write(OPTION_MOTOR_MIN_SPEED)
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def motorSetGearRatio(self, value):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_OPTION)
		self.arduinoDev.write(OPTION_MOTOR_GEAR_RATIO)
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def motorInvertDir(self, value):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_OPTION)
		self.arduinoDev.write(OPTION_MOTOR_INVERT_DIR)
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def motorSetMotionMode(self, value):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_OPTION)
		self.arduinoDev.write(OPTION_MOTOR_MOTION_MODE)
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def motorSetRange(self, value):
		self.lockDevice()
		self.arduinoDev.write(JOB_MOTOR_OPTION)
		self.arduinoDev.write(OPTION_MOTOR_RANGE)
		self.arduinoDev.write(ctypes.c_uint16(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()

