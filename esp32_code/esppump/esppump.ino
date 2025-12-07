#include <WiFi.h>
#include <HTTPClient.h>
const char* ssid     = "paitummuan";
const char* password = "1234567890";
#define waterpump 23
const char* serverUrl = "http://api.grouprproject.uk/api/data/fire";
void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  // put your setup code here, to run once:
  pinMode(waterpump, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(1000);
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    int fireResponseCode = http.GET();
  if (fireResponseCode > 0) {
      Serial.print("fire API response code: ");
      Serial.println(fireResponseCode);

      if (fireResponseCode > 0) {
        String firePayload = http.getString();
        firePayload.trim();
        Serial.println("fire payload: '" + firePayload + "'");

        // ตอนนี้ /api/data/light ส่ง 0 หรือ 1 ตรง ๆ
        int value = firePayload.toInt();     // "0" -> 0, "1" -> 1
        if(value == 1 ){
          digitalWrite(waterpump, HIGH);

        }
        else{
          digitalWrite(waterpump, LOW);
        }
    } else {
      Serial.print("fire API error code: ");
      Serial.println(fireResponseCode);
    }

}}}
