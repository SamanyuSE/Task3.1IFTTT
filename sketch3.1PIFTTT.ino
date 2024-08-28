#include <WiFi.h>
#include <WiFiClient.h>
#include <BH1750.h>
#include <Wire.h>

const char* SSID = "Samanyu's iPhone";
const char* PASSWORD = "IAMSATAN";


const char* ifttt_host = "maker.ifttt.com";
const String ifttt_event = "Task 3.1P Samanyu Sharma";
const String ifttt_key = "tiO8YLtUvseOZ5vbaUkJk";

bool prev = false;
bool new1 = false;

float Light_intensity;
BH1750 sensor;


void setup() {
  
  Serial.begin(9600);
  
  Wire.begin();
  
  sensor.begin();

  // Connect to WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void loop() {
  
  Light_intensity = sensor.readLightLevel();
  
  if (Light_intensity > 400) {
    new1 = true;
    Serial.print("Light intensity is: ");
    Serial.print(Light_intensity);
    Serial.println(" lx - Sufficient sunlight!");
    
    if(prev != new1){
      triggerIFTTTWebhook(Light_intensity);
    }
    prev = new1;

  } 
  else {
    new1 = false;
    Serial.print("Light intensity is: ");
    Serial.print(Light_intensity);
    Serial.println(" lx - Insufficient sunlight!");

    // Trigger IFTTT Webhook
    if(prev != new1){
      triggerIFTTTWebhook(Light_intensity);
    }
    prev = new1;
  }

  delay(10000);
}

void triggerIFTTTWebhook(float sensor_value) {
  WiFiClient client;
  String url = "/trigger/" + ifttt_event + "/with/key/" + ifttt_key + "?value1=" + String(sensor_value);

  if (client.connect(ifttt_host, 80)) {
    Serial.println("Connecting to IFTTT...");

    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(ifttt_host));
    client.println("Connection: close");
    client.println();

    while (client.connected() || client.available()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        //Serial.println(line);
      }
    }

    client.stop();
    Serial.println("IFTTT Webhook triggered.");
  } else {
    Serial.println("Failed to connect to IFTTT.");
  }
}