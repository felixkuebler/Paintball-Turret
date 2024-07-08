import cv2

class RgbCamera():

    def __init__(self, device=0):
        self.capture=cv2.VideoCapture(device)


    def __del__(self):
        self.capture.release()


    def read(self):
        if not self.capture.isOpened():
            raise RuntimeError('Could not start camera.')

        # read current frame
        return self.capture.read()
        

    def getFrame(self):
        
        ret, frame = self.read()

        # encode as a jpeg image and return it
        return cv2.imencode('.jpg', frame)[1].tobytes()