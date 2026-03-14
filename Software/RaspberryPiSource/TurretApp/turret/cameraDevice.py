import cv2

class Camera():

    def __init__(self):
        self.capture=cv2.VideoCapture(0)

    def __del__(self):
        self.capture.release()

    def get_frame(self):
        if not self.capture.isOpened():
            raise RuntimeError('Could not start camera.')

        # read current frame
        _, img = self.capture.read()

        # encode as a jpeg image and return it
        return cv2.imencode('.jpg', img)[1].tobytes()
