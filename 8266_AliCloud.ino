#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
//Wifi 配置
#define WIFI_SSID         ""
#define WIFI_PASSWD       ""

/* 产品的三元组信息*/
#define PRODUCT_KEY       ""
#define DEVICE_NAME       ""
#define DEVICE_SECRET     ""
#define REGION_ID          "cn-shanghai"
/* LD线上环境域名和端口号，不需要改 */
#define MQTT_SERVER       PRODUCT_KEY ".iot-as-mqtt." REGION_ID ".aliyuncs.com"
#define MQTT_PORT          1883
#define MQTT_USRNAME      DEVICE_NAME "&" PRODUCT_KEY

// TODO: MQTT连接的签名信息，哈希加密请以"clientIdtestdeviceName"+设备名称+"productKey"+设备模型标识+“timestamp123456789”前往http://tool.oschina.net/encrypt?type=2进行加密
#define CLIENT_ID         "12345|securemode=3,signmethod=hmacsha1,timestamp=123456789|"
#define MQTT_PASSWD       ""

#define ALINK_BODY_FORMAT         "{\"id\":\"12345\",\"method\":\"thing.event.property.post\",\"params\":%s}"
#define ALINK_TOPIC_PROP_POST     "/sys/"PRODUCT_KEY"/"DEVICE_NAME"/thing/event/property/post"
//==========================================
//PubSubClient.h库参数修改，否则连不上
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
//Publish消息，
void mqttIntervalPost()
{
    char param[200];
    char jsonBuf[1000];
    int x=150;
    sprintf(param, "{\"PM25Value\":%d}",x); //此处填写属性的标识符，例如PM25Value
    sprintf(jsonBuf, ALINK_BODY_FORMAT, param);
    Serial.println(jsonBuf);
    boolean d = client.publish(ALINK_TOPIC_PROP_POST, jsonBuf);
    Serial.println("publish:0=Failed;1=Success");
    Serial.println(d);
}

void setup() 
{
    /* initialize serial for debugging */
    Serial.begin(115200);
    wifiInit();
}

// the loop function runs over and over again forever
void loop()
{
    if (millis() - lastMs >= 10000)//每10秒publish一下
    {
        lastMs = millis();
        mqttCheckConnect(); 
        mqttIntervalPost();
    }
    client.loop();
}
