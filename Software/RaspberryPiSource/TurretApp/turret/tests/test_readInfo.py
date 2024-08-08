import time
import sys

sys.path.append('..')

from arduinoSerialDevice import ArduinoSerialDevice

serial = ArduinoSerialDevice(port='/dev/tty.usbserial-14240', baudrate=9600)

while True:

	#value = serial.digitalRead(10);
	#print('Digital Read = %s' % value)

	#value = serial.analogRead(14)
	#print('Analog Read = %s' % value)

	value = serial.motorYawReadPosition()
	print('Motor Yaw Position = %s' % value)