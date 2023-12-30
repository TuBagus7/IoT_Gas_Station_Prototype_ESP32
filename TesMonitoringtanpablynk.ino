#define BLYNK_TEMPLATE_ID "TMPL6w893ConK"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "hNC6dSkTXVSTiaBVuMuQZJbEyvOPYPrL"

#include <Blynk.h>
#include <Wire.h>
#include <WiFi.h>
#include <Preferences.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

char auth[] = BLYNK_AUTH_TOKEN;

#define TRIGGER_PIN 4  // HC-SR04 trigger pin
#define ECHO_PIN 5     // HC-SR04 echo pin
#define DHT_PIN 18     // DHT22 data pin
#define RED_LED 12     // Red LED pin
#define YELLOW_LED 14  // Yellow LED pin
#define GREEN_LED 27   // Green LED pin

float volume;
const float maxlength = 18.8;
const float radius = 9;
const float phi = 3.14;
const int i2cAddress = 0x27;


//Definisikan Blynk dengan pin Sensor
#define VPIN_Temperature V0
#define VPIN_Humidity V1
#define VPIN_Volume V2
#define VPIN_ledr V3
#define VPIN_ledy V4
#define VPIN_ledg V5


DHT dht(DHT_PIN, DHT22);
BlynkTimer timer;
LiquidCrystal_I2C lcd(i2cAddress, 16, 2);  // Change the dimensions if your LCD is different

void checkBlynkStatus()
{ // called every 2 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false)
  {
    Serial.println("Blynk Not Connected");
  }
  if (isconnected == true)
  {
    Serial.println(" Blynk IoT Connected ");
  }
  // display.display();
//  delay(1000);
}
void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.begin();          // Initialize LCD with I2C address
  Wire.begin(21, 22);  //SDA 21 SCL 22
  lcd.backlight();

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Blynk.config(auth);
  const char *ssid = "Redmi Note 10 Pro";
  const char *password = "1sampai7";
  Blynk.begin(auth, ssid, password, "Blynk.cloud", 80);
  
}
float getUltrasonicDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  //return pulseIn(ECHO_PIN, HIGH) * 0.0343 / 2.0;  // Calculate distance in cm
  int duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2;
  timer.setInterval(2000L, checkBlynkStatus);
}

float tinggi_sebelumnya = 19;
bool keran_dibuka = false;

void loop() {
  Blynk.run();
  float length = getUltrasonicDistance();
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // fix nilai sensor
  if (length >= maxlength) {
    length = maxlength;
  }

  // hitung ketinggian air
  float tinggi_sekarang = maxlength - length;

  // cek kondisi keran
  if (tinggi_sekarang != tinggi_sebelumnya) {
    keran_dibuka = true;
  } else {
    keran_dibuka = false;
  }

  float selisih = tinggi_sebelumnya - tinggi_sekarang;
  // if (selisih < 0) {
  //   selisih = selisih * -1;
  // }

  Serial.println(selisih);

  if (selisih >= 0.4 && keran_dibuka == true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Calculating");
    lcd.setCursor(0, 1);
    for (int i = 0; i < 8; i++) {
      lcd.print(". ");
      delay(625);
    }
    volume = (phi * (radius * radius) * tinggi_sekarang) / 1000;
  } else {
    if (selisih == 0) {
      volume = (phi * (radius * radius) * tinggi_sekarang) / 1000;
    }

    Serial.print("Temp: ");
    Serial.println(temperature);
    Serial.print("Hum: ");
    Serial.println(humidity);
    Serial.print("Vol: ");
    Serial.println(volume);

    Blynk.virtualWrite(VPIN_Temperature, temperature);
    Blynk.virtualWrite(VPIN_Humidity, humidity);
    Blynk.virtualWrite(VPIN_Temperature, temperature);
    Blynk.virtualWrite(VPIN_Volume, volume);

    lcd.clear();
    lcd.print("T:");
    lcd.print(temperature);
    lcd.print(" H:");
    lcd.print(humidity);
    lcd.setCursor(0, 1);
    lcd.print("Vol:" + String(volume) + " Liter");

  if (volume <= 1.50) {
      digitalWrite(RED_LED, HIGH);
      digitalWrite(YELLOW_LED, LOW);
      digitalWrite(GREEN_LED, LOW);

      Blynk.virtualWrite(VPIN_ledr, 1);
      Blynk.virtualWrite(VPIN_ledy, 0);
      Blynk.virtualWrite(VPIN_ledg, 0);
 }else if (volume >= 1.51 && volume <= 3.00) {
      digitalWrite(YELLOW_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      digitalWrite(GREEN_LED, LOW);

      Blynk.virtualWrite(VPIN_ledr, 0);
      Blynk.virtualWrite(VPIN_ledy, 1);
      Blynk.virtualWrite(VPIN_ledg, 0);
 }else if (volume >= 3.01 && volume <= 4.80) {
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, LOW);
      digitalWrite(YELLOW_LED, LOW);

      Blynk.virtualWrite(VPIN_ledr, 0);
      Blynk.virtualWrite(VPIN_ledy, 0);
      Blynk.virtualWrite(VPIN_ledg, 1);
    }

    delay(2000);
  }

  // ambil nilai terkecil
  if (tinggi_sekarang < tinggi_sebelumnya || selisih <= -0.1 ) {
    tinggi_sebelumnya = tinggi_sekarang;
  }
}
