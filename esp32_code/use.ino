#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h" 
#include <math.h>

// The respective input pin can be declared here 
#define DHTPIN 4    
#define led 23  
int firePin = 5;
int lightPin = 32;
int temPin = 35;
// The sensor is initialized 
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE); 
const char* ssid     = "paitummuan";
const char* password = "1234567890";

// change to your server IP / domain
const char* serverUrl = "http://grouprproject.uk/api/data";
const char* ledurl = "http://grouprproject.uk/api/data/light";
double Thermister(int RawADC) {
  double Temp,Vo;
  Vo=float(RawADC)*(3.3/4095);
  Temp = log(10000*(3.3/Vo-1));
  //Temp = log(((10240000/RawADC) - 10000));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;// Convert Kelvin to Celcius
  return Temp;
}


void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  dht.begin();
  pinMode(firePin, INPUT);
  pinMode(led, OUTPUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    HTTPClient httpled;

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    
    // Example data (replace with your real sensor data)
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int digitalVal = digitalRead(firePin); 
    float lightvalue = analogRead(lightPin);
    float voltage = lightvalue * (5.0/1023) * 1000;
    int lightPct   = map(lightvalue, 0, 4095, 0, 100); 
    int readVal = analogRead(temPin);
    double temp = Thermister(readVal);

    

    String jsonData = "{";
    jsonData += "\"temperature\":" + String(temperature, 2) + ",";
    jsonData += "\"humidity\":" + String(humidity, 2) + ",";
    jsonData += "\"have_fire\":" + String(digitalVal)+",";
    jsonData += "\"light_level\":" + String(lightvalue, 2)+",";
    jsonData += "\"temperature2\":" + String(temp, 2);
    jsonData += "}";

    int httpResponseCode = http.POST(jsonData);
    Serial.println(jsonData);
    if (httpResponseCode > 0) {
      Serial.print("Server response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println("Response: " + payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
    httpled.begin(ledurl);
    int ledResponseCode = httpled.GET();

    if (ledResponseCode > 0) {
      Serial.print("LED API response code: ");
      Serial.println(ledResponseCode);

      if (ledResponseCode > 0) {
        String ledPayload = httpled.getString();
        ledPayload.trim();
        Serial.println("LED payload: '" + ledPayload + "'");

        // ตอนนี้ /api/data/light ส่ง 0 หรือ 1 ตรง ๆ
        int value = ledPayload.toInt();     // "0" -> 0, "1" -> 1
        if(value == 1 ){
          digitalWrite(led, HIGH);

      }
      else{
        digitalWrite(led, LOW);
      }
    } else {
      Serial.print("LED API error code: ");
      Serial.println(ledResponseCode);
    }

    httpled.end();

  delay(1000);  // send every 5 seconds (adjust as needed)
}}}
