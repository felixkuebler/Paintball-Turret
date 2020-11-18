import time
from arduinoSerialDevice import ArduinoSerialDevice

print('Enter Arduino ID:\n- 0 for Bottom\n- 1 for Top')

arduinoSelect = input() == '0'
arduino = 0

if arduinoSelect:
	arduino = ArduinoSerialDevice(port='/dev/ttyUSB0', baudrate=9600)
	time.sleep(2)

	arduino.motorAttach(5,6)
	arduino.motorSetGearRatio(10) # for bottom
	arduino.motorInvertDir(1) # for bottom
	arduino.motorSetRange(18000) # for bottom 180°
	arduino.motorSetMinSpeed(75)
	arduino.motorSetMaxSpeed(125)

else:
	arduino = ArduinoSerialDevice(port='/dev/ttyUSB1', baudrate=9600)
	time.sleep(2)

	arduino.motorAttach(5,6)
	arduino.motorSetGearRatio(5) # for top
	arduino.motorInvertDir(0) # for top
	arduino.motorSetRange(9000) # for top 90°
	arduino.motorSetMinSpeed(100)
	arduino.motorSetMaxSpeed(200)


print('Enter Motion Mode:\n- 0 for Angle\n- 1 for Speed')
modeSelect = input() == '0'

if modeSelect:
	print('Enter Angle:\n(terminate with \"q\")\n-----------------------------')

else:
	print('Enter Speed:\n(terminate with \"q\")\n-----------------------------')


while True:

	value = input()

	if value == 'q':
		break

	if modeSelect:
		arduino.motorWriteSpeed(int(value))
	else:
		arduino.motorWritePosition(int(value))
