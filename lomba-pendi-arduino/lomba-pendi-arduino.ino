// Library yang diperlukan
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

// Pin Relay
const int relayPins[] = { 30, 25, 32, 22, 26, 23, 33, 27, 28, 32, 22, 26, 23, 33, 27, 29 };
const int numRelays = sizeof(relayPins) / sizeof(relayPins[0]);

const int resetPin = 4;
const int allRelayPin = 5;

// Waktu Delay
// const unsigned long relayDelays[] = {80000 , 80000, 120000, 80000, 180000, 80000, 120000, 80000, 80000, 120000, 80000, 180000, 80000, 120000, 80000, 80000};
const unsigned long relayDelays[] = {8000 , 8000, 12000, 8000, 18000, 8000, 12000, 8000, 8000, 12000, 8000, 18000, 8000, 12000, 8000, 8000};
// Sensor TDS dan Suhu DS18B20
const int tdsTank1Pin = A0;   // sensor suhu DS18B20 tangki 5 putih
const int tdsTank5Pin = A1;   // sensor suhu DS18B20 tangki 1 ungu
const int tempTank1Pin = A2;  // sensor suhu DS18B20 tangki 5 biru
const int tempTank5Pin = A3;  // sensor suhu DS18B20 tangki 1 hijau
const int phTank1Pin = A5;    // sensor ph tangki 1
const int phTank5Pin = A4;    // sensor ph tangki 5

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// OneWire dan DallasTemperature
OneWire oneWire1(tempTank1Pin);
OneWire oneWire5(tempTank5Pin);
DallasTemperature sensors1(&oneWire1);
DallasTemperature sensors5(&oneWire5);
SoftwareSerial espSerial(2, 3);
float readph(int pin) {
  // Membaca nilai pH
  float analogValue = analogRead(pin);
  return analogValue;
}

void setup() {
  // Set semua pin relay sebagai OUTPUT
  for (int i = 0; i < numRelays; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);  // Matikan relay awalnya
  }

  // Inisialisasi komunikasi Serial
  Serial.begin(9600);
  // Serial2.begin(115200);
  espSerial.begin(115200);

  // Inisialisasi LCD
  lcd.begin();
  // Inisialisasi sensor suhu DS18B20
  sensors1.begin();
  sensors5.begin();
  // Inisialisasi sensor pH
  pinMode(phTank1Pin, INPUT);
  pinMode(phTank5Pin, INPUT);

  pinMode(resetPin, INPUT_PULLUP);
  pinMode(allRelayPin, INPUT_PULLUP);
}

void loop() {
  float ph1 = readph(phTank1Pin);
  float phValue1 = map(ph1, 0, 1023, 0, 18.5);
  float ph5 = readph(phTank5Pin);
  float phValue5 = map(ph5, 0, 1023, 0, 20);

  float tdsTank1 = readTDS(tdsTank1Pin);
  float tdsTank5 = readTDS(tdsTank5Pin);
  float tempTank1 = readTemperature(tempTank1Pin, sensors1);
  float tempTank5 = readTemperature(tempTank5Pin, sensors5);

  float jumlahProses = 0;

  if (espSerial.available() == 0) {
    String sdata;
    sdata = sdata + tdsTank1 + "," + tdsTank5 + "," + tempTank1 + "," + tempTank5 + "," + phValue1 + "," + phValue5 + "," + jumlahProses;
    espSerial.println(sdata);
    Serial.println(sdata);
    sdata = "";
  }

  Serial.print("TDS Tangki 1: ");
  Serial.println(tdsTank1);
  Serial.print("TDS Tangki 5: ");
  Serial.println(tdsTank5);
  Serial.print("Suhu Tangki 1: ");
  Serial.println(tempTank1);
  Serial.print("Suhu Tangki 5: ");
  Serial.println(tempTank5);
  Serial.print("pH Tangki 1: ");
  Serial.println(phValue1);
  Serial.print("pH Tangki 5: ");
  Serial.println(phValue5);

  // Tampilkan hasil di LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tangki 1");
  lcd.setCursor(0, 1);
  lcd.print("TDS1=");
  lcd.print(tdsTank1);
  lcd.setCursor(0, 2);
  lcd.print("SH1=");
  lcd.print(tempTank1);
  lcd.setCursor(0, 3);
  lcd.print("pH1=");
  lcd.print(phValue1);

  lcd.setCursor(11, 0);
  lcd.print("Tangki 5");
  lcd.setCursor(11, 1);
  lcd.print("TDS5=");
  lcd.print(tdsTank5);
  lcd.setCursor(11, 2);
  lcd.print("SH5=");
  lcd.print(tempTank5);
  lcd.setCursor(11, 3);
  lcd.print("pH5=");
  lcd.print(phValue5);
  delay(1000);

  mulai_proses:
  // Mengaktifkan relay sesuai urutan dan waktu kerja
  for (int i = 0; i < numRelays; i++) {
    jumlahProses = ((String(i).toFloat() + 1)/numRelays) * 100; 
    digitalWrite(relayPins[i], LOW);  // Aktifkan relay
    ph1 = readph(phTank1Pin);
    phValue1 = map(ph1, 0, 1023, 0, 18.5);
    ph5 = readph(phTank5Pin);
    phValue5 = map(ph5, 0, 1023, 0, 20);

    // Menampilkan nomor relay yang aktif di Serial Monitor
    Serial.print("Tahap yang sedang aktif: ");
    Serial.println(i + 1);
    tdsTank1 = readTDS(tdsTank1Pin);
    tdsTank5 = readTDS(tdsTank5Pin);
    tempTank1 = readTemperature(tempTank1Pin, sensors1);
    tempTank5 = readTemperature(tempTank5Pin, sensors5);

    if (espSerial.available() == 0) {
      String sdata;
      sdata = sdata + tdsTank1 + "," + tdsTank5 + "," + tempTank1 + "," + tempTank5 + "," + phValue1 + "," + phValue5 + "," + jumlahProses;
      espSerial.println(sdata);
      Serial.println(sdata);
      sdata = "";
    }

    Serial.print("TDS Tangki 1: ");
    Serial.println(tdsTank1);
    Serial.print("TDS Tangki 5: ");
    Serial.println(tdsTank5);
    Serial.print("Suhu Tangki 1: ");
    Serial.println(tempTank1);
    Serial.print("Suhu Tangki 5: ");
    Serial.println(tempTank5);
    Serial.print("pH Tangki 1: ");
    Serial.println(phValue1);
    Serial.print("pH Tangki 5: ");
    Serial.println(phValue5);

    // Tampilkan hasil di LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tangki 1");
    lcd.setCursor(0, 1);
    lcd.print("TDS1=");
    lcd.print(tdsTank1);
    lcd.setCursor(0, 2);
    lcd.print("SH1=");
    lcd.print(tempTank1);
    lcd.setCursor(0, 3);
    lcd.print("pH1=");
    lcd.print(phValue1);
  
    lcd.setCursor(11, 0);
    lcd.print("Tangki 5");
    lcd.setCursor(11, 1);
    lcd.print("TDS5=");
    lcd.print(tdsTank5);
    lcd.setCursor(11, 2);
    lcd.print("SH5=");
    lcd.print(tempTank5);
    lcd.setCursor(11, 3);
    lcd.print("pH5=");
    lcd.print(phValue5);
    delay(1000);


    // Delay sesuai waktu kerja relay
    delay(relayDelays[i]);

    digitalWrite(relayPins[i], HIGH);  // Matikan relay

    // Menampilkan informasi relay non-aktif di Serial Monitor
    Serial.print("Tahap yang sudah non-aktif: ");
    Serial.println((i + 1) % numRelays + 1);

    if (digitalRead(resetPin) == LOW) {
      Serial.println("Arduino direset!");

      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Arduino Direset!");

      while (digitalRead(resetPin) == LOW) {} 
      delay(1000); 
      resetArduino(); 
    }

    if (digitalRead(allRelayPin) == LOW) {
      Serial.println("Proses Kuras!");
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Proses Kuras!");
      while (digitalRead(allRelayPin) == LOW) {
        for (int i = 1; i < numRelays; i++) {
          digitalWrite(relayPins[i], LOW);
        }
      }
      delay(1000);
      for (int i = 0; i < numRelays; i++) {
        digitalWrite(relayPins[i], HIGH);
      }
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Semua Relay Mati");
      delay(5000);

      goto mulai_proses;
    }
  }
}

// Fungsi untuk membaca nilai TDS
float readTDS(int pin) {
  // Mengukur nilai TDS
  int analogValue = analogRead(pin);
  // Melakukan perhitungan untuk mengonversi nilai analog ke nilai TDS
  float tdsValue = map(analogValue, 0, 1023, 0, 1000);
  return tdsValue;
}

// Fungsi untuk membaca suhu menggunakan sensor DS18B20
float readTemperature(int pin, DallasTemperature &sensors) {
  sensors.requestTemperatures();                 // Meminta pembacaan suhu
  float tempValue = sensors.getTempCByIndex(0);  // Membaca suhu dalam derajat Celsius
  return tempValue;
}

void resetArduino() {
  asm volatile ("  jmp 0");
}


