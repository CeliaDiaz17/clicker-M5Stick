#include <M5StickCPlus2.h>

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextSize(3);
  M5.Lcd.println("Holi! :3");
}

void loop() {
}