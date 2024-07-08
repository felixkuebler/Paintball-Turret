import cv2

class RgbCamera():

    def __init__(self, device=0):
        self.capture=cv2.VideoCapture(device)


    def __del__(self):
        self.capture.release()


    def get_frame(self):
        if not self.capture.isOpened():
            raise RuntimeError('Could not start camera.')

        # read current frame
        _, img = self.capture.read()

        # encode as a jpeg image and return it
        return cv2.imencode('.jpg', img)[1].tobytes()