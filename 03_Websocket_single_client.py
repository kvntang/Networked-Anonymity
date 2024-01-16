import cv2
import urllib.request
import numpy as np
import threading
import websocket
import time

# URLs for camera streams
url_1 = 'http://100.83.156.202:80/cam-hi.jpg'  # Replace with actual URL
# ws_addr_1 = "ws://100.83.156.202:81"  # Replace with actual WebSocket address

frame1 = None

#######################################################################################
# Function to fetch frames from the camera
def fetch_frame(url, frame_number):
    global frame1, frame2
    while True:
        try:
            img_resp = urllib.request.urlopen(url)
            imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
            frame = cv2.imdecode(imgnp, -1)
            if frame_number == 1:
                frame1 = frame
                frame1 = cv2.flip(frame1, 0)
            else:
                frame2 = frame
        except Exception as e:
            print(f"Error fetching frame: {e}")

# Function to handle WebSocket communication
def websocket_thread_func(ws_addr, slider_name):
    def on_message(ws, message):
        print(f"Received from {ws_addr}: {message}")

    def on_error(ws, error):
        print(f"Error from {ws_addr}: {error}")

    def on_close(ws, close_status_code, close_msg):
        print(f"WebSocket {ws_addr} closed")

    def on_open(ws):
        def run(*args):
            while True:
                slider_value = cv2.getTrackbarPos(slider_name, slider_name) #get val from slider
                ws.send(str(slider_value)) #send to esp32
                time.sleep(1)  # Wait before sending the next value
        threading.Thread(target=run).start()

    websocket.enableTrace(True)
    ws = websocket.WebSocketApp(ws_addr,
                                on_open=on_open,
                                on_message=on_message,
                                on_error=on_error,
                                on_close=on_close)
    ws.run_forever()


##############################################################################################
            
# Start threads for fetching frames
thread1 = threading.Thread(target=fetch_frame, args=(url_1, 1,))
thread1.start()

# Start WebSocket threads
# thread_ws1 = threading.Thread(target=websocket_thread_func, args=(ws_addr_1, "LED Control ESP32 1",))
# thread_ws1.start()

# Create OpenCV windows and sliders
cv2.namedWindow('Camera 1')
# cv2.createTrackbar('LED Control ESP32 1', 'Camera 1', 0, 1, lambda x: None)

##############################################################################################
while True:
    if frame1 is not None:
        
        cv2.imshow('Camera 1', frame1)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
