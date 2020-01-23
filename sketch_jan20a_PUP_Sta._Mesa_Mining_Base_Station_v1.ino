
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C dataUpdate(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
LiquidCrystal_I2C dataThreshold(0x23, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

SoftwareSerial mySerial(12, 11); // RX, TX
String humidity;
String temperature;
String heatIndex;
String airQuality;

int humThresh;
int temThresh;
int heatThresh;
int airThresh;

const int Select = A0;
int selectVal;
int selectMap;

const int Threshold = A1;
int thresholdVal;
int thresholdMap;

const int NC = A2;
const int lock = A3;
int lockVal;

const int NC1 = 8;
const int pushButton = 9;
int pushVal;

const int redLED = 10;

const int in1 = 4;
const int in2 = 5;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial.println("BEGIN");

  mySerial.begin(9600);
  mySerial.setTimeout(10);
  mySerial.println("BEGIN");

  dataUpdate.begin(20, 4);
  dataUpdate.backlight();
  dataThreshold.begin(20, 4);
  dataThreshold.backlight();

  pinMode(Select, INPUT);
  pinMode(Threshold, INPUT);

  pinMode(NC, OUTPUT);
  digitalWrite(NC, LOW);
  pinMode(lock, INPUT_PULLUP);

  pinMode(NC1, OUTPUT);
  digitalWrite(NC1, LOW);
  pinMode(pushButton, INPUT_PULLUP);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(redLED, OUTPUT);
}

void loop() {
  lockVal = digitalRead(lock);
  pushVal = digitalRead(pushButton);
  selectVal = analogRead(Select);
  thresholdVal = analogRead(Threshold);

  selectMap = map(selectVal, 0, 1000, 1, 4);
  thresholdMap = map(thresholdVal, 0, 1000, 1, 500);


  if (lockVal == HIGH) {
    dataThreshold.clear();
    THRESHOLD_SHOW();
  } else {
    dataThreshold.clear();
    THRESHOLD_SELECT();
  }

  decide();
  loraModule();

  delay(200);
}


void decide() {
  if (humThresh < humidity.toInt() || temThresh < temperature.toInt() || heatThresh < heatIndex.toInt() || airThresh < airQuality.toInt()) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}


void THRESHOLD_SHOW() {
  dataThreshold.setCursor(3, 0);
  dataThreshold.print("Humidity");
  dataThreshold.setCursor(3, 1);
  dataThreshold.print("Temperature");
  dataThreshold.setCursor(3, 2);
  dataThreshold.print("Heat Index");
  dataThreshold.setCursor(3, 3);
  dataThreshold.print("Air Quality");
  dataThreshold.print(String("  ") + thresholdMap);

  switch (selectMap) {
    case 1:
      Serial.println("Humidity");
      humThresh = thresholdMap;
      dataThreshold.setCursor(0, 0);
      dataThreshold.print("->");
      break;

    case 2:
      Serial.println("Temperature");
      temThresh = thresholdMap;
      dataThreshold.setCursor(0, 1);
      dataThreshold.print("->");
      break;

    case 3:
      Serial.println("Heat Index");
      heatThresh = thresholdMap;
      dataThreshold.setCursor(0, 2);
      dataThreshold.print("->");
      break;

    case 4:
      Serial.println("Air Quality");
      airThresh = thresholdMap;
      dataThreshold.setCursor(0, 3);
      dataThreshold.print("->");
      break;
  }

}

void THRESHOLD_SELECT() {
  dataThreshold.setCursor(0, 0);
  dataThreshold.print("Humidity: ");
  dataThreshold.print(humThresh);
  dataThreshold.setCursor(0, 1);
  dataThreshold.print("Temperature: ");
  dataThreshold.print(temThresh);
  dataThreshold.setCursor(0, 2);
  dataThreshold.print("Heat Index: ");
  dataThreshold.print(heatThresh);
  dataThreshold.setCursor(0, 3);
  dataThreshold.print("Air Quality: ");
  dataThreshold.print(airThresh);
}


void loraModule() {
  if (mySerial.available() > 0) {
    humidity = mySerial.readStringUntil(',');
    temperature = mySerial.readStringUntil(',');
    heatIndex = mySerial.readStringUntil(',');
    airQuality = mySerial.readStringUntil('#');

    Serial.println(String("Humidity: ") + humidity + String("%"));
    Serial.println(String("Temperature: ") + temperature + String("°C"));
    Serial.println(String("Heat Index: ") + heatIndex + String("°C"));
    Serial.println(String("MQ Sensor Value: ") + airQuality);

    dataUpdate.clear();
    dataUpdate.setCursor(0, 0);
    dataUpdate.print(String("Humidity: ") + humidity + String("%"));
    dataUpdate.setCursor(0, 1);
    dataUpdate.print(String("Temperature: ") + temperature + String(" C"));
    dataUpdate.setCursor(0, 2);
    dataUpdate.print(String("Heat Index: ") + heatIndex + String(" C"));
    dataUpdate.setCursor(0, 3);
    dataUpdate.print(String("Air Quality: ") + airQuality);
    digitalWrite(redLED, HIGH);
    delay(10);
    digitalWrite(redLED, LOW);
    delay(10);
  }
}
