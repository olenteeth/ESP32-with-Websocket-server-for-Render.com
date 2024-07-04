#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
//#include <BluetoothSerial.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>
#define DHTPIN 27
#define DHTTYPE DHT11
#define LED 2 
#define led1 15
#define led2 12
#define led3 14
DHT dht(DHTPIN, DHTTYPE);
//BluetoothSerial btserial;
//char incomingbt;
//String messagebt="";
WebSocketsClient webSocket;
WiFiMulti WiFiMulti;
const char *ssid = "username";
const char *password = "password";
const char *websocket_server = "link_on_render"; 
const int websocket_port = 443;
const char *url = "/";
int status=0, status1=2, status2=4, status3=6, temp=0;
// Tạo dữ liệu JSON
DynamicJsonDocument jsonDoc(128); // Kích thước bộ nhớ cần cho JSON
float temperature, humidity;
unsigned long timeloop=0;

//use cmd
//openssl s_client -showcerts -connect <link web>:443
const char ENDPOINT_CA_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
// CA pass here
-----END CERTIFICATE-----
)EOF";

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
  {
    Serial.printf("[WSc] Connected to url: %s\n", payload);
  }
  break;
  case WStype_TEXT:
    String message;
    for (size_t i = 0; i < length; i++)
    {
      message += char(payload[i]);
    }
    Serial.print("Received message: ");
    Serial.println(message);
    if(message.indexOf("ESP32")!=-1)
    {
      if (message.indexOf("LED1_ON")!=-1)
    {
      digitalWrite(LED, HIGH);
      status=1;
      webSocket.sendTXT("LED1_ON_client");
    }

    if (message.indexOf("LED1_OFF")!=-1)
    {
      digitalWrite(LED, LOW);
      status=1;
      webSocket.sendTXT("LED1_OFF_client");
    }
    if (message.indexOf("LED2_ON")!=-1)
    {
      digitalWrite(led1, HIGH);
      status1=3;
      webSocket.sendTXT("LED2_ON_client");
    }

    if (message.indexOf("LED2_OFF")!=-1)
    {
      digitalWrite(led1, LOW);
      status1=4;
      webSocket.sendTXT("LED2_OFF_client");
    }
    if (message.indexOf("LED3_ON")!=-1)
    {
      digitalWrite(led2, HIGH);
      status2=5;
      webSocket.sendTXT("LED3_ON_client");
    }

    if (message.indexOf("LED3_OFF")!=-1)
    {
      digitalWrite(led2, LOW);
      status2=4;
      webSocket.sendTXT("LED3_OFF_client");
    }
    if (message.indexOf("LED4_ON")!=-1)
    {
      digitalWrite(led3, HIGH);
      status3=7;
      webSocket.sendTXT("LED4_ON_client");
    }

    if (message.indexOf("LED4_OFF")!=-1)
    {
      digitalWrite(led3, LOW);
      status3=6;
      webSocket.sendTXT("LED4_OFF_client");
    }
    }
    break;
  }
}

void sendToServer()
{
  jsonDoc["temperature"] = temperature; 
  jsonDoc["humidity"] = humidity;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  webSocket.sendTXT(jsonString);
}
/*void redbt()
{
  if(btserial.available())
  {
    incomingbt=btserial.read();
    if(incomingbt!='\n')
      messagebt+=incomingbt;
  }
  else
    messagebt="";
  if (messagebt == "1")
  {
    digitalWrite(LED, HIGH);
    status=1;
  }

  if (messagebt == "0")
  {
    digitalWrite(LED, LOW);
    status=0;
  }
  if (messagebt == "3")
    {
      digitalWrite(led1, HIGH);
      status1=3;
    }

    if (messagebt == "2")
    {
      digitalWrite(led1, LOW);
      status1=4;
    }
    if (messagebt == "5")
    {
      digitalWrite(led2, HIGH);
      status2=5;
    }

    if (messagebt == "4")
    {
      digitalWrite(led2, LOW);
      status2=4;
    }
    if (messagebt == "7")
    {
      digitalWrite(led3, HIGH);
      status3=7;
    }

    if (messagebt == "6")
    {
      digitalWrite(led3, LOW);
      status3=6;
    }
}*/
void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  //btserial.begin("ESP32test");
  Serial.print("Connecting to Wifi...");
  WiFiMulti.addAP(ssid, password);
  dht.begin();
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to Wifi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  //webSocket.beginSSL(websocket_server, websocket_port, url);
  //webSocket.begin(websocket_server, websocket_port, url);
  webSocket.beginSslWithCA(websocket_server, websocket_port, url, ENDPOINT_CA_CERT, "wss");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(1000);
}

void loop()
{
  if((millis()-timeloop) >= 1000)
  {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
    timeloop=millis();
    Serial.print("temp:");
    Serial.println(temperature);
    Serial.print("humi:");
    Serial.println(humidity);
    sendToServer();
  }
  webSocket.loop();
  //redbt();
}