#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

// LCD & Servo
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;

// IR Sensors
int IR1 = 2; // Entry sensor
int IR2 = 3; // Exit sensor

int irSlotPins[] = {6, 8, A0, A1}; // IR sensors for open parking slots
bool slotStatus[4] = {true, true, true, true}; // true free

// RFID
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
byte validUID[] = {0x03, 0x56, 0x00, 0xE4};

// Mode Button
bool reservedMode = false;
const int modeButton = A2;

// System state
unsigned long rfidStartTime = 0;
bool waitingForRFID = false;

int reservedSlots = 4; // static for reserved mode

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  lcd.init();
  lcd.backlight();

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(modeButton, INPUT);

  for (int i = 0; i < 4; i++) pinMode(irSlotPins[i], INPUT);

  myservo.attach(4);
  myservo.write(100); // Gate closed

  lcd.setCursor(0, 0);
  lcd.print("  SMART PARK  ");
  lcd.setCursor(0, 1);
  lcd.print(" SYSTEM READY ");
  delay(2000);
  lcd.clear();
}

void loop() {

  reservedMode = (digitalRead(modeButton) == LOW);
  updateOpenSlots();

  if (!waitingForRFID) {
    lcd.setCursor(0, 0);
    if (reservedMode) {
      lcd.print("Reserved parking ");
      lcd.setCursor(0, 1);
      lcd.print("Slots Left: ");
      lcd.print(reservedSlots);
      lcd.print("    ");
    } else {
      lcd.print("Open parking    ");
      lcd.setCursor(0, 1);
      lcd.print("Free: ");
      lcd.print(getFreeSlotNumbers());
      lcd.print("    ");
    }

    // ENTRY
    if (digitalRead(IR1) == LOW && !waitingForRFID) {
      if (reservedMode && reservedSlots > 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Car Detected!");
        lcd.setCursor(0, 1);
        lcd.print("Scan RFID...");
        waitingForRFID = true;
        rfidStartTime = millis();
      } else if (!reservedMode && countFreeOpenSlots() > 0) {
        openGate();
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("No Slots Avail");
        delay(2000);
      }
    }
  }

  // RFID for reserved mode
  if (waitingForRFID) {
    if (checkRFID()) {
      openGate();
      reservedSlots--;
      waitingForRFID = false;
    } else if (millis() - rfidStartTime > 10000) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Timeout... ");
      delay(1000);
      lcd.clear();
      waitingForRFID = false;
    }
  }

  // EXIT
  if (digitalRead(IR2) == LOW && !waitingForRFID) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Exit Detected");
    myservo.write(0);
    delay(1000);
    while (digitalRead(IR1) == HIGH);
    delay(500);
    myservo.write(100);

    if (reservedMode && reservedSlots < 4) {
      reservedSlots++;
    }
    lcd.clear();
  }
}

void updateOpenSlots() {
  for (int i = 0; i < 4; i++) {
    slotStatus[i] = digitalRead(irSlotPins[i]) == HIGH;
  }
}

String getFreeSlotNumbers() {
  String result = "";
  for (int i = 0; i < 4; i++) {
    if (slotStatus[i]) {
      result += String(i + 1);
      result += ",";
    }
  }
  if (result.length() > 0) {
    result.remove(result.length() - 1); // remove last comma
  } else {
    result = "None";
  }
  return result;
}

int countFreeOpenSlots() {
  int count = 0;
  for (int i = 0; i < 4; i++) {
    if (slotStatus[i]) count++;
  }
  return count;
}

void openGate() {
  myservo.write(0);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Access Granted ");
  lcd.setCursor(0, 1);
  lcd.print("Please Enter");
  delay(1000);
  while (digitalRead(IR2) == HIGH);
  delay(200);
  myservo.write(100);
  delay(2000);
  lcd.clear();
}

bool checkRFID() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return false;
  for (byte i = 0; i < 4; i++) {
    if (rfid.uid.uidByte[i] != validUID[i]) {
      lcd.setCursor(0, 0);
      lcd.print(" Invalid Card ");
      lcd.setCursor(0, 1);
      lcd.print("Access Denied");
      delay(1500);
      lcd.clear();
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
      return false;
    }
  }
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return true;
}