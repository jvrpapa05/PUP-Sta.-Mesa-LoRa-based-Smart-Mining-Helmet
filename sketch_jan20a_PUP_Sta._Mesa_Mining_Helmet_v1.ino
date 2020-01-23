#include <SoftwareSerial.h>
SoftwareSerial mySerial(12, 11); // RX, TX
String data;
int count;

#include "DHT.h"
#define DHTPIN 6     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
float h;
float t;
float hic;

const int mqSensor = A0;
int mqVal;

const int vibratorMotor = 4;

const int irSensor = 5;
int irVal;

const int greenLED = 7;
const int buzzer = 13;


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial.println("BEGIN");

  mySerial.begin(9600);
  mySerial.setTimeout(10);
  mySerial.println("BEGIN");

  dht.begin();
  pinMode(mqSensor, INPUT);
  pinMode(vibratorMotor, OUTPUT);
  pinMode(irSensor, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Notify();
}

void loop() {
  delay(2000);
  dhtSensor();
  allSensor();
  protection();
  ledBlink();
  loraModule();

  Serial.println(h + String(",") + t + String(",") + hic + String(",") + mqVal + String("#"));
  mySerial.print(h + String(",") + t + String(",") + hic + String(",") + mqVal + String("#"));
}


void dhtSensor() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  hic = dht.computeHeatIndex(t, h, false);

  Serial.println(String("Humidity: ") + h + String("%"));
  Serial.println(String("Temperature: ") + t + String("°C"));
  Serial.println(String("Heat Index: ") + hic + String("°C"));
}


void allSensor() {
  mqVal = analogRead(mqSensor);
  Serial.println(String("MQ Sensor Value: ") + mqVal);
}

void protection() {
  irVal = digitalRead(irSensor);
  Serial.println(String("Infrared Sensor: ") + irVal);

  while (irVal == LOW) {
    irVal = digitalRead(irSensor);
    alarm();
  }
}

void alarm() {
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(100);
}

void Notify() {
  digitalWrite(vibratorMotor, HIGH);
  delay(2000);
  digitalWrite(vibratorMotor, LOW);
}

void ledBlink() {
  digitalWrite(greenLED, HIGH);
  delay(100);
  digitalWrite(greenLED, LOW);
  delay(100);
}


void loraModule() {
  if (mySerial.available() > 0) {
    data = mySerial.readString();
  }

  if (Serial.available() > 0) {
    data = Serial.readString();
  }

  Serial.println(data);
}
