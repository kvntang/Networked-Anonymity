import cv2
import urllib.request
import numpy as np
import threading
import websocket
import time

# Server configuration
servers = {
    "server1": {"stream_url": "http://100.83.156.202:80/cam-hi.jpg", "ws_addr": "ws://100.83.156.202:81"},
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
def websocket_thread_func(server_name):
    ws_addr = servers[server_name]["ws_addr"]
    slider_name = f"LED Control {server_name}"

    def on_message(ws, message):
        print(f"Received from {ws_addr}: {message}")

    def on_error(ws, error):
        print(f"Error from {ws_addr}: {error}")

    def on_close(ws, close_status_code, close_msg):
        print(f"WebSocket {ws_addr} closed")

    def on_open(ws):
        def run(*args):
            while True:
                slider_value = cv2.getTrackbarPos(slider_name, server_name)
                ws.send(str(slider_value))
                time.sleep(1)
        threading.Thread(target=run).start()

    websocket.enableTrace(True)
    ws = websocket.WebSocketApp(ws_addr,
                                on_open=on_open,
                                on_message=on_message,
                                on_error=on_error,
                                on_close=on_close)
    ws.run_forever()


##############################################################################################
# Create threads for each server
for server_name in servers:
    threading.Thread(target=fetch_frame, args=(server_name,)).start()
    threading.Thread(target=websocket_thread_func, args=(server_name,)).start()
    cv2.namedWindow(server_name)
    cv2.createTrackbar(f"LED Control {server_name}", server_name, 0, 1, lambda x: None)


##############################################################################################
while True:
    for server_name in servers:
        if frames[server_name] is not None:
            cv2.imshow(server_name, frames[server_name])

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
