#define BLYNK_TEMPLATE_ID "TMPL6_Xsaccn1"
#define BLYNK_TEMPLATE_NAME "Watering System"
#define BLYNK_AUTH_TOKEN "EueOS80dBNTM0gaNp6HlHflDtbNL6ZjD"

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

#include "DHT.h"
#define DHTPIN 4 
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

const int IO = 27;    
const int SCLK = 14;  
const int CE = 26;  

int S1 = 23;

ThreeWire myWire(IO, SCLK, CE);
RtcDS1302<ThreeWire> Rtc(myWire);

char auth[] = "EueOS80dBNTM0gaNp6HlHflDtbNL6ZjD";
char ssid[] = "3BB-Fiber_Jantaratana_2.4GHz";
char pass[] = "0950398583";

// Global variable for the date string
char datestring[20];
float h;
float t;
float f;
float hif;
float hic;

void setup() {

  Serial.begin(9600);

  Serial.println("Connecting to Wi-Fi...");
  Blynk.begin(auth, ssid, pass);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi connected.");
  } else {
    Serial.println("Wi-Fi connection failed.");
  }

  Serial.println(F("DHT22 test!"));
  dht.begin();

  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }

  pinMode(S1, OUTPUT);
}

void loop() {

  Blynk.run();

  dht22_work();

  RtcDateTime now = Rtc.GetDateTime();

  printDateTime(now);
  Serial.println();

  if (!now.IsValid()) {
    Serial.println("RTC lost confidence in the DateTime!");
  }

  // ช่วงเวลา 6:00 - 6:20 น.
  if (now.Hour() == 6 && now.Minute() >= 0 && now.Minute() < 20) {
    digitalWrite(S1, HIGH);  // เปิด relay
    Blynk.virtualWrite(V5, "WORKING"); 
  } 
  // ช่วงเวลา 16:00 - 16:20 น.
  else if (now.Hour() == 16 && now.Minute() >= 0 && now.Minute() < 20) {
    digitalWrite(S1, HIGH);  // เปิด relay
    Blynk.virtualWrite(V5, "WORKING"); 
  } 
  // นอกช่วงเวลา relay ปิด
  else {
    digitalWrite(S1, LOW);   // ปิด relay
    Blynk.virtualWrite(V5, "NOT WORKING"); 
  }


  Blynk.virtualWrite(V1, datestring); 
  Blynk.virtualWrite(V2, t); 
  Blynk.virtualWrite(V3, h); 


  delay(1000); 
}

void printDateTime(const RtcDateTime& dt) {

  // Use the global datestring variable
  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
}

void dht22_work(){

  h = dht.readHumidity(); // Humidity
  t = dht.readTemperature(); // Temperature in Celsius
  f = dht.readTemperature(true); // Temperature in Fahrenheit

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  hif = dht.computeHeatIndex(f, h); 
  hic = dht.computeHeatIndex(t, h, false); 
}
