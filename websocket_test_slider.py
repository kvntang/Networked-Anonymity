#slider client input: this code workes
#Jan 14, 2024

import cv2
from websocket import create_connection, WebSocketConnectionClosedException

def send_msg(ws, msg, ip):
    """ Sends a message to the websocket server and returns the response. """
    try:
        ws.send(msg)
        return ws.recv()
    except WebSocketConnectionClosedException:
        print("Connection closed by server. Reconnecting...")
        ws = create_connection(f"ws://{ip}", timeout=3)
        ws.send(msg)
        return ws.recv()
    except Exception as err:
        print(f"Error sending/receiving data: {err}")
        return ""

def on_slider_change(val):
    # This function will be called whenever the slider's value changes
    pass

def main():
    ip = "100.83.156.202:81"
    ws = None

    # Create an OpenCV window with a slider
    cv2.namedWindow("Slider")
    cv2.createTrackbar("Value", "Slider", 1, 8, on_slider_change)

    try:
        ws = create_connection(f"ws://{ip}", timeout=3)
        print(f"Connected to WebSocket server, IP {ip}")

        while True:
            # Read the value of the slider
            slider_value = cv2.getTrackbarPos("Value", "Slider")

            # Send the slider value as a message
            response = send_msg(ws, str(slider_value), ip)
            print(f"Sent: {slider_value}, Received: {response}")

            # Break the loop if 'q' is pressed
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

    except Exception as err:
        print(f"Error connecting to the server: {err}")
    finally:
        if ws:
            ws.close()
            print("Connection closed")
        cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
