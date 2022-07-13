#include <SD.h>
#include <Wire.h>
#include <Keyboard.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <Adafruit_Fingerprint.h>

#define mySerial Serial1
#define U8G2_16BIT

U8G2_SH1122_256X64_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
const int chipSelect = BUILTIN_SDCARD;
const int buttonUp = 4;     //Up Down Pin
const int buttonDown = 5;   //Down Button Pin
const int buttonEnter = 6;  //Enter Button Pin
int i = 0;
int t = 0;
int u = 0;
bool firstPress = false;
bool unlock = false;
int fingerprintID = 0;
uint8_t id;
File accFile;
File passFile;
String buffer;
String accounts[25];
String passwords[25];
char accBuff;
char passBuff;

void setup() {
  Serial.begin(115200);
  finger.begin(57600);
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  Keyboard.begin();
  pinMode(buttonUp, INPUT_PULLUP);       //Set the Up Button Pin as an Input
  pinMode(buttonDown, INPUT_PULLUP);     //Set the Down Button Pin as an Input
  pinMode(buttonEnter, INPUT_PULLUP);    //Set the Enter Buttron pin as an Input

  //Start SplashScreen
  u8g2.firstPage();
  do {
    u8g2.drawStr(0, 24, "Password Manager");
  } while ( u8g2.nextPage() );
  delay(250);

  if (!SD.begin(chipSelect)) {
    while (true);
  }
  passFile = SD.open("passwords.txt");
  if (passFile) {
    while (passFile.available()) {
      buffer = passFile.readStringUntil('\n');
      Serial.println(buffer);
      passwords[t] = buffer;
      t++;
    }
    passFile.close();
  }
  accFile = SD.open("accounts.txt");
  if (accFile) {
    while (accFile.available()) {
      buffer = accFile.readStringUntil('\n');
      Serial.println(buffer);
      accounts[u] = buffer;
      u++;
    }
    accFile.close();
  }
}
void loop() {
  /*
  fingerprintID = getFingerprintIDez();
  if (fingerprintID > 0) {
    unlock = true;
  }
  */
  if (unlock == true) {
    if (digitalRead(buttonUp) == LOW && firstPress == false) {
      i++;
      u8g2.clearBuffer();
      u8g2.firstPage();
      accBuff = accounts[i].c_str();
      passBuff = passwords[i].c_str();
      do{
        u8g2.drawStr(0, 24, accBuff);
        u8g2.drawStr(0, 40, passBuff); 
      }while (u8g2.nextPage());
      firstPress = true;
    }
    if (digitalRead(buttonUp) == LOW && i < t - 1 && firstPress == true) {
      i++;
      u8g2.clearBuffer();
      u8g2.firstPage();
      accBuff = accounts[i].c_str();
      passBuff = passwords[i].c_str();
      do{
        u8g2.drawStr(0, 24, accBuff);
        u8g2.drawStr(0, 40, passBuff); 
      }while (u8g2.nextPage());
    }
    if (digitalRead(buttonDown) == LOW && i > 0) {
      i--;
      u8g2.clearBuffer();
      u8g2.firstPage();
      accBuff = accounts[i].c_str();
      passBuff = passwords[i].c_str();
      do{
        u8g2.drawStr(0, 24, accBuff);
        u8g2.drawStr(0, 40, passBuff);  
      }while (u8g2.nextPage());
    }
    if (digitalRead(buttonEnter) == LOW) {
      Keyboard.print(passwords[i]);
      delay(250);
    }
  }
  //enroll();
}
/*
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    //Serial.println("-1,1");
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    //Serial.println("-1,2");
    return -1;
  }

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) {
    //Serial.println("-1,3");
    return -1;
  }
  return finger.fingerID;
}

void enroll() {
  if (count() == 0) {
    //screen("Fingerprint enroll:", "Place Finger on Sensor");
    Screen.clear();
    Screen.sendString("Fingerprint enroll:", 0, 0);
    Screen.sendString("Place Finger on Sensor", 0, 1);
    id++;
    while (!getFingerprintEnroll())
      ;
  }
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        break;
    }
  }

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      //screen("", "OK!");
      Screen.clear();
      Screen.sendString("OK!", 0, 0);
      delay(750);
      break;
    case FINGERPRINT_IMAGEMESS:
      //screen("", "Fail!");
      Screen.clear();
      Screen.sendString("Fail!", 0, 0);
      delay(2000);
      return p;
  }
  //  screen("", "Remove Finger!");
  Screen.clear();
  Screen.sendString("Remove Finger!", 0, 0);
  delay(750);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  p = -1;
  //screen("", "Place same Finger again");
  Screen.clear();
  Screen.sendString("Place same Finger again", 0, 0);
  delay(750);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        break;
    }
  }

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      //screen("", "OK!");
      Screen.clear();
      Screen.sendString("OK!", 0, 0);
      delay(750);
      break;
    case FINGERPRINT_IMAGEMESS:
      //screen("", "Fail");
      Screen.clear();
      Screen.sendString("Fail", 0, 0);
      delay(2000);
      return p;
  }

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    //screen("", "Prints Matched!");
    Screen.clear();
    Screen.sendString("Prints Matched!", 0, 0);
    delay(1000);
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    //screen("", "Prints do not Match");
    Screen.clear();
    Screen.sendString("Prints do not Match", 0, 0);
    delay(2000);
    return p;
  } else {
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    //    screen("", "Stored!");
    Screen.clear();
    Screen.sendString("Stored!", 0, 0);
    delay(2000);
    //   screen("", "Password Manager");
    Screen.clear();
    Screen.sendString("Password Manager", 0, 0);
  }

  return true;

}

uint16_t count() {
  uint16_t x;
  finger.getTemplateCount();
  x = finger.templateCount;
  return x;
}
*/
