import paho.mqtt.client as paho


# MQTT function when something is published
def on_publish(client, userdata, result):
    print("data published!")

def main():
    # MQTT raspberry pi IP address and port
    broker = "192.168.43.181"
    port = 1883
    # Sending info that the upload process
    # to the microcontroler (sensor)
    client = paho.Client("feedback")
    client.on_publish = on_publish
    client.connect(broker, port)
    ret = client.publish("rasp/feedback", "1")
    print(ret)


if __name__ == '__main__':
    main()
