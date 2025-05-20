#include <LiquidCrystal.h>

// LCD pin bağlantıları
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

// Pin tanımlamaları
const int motorBtn = 2;
const int kemerBtn = 3;
const int kapiAnahtari = 4;

const int ldrPin = A1;
const int lm35Pin = A0;
const int potYakit = A2;

const int ledFar = 30;    // Mavi LED (Far)
const int ledKemer = 29;  // Kırmızı LED (Emniyet kemeri)
const int ledYakit = 31;  // Sarı LED (Yakıt az)

const int motor = 11;       // Araba motoru
const int klimaMotor = 12;  // Klima motoru

const int rgbRed = 8;   // Pembe LED için kırmızı kanal
const int rgbBlue = 9;  // Pembe LED için mavi kanal

const int buzzer = 10;

// Durum değişkenleri
bool motorDurumu = false;
unsigned long oncekiZaman = 0;
bool ledDurumu = false;
bool farOpen = false;

// Yakıt bilgisi gösterimi için zamanlama
unsigned long yakitGosterimZamani = 0;
const long yakitGosterimSuresi = 5000;  // 5 saniye yakıt gösterilecek
bool yakitGosterimAktif = true;

void setup() {
  // Girişler
  pinMode(motorBtn, INPUT);
  pinMode(kemerBtn, INPUT);
  pinMode(kapiAnahtari, INPUT);

  // Çıkışlar
  pinMode(ledFar, OUTPUT);
  pinMode(ledKemer, OUTPUT);
  pinMode(ledYakit, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(klimaMotor, OUTPUT);
  pinMode(rgbRed, OUTPUT);
  pinMode(rgbBlue, OUTPUT);
  pinMode(buzzer, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Sistem Basladi");
  delay(1000);
  lcd.clear();

  analogWrite(rgbRed, 50);
  analogWrite(rgbBlue, 50);

  // İlk başlangıçta yakıt gösterimi başlat
  yakitGosterimZamani = millis();
}

void loop() {
  // Sensör değerlerini oku
  int sicaklikADC = analogRead(lm35Pin);
  // Sıcaklık hesaplama
  float derece = (sicaklikADC * 500.0) / 1023.0;  // 5V için: 10mV/C ve (5/1023)=0.004887V/adım

  int yakitRaw = analogRead(potYakit);
  float yakitYuzde = yakitRaw * 100.0 / 1023.0;

  int ldrDegeri = analogRead(ldrPin);

  bool kemerTakili = digitalRead(kemerBtn);
  bool motorButonuBasildi = digitalRead(motorBtn);
  bool kapiKapali = digitalRead(kapiAnahtari);

  // Motor butonuna basılma durumunu kontrol et
  if (motorButonuBasildi && !motorDurumu) {
    if (!kemerTakili) {
      motorDurumu = false;
      digitalWrite(motor, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Kemer Takili");
      lcd.setCursor(0, 1);
      lcd.print("Degil!");
      digitalWrite(ledKemer, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(1000);  
      digitalWrite(ledKemer, LOW);
      digitalWrite(buzzer, LOW);
    } else if (kapiKapali && yakitYuzde > 0) {
      motorDurumu = true;
      yakitGosterimAktif = true;
      yakitGosterimZamani = millis();
    }
  }

  // LCD'yi her döngüde temizle
  lcd.clear();

  // Yakıt gösterim süresini kontrolu
  if (yakitGosterimAktif && (millis() - yakitGosterimZamani > yakitGosterimSuresi)) {
    yakitGosterimAktif = false;
  }

  // LCD'nin ilk satırı - Far durumu
  lcd.setCursor(0, 0);
  int ldrDeger = analogRead(ldrPin);
  if (ldrDeger <= 250) {
    digitalWrite(ledFar, HIGH);  // Far açık (mavi LED yanıyor)
    lcd.print("Far:ACIK ");
    farOpen = true;
  } else {
    digitalWrite(ledFar, LOW);  // Far kapalı (mavi LED sönüyor)
    lcd.print("Far:KAPALI");
    farOpen = false;
  }
  delay(100);

  // Yakıt bilgisi yakitGosterimAktif ise gösterilir
  if (yakitGosterimAktif) {
    lcd.setCursor(11, 0);
    lcd.print("Y:");
    lcd.print((int)yakitYuzde);
    lcd.print("%");
  }

  // 1. Yakıt seviyesi kontrolü
  if (yakitYuzde <= 0) {
    motorDurumu = false;
    digitalWrite(motor, LOW);
    digitalWrite(klimaMotor, LOW);
    digitalWrite(ledYakit, LOW);

    lcd.setCursor(0, 1);
    lcd.print("Yakit Bitti!");
    delay(100);
    return;
  }

  // 2. Kapı kontrolü
  if (!kapiKapali) {
    motorDurumu = false;
    digitalWrite(motor, LOW);
    digitalWrite(rgbRed, LOW);
    digitalWrite(rgbBlue, LOW);

    lcd.setCursor(0, 1);
    lcd.print("Kapi Acik!");
    delay(100);
    return;
  } else {
    digitalWrite(rgbRed, HIGH);
    digitalWrite(rgbBlue, HIGH);
  }

  // 3. Emniyet kemeri kontrolü
  if (!kemerTakili) {
    motorDurumu = false;
    digitalWrite(motor, LOW);
    digitalWrite(ledKemer, HIGH);
    digitalWrite(buzzer, HIGH);

    lcd.setCursor(0, 1);
    lcd.print("Kemer Tak!");
    delay(100);
    return;
  } else {
    digitalWrite(ledKemer, LOW);
    digitalWrite(buzzer, LOW);
  }

  // Yakıt durumu uyarıları
  if (yakitYuzde < 5) {
    // Yanıp sönme efekti
    unsigned long suAnkiZaman = millis();
    if (suAnkiZaman - oncekiZaman >= 500) {
      oncekiZaman = suAnkiZaman;
      ledDurumu = !ledDurumu;
      digitalWrite(ledYakit, ledDurumu);
    }

    lcd.setCursor(0, 1);
    lcd.print("Yakit<%5!");

    // Kritik yakıt uyarısında yakıt bilgisini tekrar göster
    yakitGosterimAktif = true;
    yakitGosterimZamani = millis();
  } else if (yakitYuzde < 10) {
    digitalWrite(ledYakit, HIGH);

    lcd.setCursor(0, 1);
    lcd.print("Yakit<%10");

    // Düşük yakıt uyarısında yakıt bilgisini tekrar göster
    yakitGosterimAktif = true;
    yakitGosterimZamani = millis();
  } else {
    digitalWrite(ledYakit, LOW);

    // Motor çalışma durumu kontrolü
    if (motorDurumu) {
      digitalWrite(motor, HIGH);

      // Sıcaklık kontrolü ve klima
      if (derece > 25.0) {
        digitalWrite(klimaMotor, HIGH);

        lcd.setCursor(0, 1);
        lcd.print((int)derece);
        lcd.print("C Klima:ACIK");
      } else {
        digitalWrite(klimaMotor, LOW);

        lcd.setCursor(0, 1);
        lcd.print("Motor:ACIK");
      }
    } else {
      digitalWrite(motor, LOW);
      digitalWrite(klimaMotor, LOW);

      lcd.setCursor(0, 1);
      lcd.print("Motor:KAPALI");
    }
  }

  // Yakıt seviyesi değiştiğinde yakıt bilgisini tekrar göster
  static int oncekiYakit = -1;
  if ((int)yakitYuzde != oncekiYakit) {
    yakitGosterimAktif = true;
    yakitGosterimZamani = millis();
    oncekiYakit = (int)yakitYuzde;
  }
  delay(100);
}