#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <Adafruit_CCS811.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_VEML6070.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>
#include <SparkFunCCS811.h>
#include <SPI.h>
#include <Wire.h>

#define SSID "fireguard"
#define PASSWORD "123456789"

#define FIREBASE_HOST "fireguard-88888.firebaseio.com"
#define FIREBASE_AUTH "4SmRwyDRoxOhQKAGTwfBzZ8EZTcvq06ZASWP9nUK"

// Testing
#define DHT_TEST
#define CCS811_TEST
#define VEML6070_TEST
#define TCS35725_TEST
#define SOIL

// DHT11
#define DHT_PIN D3
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// CSS811 CO2 and TVOC
#define CSS811_ADDR 0x5B
CCS811 ccs811(CSS811_ADDR);

// VEML6070
Adafruit_VEML6070 uv = Adafruit_VEML6070();

// TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

// Soil moisture sensor
#define sense_pin 0
int MoistureLevel = 0;

void ConnectToWifi()
{
  delay(100);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(SSID);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
  would try to act as both a client and an access-point and could cause
  network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("Begin connecting to wifi");
  //start connecting to WiFi
  WiFi.begin(SSID, PASSWORD);
  //while client is not connected to WiFi keep loading
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

void GenerateTemperatureData(String FirebaseHeading, int lower, int upper)
{
  String tempValueID = Firebase.pushInt(FirebaseHeading + "/DHT11/temperature", random(lower, upper));
}
void GenerateHumidityData(String FirebaseHeading, int lower, int upper)
{
  String tempValueID = Firebase.pushInt(FirebaseHeading + "/DHT11/humidity", random(lower, upper));
}
void GenerateCO2Data(String FirebaseHeading, int lower, int upper)
{
  String tempValueID = Firebase.pushInt(FirebaseHeading + "/CCS811/CO2", random(lower, upper));
}
void GenerateTVOCData(String FirebaseHeading, int lower, int upper)
{
  String tempValueID = Firebase.pushInt(FirebaseHeading + "/CCS811/TVOC", random(lower, upper));
}
void GenerateSoilData(String FirebaseHeading, int lower, int upper)
{
  String tempValueID = Firebase.pushInt(FirebaseHeading + "/SN13322/SoilMoisture", random(lower, upper));
}
void GenerateColorTempData(String FirebaseHeading, int lower, int upper)
{
  String tempValueID = Firebase.pushInt(FirebaseHeading + "/TCS34725/ColorTemperature", random(lower, upper));
}
void GenerateLightIntensityData(String FirebaseHeading, int lower, int upper)
{
  String tempValueID = Firebase.pushInt(FirebaseHeading + "/TCS34725/LightIntensity", random(lower, upper));
}
void GenerateUVData(String FirebaseHeading, int lower, int upper)
{
  String tempValueID = Firebase.pushInt(FirebaseHeading + "/VEML6070/UV", random(lower, upper));
}

void setup() 
{
  Serial.begin(115200);
  Wire.begin();

  // ESP8266
  ConnectToWifi();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //DHT11
  dht.begin();
  pinMode(D0, OUTPUT);

  //CCS811
  ccs811.begin();

  // VEML6070
  uv.begin(VEML6070_1_T);

  // TCS34725
  while (!tcs.begin()) 
  {
    Serial.println("No TCS34725 found ... check your connections");
  } 
  if (tcs.begin())
  {
    Serial.println("Found sensor");
  }
}

void loop() 
{
  // Sample1 -> Warm, Dry, Bright
  GenerateTemperatureData("Sample1", 25, 40);
  GenerateHumidityData("Sample1", 10, 20);
  GenerateCO2Data("Sample1", 300, 500);
  GenerateTVOCData("Sample1", 200, 400);
  GenerateSoilData("Sample1", 0, 5);
  GenerateColorTempData("Sample1", 8000, 10000);
  GenerateLightIntensityData("Sample1", 2000, 3000);
  GenerateUVData("Sample1", 50000, 60000);
  )
#ifdef DHT_TEST
  // DHT11 Temperature and Humidity Sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("[INFO] Current Humidity = ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("[INFO] Current Temperature = ");
  Serial.print(t);
  Serial.println(" °C");
  
  // Eliminate abnormal values
  if ((t >= -15 && t <= 80) && (h >= 0 && h <= 100)) 
  {
    // === Push temperature value to Firebase ===
    String tempValueID = Firebase.pushFloat("Live/DHT11/temperature", t);
    if (Firebase.failed()) 
    {
        Serial.print("[ERROR] pushing /Live/DHT11/temperature failed:");
        Serial.println(Firebase.error());
        return;
    }

    Serial.print("[INFO] pushed: /Live/DHT11/temperature \tkey: ");
    Serial.println(tempValueID);

    // === Push humidity value to Firebase ===
    String humValueID = Firebase.pushFloat("Live/DHT11/humidity", h);
    if (Firebase.failed()) 
    {
        Serial.print("[ERROR] pushing /Live/DHT11/humidity failed:");
        Serial.println(Firebase.error());
        return;
    }

    Serial.print("[INFO] pushed: /Live/DHT11/humidity    \tkey: ");
    Serial.println(humValueID);
    Serial.println();
  } 
  else 
  {
      Serial.println("[ERROR] Wrong values!");
  }
#endif
#ifdef CCS811_TEST
  if (ccs811.dataAvailable())
  {
    ccs811.readAlgorithmResults();
    int tempCO2 = ccs811.getCO2();
    int tempTVOC = ccs811.getTVOC();
    Serial.print("[INFO] CO2: ");
    Serial.print(tempCO2);
    Serial.println();
    Serial.print("[INFO] TVOC: ");
    Serial.print(tempTVOC);
    Serial.println();

    // Push CO2 reading to Firebase
    String CO2ValueID = Firebase.pushFloat("Live/CCS811/CO2", tempCO2);
    if (Firebase.failed()) 
    {
        Serial.print("[ERROR] pushing /Live/CCS811/CO2 failed:");
        Serial.println(Firebase.error());
        return;
    }
    Serial.print("[INFO] pushed: /Live/CCS811/CO2 \tkey: ");
    Serial.println(CO2ValueID);

    // Push TVOC reading to Firebase
    String TVOCValueID = Firebase.pushFloat("Live/CCS811/TVOC", tempTVOC);
    if (Firebase.failed()) 
    {
        Serial.print("[ERROR] pushing /Live/CCS811/TVOC failed:");
        Serial.println(Firebase.error());
        return;
    }
    Serial.print("[INFO] pushed: /Live/CCS811/TVOC \tkey: ");
    Serial.println(TVOCValueID);
  }
  else if (ccs811.checkForStatusError())
  {
    Serial.print("[ERROR] Cannot print values");
    Serial.println();
  }
#endif
#ifdef VEML6070_TEST
  Serial.print("[INFO] UV light level: ");
  int UV = uv.readUV();
  Serial.print(UV);

  // Push UV index reading to Firebase
  String UVValueID = Firebase.pushFloat("Live/VEML6070/UV", UV);
  if (Firebase.failed()) 
  {
      Serial.print("[ERROR] pushing /Live/VEML6070/UV failed:");
      Serial.println(Firebase.error());
      return;
  }
  Serial.println();
  Serial.print("[INFO] pushed: /Live/VEML6070/UV \tkey: ");
  Serial.println(UVValueID);
#endif
#ifdef TCS35725_TEST
  uint16_t r, g, b, c, colorTemp, lux;
 
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);
  
  Serial.print("[INFO] Color Temp: "); Serial.print(colorTemp, DEC); Serial.println(" K");
  Serial.print("[INFO] Light Intensity: "); Serial.print(lux, DEC); Serial.println(" lx");
  Serial.print("[INFO] Red: "); Serial.println(r, DEC);
  Serial.print("[INFO] Green: "); Serial.println(g, DEC);
  Serial.print("[INFO] Blue: "); Serial.println(b, DEC);
  Serial.print("[INFO] Clear: "); Serial.println(c, DEC);
  Serial.println();

  String ColorTempID = Firebase.pushInt("Live/TCS34725/ColorTemperature", colorTemp);
  if (Firebase.failed()) 
  {
      Serial.print("[ERROR] pushing /Live/TCS34725/ColorTemperature failed:");
      Serial.println(Firebase.error());
      return;
  }
  Serial.print("[INFO] pushed: /Live/TCS34725/ColorTemperature \tkey: ");
  Serial.println(colorTemp);

  String LightIntensity = Firebase.pushInt("Live/TCS34725/LightIntensity", lux);
  if (Firebase.failed()) 
  {
      Serial.print("[ERROR] pushing /Live/TCS34725/ColorTemperature failed:");
      Serial.println(Firebase.error());
      return;
  }
  Serial.print("[INFO] pushed: /Live/TCS34725/ColorTemperature \tkey: ");
  Serial.println(lux);

  String Red = Firebase.pushInt("Live/TCS34725/Red", r);
  if (Firebase.failed()) 
  {
      Serial.print("[ERROR] pushing /Live/TCS34725/Red failed:");
      Serial.println(Firebase.error());
      return;
  }
  Serial.print("[INFO] pushed: /Live/TCS34725/Red \tkey: ");
  Serial.println(r);

  String Green = Firebase.pushInt("Live/TCS34725/Green", g);
  if (Firebase.failed()) 
  {
      Serial.print("[ERROR] pushing /Live/TCS34725/Green failed:");
      Serial.println(Firebase.error());
      return;
  }
  Serial.print("[INFO] pushed: /Live/TCS34725/Green \tkey: ");
  Serial.println(g);

  String Blue = Firebase.pushInt("Live/TCS34725/Blue", b);
  if (Firebase.failed()) 
  {
      Serial.print("[ERROR] pushing /Live/TCS34725/Red failed:");
      Serial.println(Firebase.error());
      return;
  }
  Serial.print("[INFO] pushed: /Live/TCS34725/Blue \tkey: ");
  Serial.println(b);

  String Clear = Firebase.pushInt("Live/TCS34725/Clear", c);
  if (Firebase.failed()) 
  {
      Serial.print("[ERROR] pushing /Live/TCS34725/Clear failed:");
      Serial.println(Firebase.error());
      return;
  }
  Serial.print("[INFO] pushed: /Live/TCS34725/Clear \tkey: ");
  Serial.println(c);
#endif
#ifdef SOIL
  Serial.print("[INFO] Soil Moisture Level: ");
  MoistureLevel = analogRead(sense_pin);
  MoistureLevel = MoistureLevel / 10;
  Serial.println(MoistureLevel);

  String SoilMoisture = Firebase.pushInt("Live/SEN13322/SoilMoisture", MoistureLevel);
  if (Firebase.failed()) 
  {
      Serial.print("[ERROR] pushing /Live/TSEN13322/SoilMoisture failed:");
      Serial.println(Firebase.error());
      return;
  }
  Serial.print("[INFO] pushed: /Live/SEN13322/SoilMoisture \tkey: ");
  Serial.println(MoistureLevel);
#endif
}