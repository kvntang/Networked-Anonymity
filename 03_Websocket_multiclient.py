import cv2
import urllib.request
import numpy as np
import threading
import websocket
import time

# Server configuration
servers = {
    "server1": {"stream_url": "http://100.83.156.202:80/cam-hi.jpg", "ws_addr": "ws://192.168.1.101:81"},
    # Add more servers as needed
}

# Global dictionary to store frames for each server
frames = {server_name: None for server_name in servers}

#######################################################################################
# Function to fetch frames from the camera
def fetch_frame(server_name):
    while True:
        try:
            img_resp = urllib.request.urlopen(servers[server_name]["stream_url"])
            imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
            frames[server_name] = cv2.flip(cv2.imdecode(imgnp, -1), 0)
        except Exception as e:
            print(f"Error fetching frame from {server_name}: {e}")

# Function to handle WebSocket communication


##############################################################################################
# Create threads for each server
for server_name in servers:
    thread = threading.Thread(target=fetch_frame, args=(server_name,))
    thread.start()
    # thread_ws = threading.Thread(target=websocket_thread_func, args=(server_name,))
    # thread_ws.start()

    threading.Thread(target=fetch_frame, args=(server_name,)).start()
    # threading.Thread(target=websocket_thread_func, args=(server_name,)).start()

    # Create OpenCV windows and sliders
    cv2.namedWindow(server_name)
    # cv2.createTrackbar(f"LED Control {server_name}", server_name, 0, 1, lambda x: None)


##############################################################################################
while True:
    for server_name in servers:
        if frames[server_name] is not None:
            cv2.imshow(server_name, frames[server_name])

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
