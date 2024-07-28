import time
import sys

sys.path.append('..')

from arduinoSerialDevice import ArduinoSerialDevice

arduino = ArduinoSerialDevice(port='/dev/tty.usbserial-14240', baudrate=9600)

print('Enter Arduino ID:\n- 0 for Yaw\n- 1 for Pitch')
motorSelect = input()

print('Enter Motion Mode:\n- 0 for Angle Absolute \n - 1 for Angle Relative \n- 2 for Speed')
modeSelect = input()

if modeSelect == '0':
	print('Enter absolute Angle:\n(terminate with \"q\")\n-----------------------------')

elif modeSelect == '1':
	print('Enter relative Angle:\n(terminate with \"q\")\n-----------------------------')

else:
	print('Enter Speed:\n(terminate with \"q\")\n-----------------------------')

while True:

	value = input()

	if value == 'q':
		break

	if motorSelect == '0':

		if modeSelect == '0':
			arduino.motorYawWritePositionAbsolute(int(value))
		elif modeSelect == '1':
			arduino.motorYawWritePositionRelative(int(value))
		else:
			arduino.motorYawWriteSpeed(int(value))

	else:
		if modeSelect == '0':
			arduino.motorPitchWritePositionAbsolute(int(value))
		elif modeSelect == '1':
			arduino.motorPitchWritePositionRelative(int(value))
		else:
			arduino.motorPitchWriteSpeed(int(value))
