#!/usr/bin/env python

import os
import sys

import cv2

import json 
from importlib import import_module
from flask import Flask, request, render_template, Response, send_from_directory

sys.path.append('./turret')

from turret import Turret
from cameraMux import CameraMux


app = Flask(__name__, template_folder='html/templates', static_folder='html/static', static_url_path='')

#turr = Turret('/dev/ttyUSB0','/dev/ttyUSB1')

cam = CameraMux()


def getFrame():
    # Video streaming generator function.
    while True:

        frame = cam.getFrame()

        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')


@app.route('/')
def index():
    # Video streaming home page.
    return render_template('index.html')


@app.route('/video_feed')
def video_feed():
    # Video streaming route. Put this in the src attribute of an img tag.
    return Response(getFrame(), mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/controlle', methods=['POST'])
def controlle():

    return Response("controlle_response", status=200, mimetype='text/html')

    # Mouse movement handling routine
    data = json.loads(request.data)

    if 'xPos' in data:
        turr.setPositionX(data['xPos'])

    if 'yPos' in data:
        turr.setPositionY(data['yPos'])

    if 'xSpeed' in data:
        turr.setSpeedX(data['xSpeed'])

    if 'ySpeed' in data:
        turr.setSpeedY(data['ySpeed'])

    if 'trigger' in data:
        turr.trigger()

    return Response("controlle_response", status=200, mimetype='text/html')


@app.route('/option', methods=['POST'])
def option():
    data = json.loads(request.data)

    if 'setCameraFeed' in data:

        if data['setCameraFeed'] == 'rgb':
            cam.setCameraFeed(CameraMux.CAMERA_FEED_RGB)
        elif data['setCameraFeed'] == 'thermal':
            cam.setCameraFeed(CameraMux.CAMERA_FEED_THERMAL)
        elif data['setCameraFeed'] == 'hybrid':
            cam.setCameraFeed(CameraMux.CAMERA_FEED_HYBRID)

    if 'setThermalCameraColorMap' in data:
        if data['setThermalCameraColorMap'] == 'autumn':
            cam.setThermalColorMap(cv2.COLORMAP_AUTUMN)
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
        elif data['setThermalCameraColorMap'] == 'magma':
            cam.setThermalColorMap(cv2.COLORMAP_MAGMA)
        elif data['setThermalCameraColorMap'] == 'inferno':
            cam.setThermalColorMap(cv2.COLORMAP_INFERNO)
        elif data['setThermalCameraColorMap'] == 'plasma':
            cam.setThermalColorMap(cv2.COLORMAP_PLASMA)
        elif data['setThermalCameraColorMap'] == 'viridis':
            cam.setThermalColorMap(cv2.COLORMAP_VIRIDIS)
        elif data['setThermalCameraColorMap'] == 'cividis':
            cam.setThermalColorMap(cv2.COLORMAP_CIVIDIS)
        elif data['setThermalCameraColorMap'] == 'twilight':
            cam.setThermalColorMap(cv2.COLORMAP_TWILIGHT)
        elif data['setThermalCameraColorMap'] == 'twilightShifted':
            cam.setThermalColorMap(cv2.COLORMAP_TWILIGHT_SHIFTED)
        elif data['setThermalCameraColorMap'] == 'turbo':
            cam.setThermalColorMap(cv2.COLORMAP_TURBO)
        elif data['setThermalCameraColorMap'] == 'deepGreen':
            cam.setThermalColorMap(cv2.COLORMAP_DEEPGREEN)
        else:
            cam.setThermalColorMap(cv2.COLORMAP_BONE)
            
    return Response("reset_response", status=200, mimetype='text/html')

    if 'resetPosX' in data:
        turr.resetPostionX()

    if 'resetPosY' in data:
        turr.resetPostionY()

    return Response("reset_response", status=200, mimetype='text/html')


@app.route('/feedback', methods=['GET'])
def feedback():

    return '{}'

    if request.args.get('position'):
        xPos = turr.getPositionX()
        yPos = turr.getPositionY()
        return '{\"xPos\":\"%s\", \"yPos\":\"%s\"}' % (xPos, yPos)

    if request.args.get('target'):
        isAtTargetX = turr.isAtTargetX()
        isAtTargetY = turr.isAtTargetY()
        return '{\"xTar\":\"%s\", \"yTar\":\"%s\"}' % (isAtTargetX, isAtTargetY)


    return '{}'


if __name__ == '__main__':
    app.run(host='0.0.0.0', threaded=True)
