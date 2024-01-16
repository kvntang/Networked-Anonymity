#text client input: this code workes
#Jan 14, 2024

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

def main():
    ip = "100.83.156.202:81"
    ws = None
    try:
        ws = create_connection(f"ws://{ip}", timeout=3)
        print(f"Connected to WebSocket server, IP {ip}")

        while True:
            instr = input("> ")
            if len(instr) == 0:
                break
            print(send_msg(ws, instr, ip))

    except Exception as err:
        print(f"Error connecting to the server: {err}")
    finally:
        if ws:
            ws.close()
            print("Connection closed")

if __name__ == "__main__":
    main()
