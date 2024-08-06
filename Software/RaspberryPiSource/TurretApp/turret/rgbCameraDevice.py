import cv2

class RgbCamera():

    def __init__(self, device=0):
        self.capture=cv2.VideoCapture(device)


    def __del__(self):
        self.capture.release()


    def readRaw(self):
        if not self.capture.isOpened():
            #raise RuntimeError('Could not start camera.')
            return False, None

        # read current frame
        return self.capture.read()
        

    def read(self, zoomEnabled=False):
        ret, frame = self.readRaw()
        
        if (zoomEnabled):

            width  = self.capture.get(cv2.CAP_PROP_FRAME_WIDTH)
            height = self.capture.get(cv2.CAP_PROP_FRAME_HEIGHT)
        
            frame = frame[int(height/4):int(height*3/4), int(width/4):int(width*3/4)]
            
        frame = cv2.resize(frame, (640, 480))
        
        return ret, frame
        

    def getFrame(self):
        
        ret, frame = self.read()

        if not ret:
            return ret, None

        # encode as a jpeg image and return it
        return ret, cv2.imencode('.jpg', frame)[1].tobytes()
