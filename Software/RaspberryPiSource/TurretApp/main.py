#!/usr/bin/env python

import os
import sys

import cv2

import json 
from importlib import import_module
from flask import Flask, request, render_template, Response, send_from_directory

import time

import threading

sys.path.append('./turret')

from arduinoSerialDevice import ArduinoSerialDevice
from cameraMux import CameraMux


app = Flask(__name__, template_folder='html/templates', static_folder='html/static', static_url_path='')

serial = ArduinoSerialDevice()
cam = CameraMux()

yawPos = 0
pitchPos = 0
positionUpdate = False

def getFrame():
    # Video streaming generator function.
    while True:

        ret, frame = cam.getFrame()

        # use no signal image if a fault in the selected camera was returend
        if not ret:
            frame = cv2.imread('html/static/img/testImage.jpg')
            frame = cv2.imencode('.jpg', frame)[1].tobytes()

        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')


def getPosition():
    # Position streaming generator function.
    
    global pitchPos
    global yawPos
    global positionUpdate
        
    while True:

        if positionUpdate:
            positionUpdate = False
            data = '{\"pitchPos\":\"%s\", \"yawPos\":\"%s\"}\n' % (pitchPos, yawPos)
            yield (data)
        else:
            yield()
            
        # only stream ever 100ms
        time.sleep(100/1000)


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/videoFeed')
def videoFeed():
    return Response(getFrame(), mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/positionFeed')
def positionFeed():
    return Response(getPosition(), mimetype='text/plain')
    

@app.route('/control', methods=['POST'])
def control():

    # Mouse movement handling routine
    data = json.loads(request.data)

    if 'yawPos' in data:
        serial.motorYawWritePositionAbsolute(-int(data['yawPos']))

    if 'pitchPos' in data:
        serial.motorPitchWritePositionAbsolute(-int(data['pitchPos']))

    if ('pitchSpeed' in data) and ('yawSpeed' in data):
        serial.motorWriteSpeed(-int(data['pitchSpeed']), -int(data['yawSpeed']))
        
    else:
        
        if 'pitchSpeed' in data:
            serial.motorPitchWriteSpeed(-int(data['pitchSpeed']))
            
        if 'yawSpeed' in data:
            serial.motorYawWriteSpeed(-int(data['yawSpeed']))

    if 'trigger' in data:
        serial.triggerPrimary(int(data['trigger']))

    return Response("control_response", status=200, mimetype='text/html')


@app.route('/option', methods=['POST'])
def option():
    data = json.loads(request.data)

    if 'setCameraZoom' in data:
        
        if data['setCameraZoom'] == 'true':
            cam.setCameraZoom(True)
        else:
            cam.setCameraZoom(False)
            
    if 'setCameraFeed' in data:

        if data['setCameraFeed'] == 'rgb':
            cam.setCameraFeed(CameraMux.CAMERA_FEED_RGB)
        elif data['setCameraFeed'] == 'thermalAbs':
            cam.setCameraFeed(CameraMux.CAMERA_FEED_THERMAL_ABS)
        elif data['setCameraFeed'] == 'thermalNorm':
            cam.setCameraFeed(CameraMux.CAMERA_FEED_THERMAL_NORM)
        elif data['setCameraFeed'] == 'hybrid':
            cam.setCameraFeed(CameraMux.CAMERA_FEED_HYBRID)

    if 'setThermalCameraColorMap' in data:
        if data['setThermalCameraColorMap'] == 'autumn':
            cam.setThermalColorMap(cv2.COLORMAP_AUTUMN)
        elif data['setThermalCameraColorMap'] == 'bone':
            cam.setThermalColorMap(cv2.COLORMAP_BONE)
        elif data['setThermalCameraColorMap'] == 'jet':
            cam.setThermalColorMap(cv2.COLORMAP_JET)
        elif data['setThermalCameraColorMap'] == 'winter':
            cam.setThermalColorMap(cv2.COLORMAP_WINTER)
        elif data['setThermalCameraColorMap'] == 'rainbow':
            cam.setThermalColorMap(cv2.COLORMAP_RAINBOW)
        elif data['setThermalCameraColorMap'] == 'ocean':
            cam.setThermalColorMap(cv2.COLORMAP_OCEAN)
        elif data['setThermalCameraColorMap'] == 'summer':
            cam.setThermalColorMap(cv2.COLORMAP_SUMMER)
        elif data['setThermalCameraColorMap'] == 'spring':
            cam.setThermalColorMap(cv2.COLORMAP_SPRING)
        elif data['setThermalCameraColorMap'] == 'cool':
            cam.setThermalColorMap(cv2.COLORMAP_COOL)
        elif data['setThermalCameraColorMap'] == 'hsv':
            cam.setThermalColorMap(cv2.COLORMAP_HSV)
        elif data['setThermalCameraColorMap'] == 'pink':
            cam.setThermalColorMap(cv2.COLORMAP_PINK)
        elif data['setThermalCameraColorMap'] == 'hot':
            cam.setThermalColorMap(cv2.COLORMAP_HOT)
        elif data['setThermalCameraColorMap'] == 'parula':
            cam.setThermalColorMap(cv2.COLORMAP_PARULA)
        else:
            cam.setThermalColorMap(CameraMux.THERMAL_COLOR_MAP_DEFAULT)
            
    if 'calibrateMotorPitch' in data:
        serial.motorPitchCalibrate()
        
    if 'calibrateMotorYaw' in data:
        serial.motorYawCalibrate()
        
    return Response("reset_response", status=200, mimetype='text/html')


@app.route('/feedback', methods=['GET'])
def feedback():
    
    return '{}'


def readPositionStream():
    
    global pitchPos
    global yawPos
    global positionUpdate
    
    serial.enableDataStream()
    
    while True:
        source, position = serial.readDataStream()
        
        if (source == serial.CMD_MOTOR_PITCH_READ_POSITION.value):
            pitchPos = position
            positionUpdate = True
        elif (source == serial.CMD_MOTOR_YAW_READ_POSITION.value):
            yawPos = position
            positionUpdate = True
            
            
if __name__ == '__main__':
    # create a dedicated thread to poll data from ArduinoSerialDevice
    # this is important to prevent html request blocking
    readPositionThread = threading.Thread(target = readPositionStream)
    readPositionThread.start()
    
    app.run(host='0.0.0.0', threaded=True)
