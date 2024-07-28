import serial
import ctypes


class ArduinoSerialDevice():

	CMD_SET_OUTPUT = ctypes.c_ubyte(1)
	CMD_SET_INPUT = ctypes.c_ubyte(2)
	CMD_DIGITAL_READ = ctypes.c_ubyte(3)
	CMD_DIGITAL_WRITE = ctypes.c_ubyte(4)
	CMD_ANALOG_READ = ctypes.c_ubyte(5)
	CMD_ANALOG_WRITE = ctypes.c_ubyte(6)

	CMD_MOTOR_PITCH_WRITE_POSITION_ABSOLUTE = ctypes.c_ubyte(10)
	CMD_MOTOR_PITCH_WRITE_POSITION_RELATIVE = ctypes.c_ubyte(11)
	CMD_MOTOR_PITCH_WRITE_SPEED = ctypes.c_ubyte(12)
	CMD_MOTOR_PITCH_READ_POSITION = ctypes.c_ubyte(13)

	CMD_MOTOR_YAW_WRITE_POSITION_ABSOLUTE = ctypes.c_ubyte(20)
	CMD_MOTOR_YAW_WRITE_POSITION_RELATIVE = ctypes.c_ubyte(21)
	CMD_MOTOR_YAW_WRITE_SPEED = ctypes.c_ubyte(22)
	CMD_MOTOR_YAW_READ_POSITION = ctypes.c_ubyte(23)

	CMD_MOTOR_AT_TARGET = ctypes.c_ubyte(15)

	CMD_MOTOR_OPTION = ctypes.c_ubyte(16)
	OPTION_MOTOR_RESET = ctypes.c_ubyte(1)

	CMD_TERMINATOR = ('#').encode()


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
		self.arduinoDev.write(CMD_SET_INPUT)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def pinModeOutput(self, pin):
		self.lockDevice()
		self.arduinoDev.write(CMD_SET_OUTPUT)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def digitalWrite(self, pin, value):
		self.lockDevice()
		self.arduinoDev.write(CMD_DIGITAL_WRITE)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def digitalRead(self, pin):
		self.lockDevice()
		self.arduinoDev.write(CMD_DIGITAL_READ)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(CMD_TERMINATOR)

		byteStream = self.arduinoDev.read()
		self.unlockDevice()

		return ctypes.c_byte(int.from_bytes(byteStream, 'little')).value


	def analogWrite(self, pin, value):
		self.lockDevice()
		self.arduinoDev.write(CMD_ANALOG_WRITE)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(ctypes.c_ubyte(value))
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()


	def analogRead(self, pin):
		self.lockDevice()
		self.arduinoDev.write(CMD_ANALOG_READ)
		self.arduinoDev.write(ctypes.c_ubyte(pin))
		self.arduinoDev.write(CMD_TERMINATOR)

		byteStream = self.arduinoDev.read(2)
		self.unlockDevice()

		return ctypes.c_int16(int.from_bytes(byteStream, 'little')).value


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


	def motorPitchReadPosition(self):
		self.lockDevice()
		self.arduinoDev.write(CMD_MOTOR_PITCH_READ_POSITION)
		self.arduinoDev.write(CMD_TERMINATOR)

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
		self.arduinoDev.write(CMD_MOTOR_YAW_READ_POSITION)
		self.arduinoDev.write(CMD_TERMINATOR)

		byteStream = self.arduinoDev.read(4)
		self.unlockDevice()

		return ctypes.c_int32(int.from_bytes(byteStream, 'little')).value


	def motorIsAtTarget(self):
		self.lockDevice()
		self.arduinoDev.write(CMD_MOTOR_AT_TARGET)
		self.arduinoDev.write(CMD_TERMINATOR)

		byteStream = self.arduinoDev.read()
		self.unlockDevice()

		return ctypes.c_byte(int.from_bytes(byteStream, 'little')).value


	def motorResetPosition(self):
		self.lockDevice()
		self.arduinoDev.write(CMD_MOTOR_OPTION)
		self.arduinoDev.write(OPTION_MOTOR_RESET)
		self.arduinoDev.write(CMD_TERMINATOR)
		self.unlockDevice()