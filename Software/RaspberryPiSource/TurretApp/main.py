#!/usr/bin/env python

import os
import sys
import json 
from importlib import import_module
from flask import Flask, request, render_template, Response, send_from_directory

sys.path.append('./turret')
from rgbCameraDevice import Camera
from turret import Turret


app = Flask(__name__, template_folder='html/templates', static_folder='html/static', static_url_path='')
turr = Turret('/dev/ttyUSB0','/dev/ttyUSB1')
cam = Camera()

def getFrame():
    # Video streaming generator function.
    while True:
        frame = cam.get_frame()
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

    if 'resetPosX' in data:
        turr.resetPostionX()

    if 'resetPosY' in data:
        turr.resetPostionY()

    return Response("reset_response", status=200, mimetype='text/html')


@app.route('/feedback', methods=['GET'])
def feedback():

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
