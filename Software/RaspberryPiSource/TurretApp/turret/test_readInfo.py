import time
from arduinoSerialDevice import ArduinoSerialDevice

arduino = ArduinoSerialDevice(port='/dev/ttyUSB0', baudrate=9600)
time.sleep(2)

arduino.pinModeInput(10)
arduino.pinModeInput(14)
time.sleep(1)

while True:

	value = arduino.digitalRead(10);
	print('Digital Read = %s' % value)

	#value = arduino.analogRead(14)
	#print('Analog Read = %s' % value)

	#value = arduino.motorReadPosition()
	#print('Motor Position = %s' % value)