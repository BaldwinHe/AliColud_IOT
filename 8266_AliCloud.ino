#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define WIFI_SSID         "RogersA0068"
#define WIFI_PASSWD       "7EAB7Rogers"
int SENSOR_PIN=10;
/* 产品的三元组信息，根据9个测试设备的三元组，每个设备都烧录不同的*/
#define PRODUCT_KEY       "a1HtLanBEMH"
#define DEVICE_NAME       "ESP8266_001"
#define DEVICE_SECRET     "YpWkcNpTUxFFBXvHgsmKBwiC47xom4mc"
#define REGION_ID          "cn-shanghai"
/* LD线上环境域名和端口号，不需要改 */
#define MQTT_SERVER       PRODUCT_KEY ".iot-as-mqtt." REGION_ID ".aliyuncs.com"
#define MQTT_PORT          1883
#define MQTT_USRNAME      DEVICE_NAME "&" PRODUCT_KEY

// TODO: MQTT连接的签名信息，哈希加密请以"clientIdtestdeviceName"+设备名称+"productKey"+设备模型标识+“timestamp123456789”前往http://tool.oschina.net/encrypt?type=2进行加密
#define CLIENT_ID         "12345|securemode=3,signmethod=hmacsha1,timestamp=123456789|"
#define MQTT_PASSWD       "a07778bf512d2e5748de5de44b625105ec4c9df3"

#define ALINK_BODY_FORMAT         "{\"id\":\"12345\",\"method\":\"thing.event.property.post\",\"params\":%s}"
#define ALINK_TOPIC_PROP_POST     "/sys/"PRODUCT_KEY"/"DEVICE_NAME"/thing/event/property/post"
//==========================================
//Lib parameters update
// MQTT_MAX_PACKET_SIZE : Maximum packet size
//#define MQTT_MAX_PACKET_SIZE 1024
// MQTT_KEEPALIVE : keepAlive interval in Seconds
//#define MQTT_KEEPALIVE 60
//==========================================
unsigned long lastMs = 0;

WiFiClient    espClient;
PubSubClient  client(espClient);

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    payload[length] = '\0';
    Serial.println((char *)payload);
}

void wifiInit()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("WiFi not Connect");
    }
    Serial.println("Connected to AP");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
Serial.print("espClient [");
    client.setServer(MQTT_SERVER, MQTT_PORT);   /* 连接WiFi之后，连接MQTT服务器 */
    client.setCallback(callback);
}

void mqttCheckConnect()
{
    while (!client.connected())
    {
        Serial.println("Connecting to MQTT Server ...");
        if (client.connect(CLIENT_ID, MQTT_USRNAME, MQTT_PASSWD))

        {
            Serial.println("MQTT Connected!");
        }
        else
        {
            Serial.print("MQTT Connect err:");
            Serial.println(client.state());
            delay(5000);
        }
    }
}

void mqttIntervalPost()
{
    char param[200];
    char jsonBuf[1000];
    int x=150;
    sprintf(param, "{\"PM25Value\":%d}",x);
    sprintf(jsonBuf, ALINK_BODY_FORMAT, param);
    Serial.println(jsonBuf);
    boolean d = client.publish(ALINK_TOPIC_PROP_POST, jsonBuf);
    Serial.println("publish:0 失败;1成功");
    Serial.println(d);
}

void setup() 
{
    pinMode(13,  INPUT);
    /* initialize serial for debugging */
    Serial.begin(115200);
    Serial.println("Demo Start");
    wifiInit();
}

// the loop function runs over and over again forever
void loop()
{
    if (millis() - lastMs >= 10000)
    {
        lastMs = millis();
        mqttCheckConnect(); 
        /* 上报消息心跳周期 */
        mqttIntervalPost();
    }
    client.loop();
}
