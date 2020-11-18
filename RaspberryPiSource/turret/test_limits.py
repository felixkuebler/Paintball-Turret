import time
from arduinoSerialDevice import ArduinoSerialDevice

arduinoBottom = ArduinoSerialDevice(port='/dev/ttyUSB0', baudrate=9600)
arduinoTop = ArduinoSerialDevice(port='/dev/ttyUSB1', baudrate=9600)
time.sleep(2)

arduinoTop.pinModeOutput(4)

arduinoTop.motorAttach(5,6)
arduinoTop.motorInvertDir(0)
arduinoTop.motorSetRange(9000)
arduinoTop.motorSetGearRatio(5)
arduinoTop.motorSetMinSpeed(100)
arduinoTop.motorSetMaxSpeed(200)

arduinoBottom.motorAttach(5,6)
arduinoBottom.motorInvertDir(1)
arduinoBottom.motorSetGearRatio(10)
arduinoBottom.motorSetRange(18000)
arduinoBottom.motorSetMinSpeed(75)
arduinoBottom.motorSetMaxSpeed(125)

time.sleep(1)

arduinoBottom.motorWritePosition(9000)

while not arduinoBottom.motorIsAtTarget():
	time.sleep(0.1)

arduinoBottom.motorWritePosition(-9000)

while not arduinoBottom.motorIsAtTarget():
	time.sleep(0.1)

arduinoBottom.motorWritePosition(0)

while not arduinoBottom.motorIsAtTarget():
	time.sleep(0.1)

arduinoTop.motorWritePosition(4500)

while not arduinoTop.motorIsAtTarget():
	time.sleep(0.1)

arduinoTop.motorWritePosition(-4500)

while not arduinoTop.motorIsAtTarget():
	time.sleep(0.1)

arduinoTop.motorWritePosition(0)

while not arduinoTop.motorIsAtTarget():
	time.sleep(0.1)

arduinoTop.digitalWrite(4,1)
arduinoTop.digitalWrite(4,0)

