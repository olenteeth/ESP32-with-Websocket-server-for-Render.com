/*
Adafruit sensor 1.1.14
DHT sensor library by Adafruit 1.4.6
WebSockets by Markus Sattler 2.4.1
ArduinoJson by Benoit Blanchon 7.0.4
The remaining libraries are buit-in libraries
*/

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
const char *websocket_server = "web-ui-ig72.onrender.com"; //link web deploy with render 
const int websocket_port = 443; //80 for http, 443 for https
const char *url = "/";
int status=0, status1=2, status2=4, status3=6, temp=0;

DynamicJsonDocument jsonDoc(128);
float temperature, humidity;
unsigned long timeloop=0;

//use cmd
//openssl s_client -showcerts -connect <link web>:443
// CA-pass here is example of Websocket library
const char ENDPOINT_CA_CERT[] PROGMEM = R"EOF(
----BEGIN CERTIFICATE-----
MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT
DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0NlowSjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT
GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF
q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0
Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWAa6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj
/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0TAQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG
CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNvbTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k
c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAwVAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC
ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAzMDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu
Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsFAAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo
uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu
X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlGPfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6
KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==
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
