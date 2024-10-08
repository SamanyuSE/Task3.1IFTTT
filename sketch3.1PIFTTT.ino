#include <WiFi.h>          
#include <WiFiClient.h>    
#include <BH1750.h>        
#include <Wire.h>          

const char* ssid = "oplus_co_apdmkd";  
const char* password = "tohm3279";      

const char* ifttt_host = "maker.ifttt.com";  
const String ifttt_event = "light";         
const String ifttt_key = "inqJ1eTVkSbf9gWPjAXyeH6tERzHNkVNOkkSjEDRPDp";
// Variables to track light intensity and previous state
bool prev = false;  // Previous state of sunlight condition
bool new1 = false;  // Current state of sunlight condition

float sun_intensity; // Variable to store the sunlight intensity
BH1750 sensor;       // Create an instance of the BH1750 light sensor


void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud rate
  
  Wire.begin(); // Initialize I2C communication
  sensor.begin(); // Initialize the BH1750 sensor

  // Connect to WiFi
  WiFi.begin(ssid, password); // Start WiFi connection
  Serial.print("Connecting to WiFi");
  
  // Wait for the connection to succeed
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Wait for 500 milliseconds
    Serial.print("."); // Print a dot to indicate progress
  }
  Serial.println(" Connected!"); // Print when connected
}

void loop() {
  // Read the light level from the sensor
  sun_intensity = sensor.readLightLevel();
  
  // Check if the light intensity is greater than 400 lx
  if (sun_intensity > 400) {
    new1 = true; // Update current state to sufficient sunlight
    Serial.print("Light intensity: ");
    Serial.print(sun_intensity); // Print the light intensity
    Serial.println(" lx - Sufficient sunlight!");
    
    // Trigger IFTTT webhook if the state has changed
    if(prev != new1){
      triggerIFTTTWebhook(sun_intensity);
    }
    prev = new1; // Update previous state

  } else {
    new1 = false; // Update current state to insufficient sunlight
    Serial.print("Light intensity: ");
    Serial.print(sun_intensity); // Print the light intensity
    Serial.println(" lx - Insufficient sunlight!");

    // Trigger IFTTT webhook if the state has changed
    if(prev != new1){
      triggerIFTTTWebhook(sun_intensity);
    }
    prev = new1; // Update previous state
  }

  delay(10000); // Wait for 10 seconds before the next reading
}

void triggerIFTTTWebhook(float sensor_value) {
  WiFiClient client; // Create a WiFi client object
  // Construct the URL for the IFTTT webhook
  String url = "/trigger/" + ifttt_event + "/with/key/" + ifttt_key + "?value1=" + String(sensor_value);

  // Connect to the IFTTT server
  if (client.connect(ifttt_host, 80)) {
    Serial.println("Connecting to IFTTT..."); // Print connection message

    // Send HTTP GET request to IFTTT webhook
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + String(ifttt_host)); // Set the host
    client.println("Connection: close"); // Close the connection after response
    client.println(); // End the request

    // Read the response from the IFTTT server
    while (client.connected() || client.available()) {
      if (client.available()) {
        String line = client.readStringUntil('\n'); // Read the response line
      
      }
    }

    client.stop(); // Stop the client connection
    Serial.println("IFTTT Webhook triggered."); // Print confirmation message
  } else {
    Serial.println("Failed to connect to IFTTT."); // Print error message if connection fails
  }
}
