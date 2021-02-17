/*
    This sketch demonstrates how to scan WiFi networks.
    The API is almost the same as with the WiFi Shield library,
    the most obvious difference being the different file you need to include:
*/
#include "WiFi.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

#define OLED_DISPLAY true

// any pins can be used
#define SHARP_SCK  36
#define SHARP_MOSI 35
#define SHARP_SS   34

#define SHARP_5V_ON 42
#define BLACK 0
#define WHITE 1

int minorHalfSize;
Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 400, 240);

#include <Wire.h>
const uint8_t I2C_SDA_PIN = 8; //SDA;  // i2c SDA Pin
const uint8_t I2C_SCL_PIN = 9; //SCL;  // i2c SCL Pin

void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");

  pinMode(SHARP_5V_ON, OUTPUT);
  digitalWrite(SHARP_5V_ON, HIGH);
  delay(100);
  // start & clear the display
  display.begin();
  minorHalfSize = min(display.width(), display.height()) / 2;
  testfillroundrect();
  display.refresh();
  delay(2000);

  display.clearDisplay();
  display.setRotation(0);
  display.setTextSize(3);
  display.setTextColor(BLACK);
  display.setCursor(2, 2);
  display.println("ESP32-S2 WiFi scanner");
  display.refresh();

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
}

void i2c_scan() {
  byte error, address;
  int nDevices;

  display.clearDisplay();
  display.setCursor(2, 2);
  display.println("ESP32-S2 I2C scanner");

  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      nDevices++;

      Serial.print("Device found @ 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

#if OLED_DISPLAY
      display.println((String)nDevices + ":x" + String(address, HEX));
      display.refresh();
#endif
    }
    else if (error == 4)
    {
      Serial.print("Unknow error @ 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);

#if OLED_DISPLAY
      display.println("!:x" + String(address, HEX));
      display.refresh();
#endif
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");

#if OLED_DISPLAY
    display.println("No I2C devices found");
    display.refresh();
#endif
  } else {
    Serial.println("done\n");

#if OLED_DISPLAY
    display.println("done");
    display.refresh();
#endif
  }
}

void loop() {
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    display.clearDisplay();
    display.setCursor(2, 2);
    display.println("ESP32-S2 WiFi scanner");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      String message;
      message += i + 1;
      message += ": ";
      message += WiFi.SSID(i);
      message += " (";
      message += WiFi.RSSI(i);
      message += ")";
      message += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*";
      Serial.println(message);
      display.println(message);
      delay(10);
    }
  }
  display.refresh();
  Serial.println("");

  // Wait a bit before scanning again
  delay(5000);
  i2c_scan();
  delay(3000);
}

void testfillroundrect(void) {
  uint8_t color = BLACK;
  for (int i = 0; i < minorHalfSize / 2; i += 2) {
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, minorHalfSize / 2, color & 1);
    display.refresh();
    color++;
  }
}
