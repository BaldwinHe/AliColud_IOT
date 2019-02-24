# coding=utf-8

import datetime
import time
import hmac
import hashlib
import math
import random

TEST = 0

ProductKey = "a1HtLanBEMH"
ClientId = "26lMb9TqgHsJEq0yTJB2"  # 自定义clientId
DeviceName = "26lMb9TqgHsJEq0yTJB2"
DeviceSecret = "poaa4C5xjXJbwkq0Ndzw4sgn7yhsgtAS"
publish="/sys/a1HtLanBEMH/26lMb9TqgHsJEq0yTJB2/thing/event/property/post"
subscribe="/a1HtLanBEMH/26lMb9TqgHsJEq0yTJB2/user/get"
# signmethod
signmethod = "hmacsha1"
# signmethod = "hmacmd5"

# 当前时间毫秒值
us = math.modf(time.time())[0]
ms = int(round(us * 1000))
timestamp = str(ms)

data = "".join(("clientId", ClientId, "deviceName", DeviceName,
                "productKey", ProductKey, "timestamp", timestamp
                ))
# print(round((time.time() * 1000)))
print("data:", data)

if "hmacsha1" == signmethod:
    ret = hmac.new(bytes(DeviceSecret, encoding="utf-8"),
                   bytes(data, encoding="utf-8"),
                   hashlib.sha1).hexdigest()
elif "hmacmd5" == signmethod:
    ret = hmac.new(bytes(DeviceSecret, encoding="utf-8"),
                   bytes(data, encoding="utf-8"),
                   hashlib.md5).hexdigest()
else:
    raise ValueError

sign = ret
print("sign:", sign)

# ======================================================

strBroker = ProductKey + ".iot-as-mqtt.cn-shanghai.aliyuncs.com"
port = 1883

client_id = "".join((ClientId,
                     "|securemode=3",
                     ",signmethod=", signmethod,
                     ",timestamp=", timestamp,
                     "|"))
username = "".join((DeviceName, "&", ProductKey))
password = sign

print("="*30)
print("client_id:", client_id)
print("username:", username)
print("password:", password)
print("="*30)

def secret_test():
    DeviceSecret = "secret"
    data = "clientId12345deviceNamedeviceproductKeypktimestamp789"
    ret = hmac.new(bytes(DeviceSecret, encoding="utf-8"),
                   bytes(data, encoding="utf-8"),
                   hashlib.sha1).hexdigest()
    print("test:", ret)


# ======================================================
# MQTT Initialize.--------------------------------------

try:
    import paho.mqtt.client as mqtt
except ImportError:
    print("MQTT client not find. Please install as follow:")
    print("git clone http://git.eclipse.org/gitroot/paho/org.eclipse.paho.mqtt.python.git")
    print("cd org.eclipse.paho.mqtt.python")
    print("sudo python setup.py install")


# ======================================================
# The callback for when the client receives a CONNACK response from the server.
def on_connect(mqttc, obj, flags,rc):
    print("OnConnetc, rc: " + str(rc))
    mqttc.subscribe(subscribe, 0)
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
# The callback for when a PUBLISH message is received from the server.
def on_publish(mqttc, obj, mid):
    print("OnPublish, mid: " + str(mid))


def on_subscribe(mqttc, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))


def on_log(mqttc, obj, level, string):
    print("Log:" + string)


def on_message(mqttc, obj, msg):
    curtime = datetime.datetime.now()
    strcurtime = curtime.strftime("%Y-%m-%d %H:%M:%S")
    print(strcurtime + ": " + msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    on_exec(str(msg.payload))


def on_exec(strcmd):
    print("Exec:", strcmd)
    strExec = strcmd


# =====================================================
if __name__ == '__main__':
    if TEST:
        secret_test()
        exit(0)

    mqttc = mqtt.Client(client_id)
    mqttc.username_pw_set(username, password)
    mqttc.on_message = on_message
    mqttc.on_connect = on_connect
    mqttc.on_publish = on_publish
    mqttc.on_subscribe = on_subscribe
    mqttc.on_log = on_log
    mqttc.connect(strBroker, port, 300)
    while True:
        x=random.randrange(100)
        mqttc.publish(publish,'{"id":"12345","method":"thing.event.property.post","params":{"PM25Value":%s,}}' %x)
        time.sleep(5)
        mqttc.loop()