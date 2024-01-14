import cv2
import urllib.request
import numpy as np
import threading

url_1 = 'http://100.83.156.202/cam-hi.jpg'
url_2 = 'http://100.82.1.247/cam-hi.jpg'

frame1 = None
frame2 = None

def fetch_frame(url, frame_number):
    global frame1, frame2
    while True:
        img_resp = urllib.request.urlopen(url)
        imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
        frame = cv2.imdecode(imgnp, -1)
        if frame_number == 1:
            frame1 = frame
        else:
            frame2 = frame

# Start threads for each URL
thread1 = threading.Thread(target=fetch_frame, args=(url_1, 1,))
thread1.start()
thread2 = threading.Thread(target=fetch_frame, args=(url_2, 2,))
thread2.start()

while True:
    if frame1 is not None:
        cv2.imshow('Cam1', frame1)
    if frame2 is not None:
        cv2.imshow('Cam2', frame2)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
