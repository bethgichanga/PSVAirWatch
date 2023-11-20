#include <DHT.h>
//#include <Adafruit_ESP8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

#define DHTPIN D2 // DHT11 is connected to GPIO Pin 2
#define MQ_PIN A0  // MQ-2 Sensor is connected to Analog Pin A0

String apiKey = "XXXXXXXXXXX"; // Enter your API key from your ThingSpeak 
const char *ssid = "XXXXXXX"; // Enter your WiFi Network's SSID
const char *pass = "XXXXXXXXX"; // Enter your WiFi Network's Password
const char *server = "api.thingspeak.com";

int humi;
int temp;
float Level; // MQ-2

DHT dht(DHTPIN, DHT11);
WiFiClient client;

void connectToWiFi() {
  Serial.println("Connecting to WiFi: " + String(ssid));
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  Serial.println("\n*** WiFi connected ***");
}

void sendDHTData() {
  humi = dht.readHumidity();
  temp = dht.readTemperature();

  if (client.connect(server, 80)) {
    String sendData = apiKey + "&field1=" + String(temp) + "&field2=" + String(humi) + "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(sendData.length());
    client.print("\n\n");
    client.print(sendData);

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print("°C. Humidity: ");
    Serial.print(humi);
    Serial.println("%.");
  }

  client.stop();
}

void sendMQ2Data() {
  float reading = analogRead(MQ_PIN);
  Level = (reading / 1023) * 100;

  if (isnan(Level)) {
    Serial.println("Failed to read from MQ-2 sensor!");
    return;
  }

  if (client.connect(server, 80)) {
    String postStr = apiKey + "&field3=" + String(Level) + "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("CO2 Level: ");
    Serial.println(Level);
    //Serial.println("Sending MQ-2 data to Thingspeak.");
  }
  client.stop();
}

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  connectToWiFi();
}

void loop() {
  sendDHTData();
  sendMQ2Data();
  delay(1500); // Thingspeak needs a minimum 1.5 seconds delay between updates
}