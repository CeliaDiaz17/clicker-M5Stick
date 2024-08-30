#include <M5StickCPlus2.h>
#include <ArduinoBLE.h>
#include <ble_manager.h>

BLEManager bleManager;

const char* opciones[] = {"Opcion A", "Opcion B", "Opcion C"};
int numOpciones = 3;
int opcionActual = 0;
bool opcionSeleccionada = false;

void mostrarOpciones();
//void updateDisplay();

void setup() {
  M5.begin();
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);

  bleManager.setupBLE();
  mostrarOpciones();
  //updateDisplay();
}

void loop() {
  M5.update();
  bleManager.loopBLE();

  if (!opcionSeleccionada) {
    if (M5.BtnB.wasPressed()) {
      opcionActual = (opcionActual + 1) % numOpciones;
      mostrarOpciones();
    }
    
    if (M5.BtnA.wasPressed()) {
      opcionSeleccionada = true;
      M5.Lcd.clear();
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.printf("La opcion %c ha sido seleccionada", 'A' + opcionActual);

      char valor[3];
      switch (opcionActual) {
        case 0:
          strcpy(valor, "00"); //A
          break;
        case 1:
          strcpy(valor, "01"); //B
          break;
        case 2:
          strcpy(valor, "10"); //C
          break;
      }
      bleManager.writeValueToCharacteristic(valor);
    }
  }
  
  //updateDisplay();
  delay(100);
}

void mostrarOpciones() {
  M5.Lcd.clear();
  M5.Lcd.setCursor(0, 0);
  for (int i = 0; i < numOpciones; i++) {
    if (i == opcionActual) {
      M5.Lcd.print("> ");
    } else {
      M5.Lcd.print("  ");
    }
    M5.Lcd.println(opciones[i]);
  }
}
