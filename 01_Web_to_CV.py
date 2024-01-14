import cv2
import urllib.request
import numpy as np

# URLs of the IP cameras
url_1 = 'http://100.83.156.202/cam-hi.jpg'
url_2 = 'http://100.82.1.247/cam-hi.jpg'

# Named windows for display
cv2.namedWindow("Cam1", cv2.WINDOW_AUTOSIZE)
cv2.namedWindow("Cam2", cv2.WINDOW_AUTOSIZE)

def get_frame_from_url(url):
    """Get a single frame from the given URL."""
    img_resp = urllib.request.urlopen(url)
    imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
    frame = cv2.imdecode(imgnp, -1)
    return frame

# Read and display video frames
while True:
    # Get frames from each URL
    frame1 = get_frame_from_url(url_1)
    frame2 = get_frame_from_url(url_2)

    # Optional: Flip the frames vertically
    frame1 = cv2.flip(frame1, 0)
    frame2 = cv2.flip(frame2, 0)

    # Display the frames
    cv2.imshow('Cam1', frame1)
    cv2.imshow('Cam2', frame2)

    # Break the loop with 'q' key
    if cv2.waitKey(5) & 0xFF == ord('q'):
        break

# Cleanup
cv2.destroyAllWindows()
