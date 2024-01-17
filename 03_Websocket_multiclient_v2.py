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



# Constants for rate limiting
FETCH_FRAME_INTERVAL = 0.1  # seconds (10 frames per second)
WEBSOCKET_INTERVAL = 0.1  # seconds (Check slider every 100 ms)





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
        time.sleep(FETCH_FRAME_INTERVAL)  # Rate limiting

def send_msg(ws, msg):
    try:
        ws.send(msg)
        return ws.recv()
    except websocket.WebSocketConnectionClosedException:
        print("Connection closed by server. Reconnecting...")
        ws.connect(ws.remote_address)
        ws.send(msg)
        return ws.recv()
    except Exception as err:
        print(f"Error sending/receiving data: {err}")
        return ""
    
def websocket_thread_func(server_name):
    ws_addr = servers[server_name]["ws_addr"]
    slider_name = f"{server_name}"
    ws = None

    try:
        ws = websocket.create_connection(ws_addr, timeout=3)
        print(f"Connected to WebSocket server at {ws_addr}")

        while True:
            slider_value = cv2.getTrackbarPos(slider_name, server_name) #will the slider be on the same window?
            response = send_msg(ws, str(slider_value))
            print(f"Sent: {slider_value}, Received: {response}")
            # print(f"Sent: {slider_value}")
            time.sleep(WEBSOCKET_INTERVAL) # Rate limiting

    except Exception as err:
        print(f"Error in WebSocket communication: {err}")
    finally:
        if ws:
            ws.close()


def websocket_thread_func_2(server_name):
    ws_addr = servers[server_name]["ws_addr"]
    slider_name = f"{server_name}"
    ws = None

    last_slider_value = -1

    try:
        ws = websocket.create_connection(ws_addr, timeout=3)
        print(f"Connected to WebSocket server at {ws_addr}")

        while True:
            slider_value = cv2.getTrackbarPos(slider_name, server_name) #will the slider be on the same window?
            if slider_value != last_slider_value: #update only if change
                last_slider_value = slider_value
                response = send_msg(ws, str(slider_value))
                print(f"Sent: {slider_value}, Received: {response}")
            time.sleep(WEBSOCKET_INTERVAL) # Rate limiting

    except Exception as err:
        print(f"Error in WebSocket communication: {err}")
    finally:
        if ws:
            ws.close()

##############################################################################################
# MAIN LOOP #
# Create threads for each server
for server_name in servers:
    threading.Thread(target=fetch_frame, args=(server_name,)).start()
    threading.Thread(target=websocket_thread_func_2, args=(server_name,)).start()
    cv2.namedWindow(server_name)
    cv2.createTrackbar(f"{server_name}", server_name, 0, 8, lambda x: None)

while True:
    for server_name in servers:
        if frames[server_name] is not None:
            cv2.imshow(server_name, frames[server_name])

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
