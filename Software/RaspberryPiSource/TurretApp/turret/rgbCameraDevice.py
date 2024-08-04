import cv2

class RgbCamera():

    def __init__(self, device=0):
        self.capture=cv2.VideoCapture(device)


    def __del__(self):
        self.capture.release()


    def read(self):
        if not self.capture.isOpened():
            #raise RuntimeError('Could not start camera.')
            return False, None

        # read current frame
        ret, frame = self.capture.read()
        
        # calculate the roi of the thermal camera
        # only show the part of the image that overlap
        # TODO make roi adjustable
        width  = self.capture.get(cv2.CAP_PROP_FRAME_WIDTH)
        height = self.capture.get(cv2.CAP_PROP_FRAME_HEIGHT)
    
        offsetX = int(width/5)
        offsetY = int(height/9)
        
        sizeX = int(width/1.5)
        sizeY = int(height/1.5)
        
        roi = frame[offsetY:offsetY+sizeY, offsetX:offsetX+sizeX]
        
        roi = cv2.resize(roi, (640, 480))

        # dont use roi for now
        frame = cv2.resize(frame, (640, 480))
        
        return ret, frame
        

    def getFrame(self):
        
        ret, frame = self.read()

        if not ret:
            return ret, None

        # encode as a jpeg image and return it
        return ret, cv2.imencode('.jpg', frame)[1].tobytes()
