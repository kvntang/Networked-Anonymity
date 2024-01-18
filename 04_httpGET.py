import cv2
import urllib.request
import numpy as np
import threading
import time


# Server configuration
servers = {
    "server1": {"ip": "http://100.83.156.202"},
    "server2": {"ip": "http://100.83.156.201"},
    # Add more servers as needed
}

# Global dictionary to store frames for each server
frames = {server_name: None for server_name in servers}

#######################################################################################
# Function to fetch frames from the camera
def fetch_frame(server_name):
    while True:
        try:
            url = f"{servers[server_name]['ip']}/cam-hi.jpg"
            img_resp = urllib.request.urlopen(url) # GET request
            imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
            frames[server_name] = cv2.flip(cv2.imdecode(imgnp, -1), 0)
        except Exception as e:
            print(f"Error fetching frame from {server_name}: {e}")
            frames[server_name] = None  # Set to None when disconnected
            time.sleep(1)  # Wait for 1 seconds before trying to reconnect


# Function to send value and receive confirmation
def send_val(server_name):
    while True:
        try:
            slider_value = cv2.getTrackbarPos(server_name, server_name)
            url = f"{servers[server_name]['ip']}/setNumber?value={slider_value}"
            with urllib.request.urlopen(url) as response:  # Send GET request and retrieve confirmation
                if response.status == 200:
                    response_content = response.read().decode('utf-8')
                    print("Response from server:", response_content)
                else:
                    print("Failed to fetch response. Status code:", response.status)
        except Exception as e:
            print(f"Error while sending request from {server_name}: {e}")

##############################################################################################
# Create threads for each server
for server_name in servers:
    threading.Thread(target=fetch_frame, args=(server_name,)).start()
    threading.Thread(target=send_val, args=(server_name,)).start()

    # Create OpenCV windows and sliders
    cv2.namedWindow(server_name)
    cv2.createTrackbar(server_name, server_name, 0, 8, lambda x: None)

# MAIN LOOP #
while True:
    for server_name in servers:
        if frames[server_name] is not None:
            cv2.imshow(server_name, frames[server_name])
        else:
            # Display a placeholder image or text indicating waiting for connection
            placeholder = np.zeros((480, 640, 3), dtype=np.uint8)
            cv2.putText(placeholder, f"Waiting for {server_name}...", (50, 240), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
            cv2.imshow(server_name, placeholder)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
