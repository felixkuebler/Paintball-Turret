#!/usr/bin/env python

import os
import sys
import json 
from importlib import import_module
from flask import Flask, request, render_template, Response

sys.path.append('./turret')
from cameraDevice import Camera
from turret import Turret


app = Flask(__name__, template_folder = 'html/templates')
turr = Turret('/dev/ttyUSB0','/dev/ttyUSB1')


def getFrame(camera):
    # Video streaming generator function.
    while True:
        frame = camera.get_frame()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')


@app.route('/')
def index():
    # Video streaming home page.
    return render_template('index.html')


@app.route('/video_feed')
def video_feed():
    # Video streaming route. Put this in the src attribute of an img tag.
    return Response(getFrame(Camera()), mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/trigger', methods=['POST'])
def trigger():
    # Trigger handling routine
    turr.trigger()

    return Response("trigger_response", status=200, mimetype='text/html')



@app.route('/move', methods=['POST'])
def move():
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

    return Response("move_response", status=200, mimetype='text/html')



@app.route('/reset', methods=['POST'])
def reset():
    # Mouse movement handling routine
    data = json.loads(request.data)

    if 'xPos' in data:
        turr.resetPostionX()

    if 'yPos' in data:
        turr.resetPostionY()

    return Response("reset_response", status=200, mimetype='text/html')


@app.route('/position', methods=['GET'])
def position():
    #data = json.loads(request.data)

    xPos = turr.getPositionX()
    yPos = turr.getPositionY()

    return '{\"xPos\":\"%s\", \"yPos\":\"%s\"}' % (xPos, yPos)

if __name__ == '__main__':
    app.run(host='0.0.0.0', threaded=True)
