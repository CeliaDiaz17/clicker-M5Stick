/*
This class implements a BLE peripheral that allows to select an option from a list of options using the buttons of the M5StickC Plus 2.
*/

#include <M5StickCPlus2.h>
#include <ArduinoBLE.h>
#include <string.h>

// Declaraciones de funciones
void mostrarOpciones();
void reiniciarPantalla();

// Declaraciones globales
const char* opciones[] = {"A", "B", "C"};
int numOpciones = 3;
int opcionActual = -1;
bool opcionSeleccionada = false;
unsigned long tiempoSeleccion = 0;
const unsigned long tiempoEspera = 7000;
bool bleConnected = false;
unsigned long lastUpdateTime = 0; //momento d la ultima actualizacion
const unsigned long resetInterval = 15000; 
unsigned char currentValue = 0x00;


class BLEManager {
private:
    static BLEManager* instance;
    BLEService testService;
    BLEUnsignedCharCharacteristic testCharacteristic;
    BLEDescriptor* testDescriptor;

    BLEManager() : 
        testService("34d9a23f-2249-42a3-bb7e-6aa0640154a9"),
        testCharacteristic("92b5d163-1322-4be4-b163-f775e211259f", BLERead | BLEWrite),
        testDescriptor(new BLEDescriptor("2901", "clicker"))  {}      


public:
    static BLEManager* getInstance() {
        if (instance == nullptr) {
            instance = new BLEManager();
        }
        return instance;
    }

    void setupBLE() {
        if (!BLE.begin()) {
            M5.Lcd.println("BLE couldn't be initialized");
            while (1);
        }
        BLE.setLocalName("M5StickCPlus2");
        BLE.setAdvertisedService(testService);
        testCharacteristic.addDescriptor(*testDescriptor);
        testService.addCharacteristic(testCharacteristic);
        BLE.addService(testService);
        BLE.advertise();
    }

    void updateAdvertising(unsigned char* opcion) {
        BLE.stopAdvertise();
        byte manufacturerData[1];
        manufacturerData[0] = 'A' + opcionActual; //si la opcion falla revisar esto
        //manufacturerData[1] = strlen(opcion);
        BLE.setManufacturerData(manufacturerData, sizeof(manufacturerData));
        BLE.advertise();
    }

    bool writeCharacteristic(unsigned char valor) {
        if (testCharacteristic.writeValue(valor)) {
            currentValue = valor;
            lastUpdateTime = millis();
            return true;
        }
        return false;
    }
    
    void loopBLE() {
      BLEDevice central = BLE.central();
      if (central) {
        bleConnected = true;
      }
      if (!bleConnected) {
        BLE.advertise();
      }

      if(currentValue != 0x00 && millis() - lastUpdateTime > resetInterval) {
        writeCharacteristic(0x00);
      }
    }

    bool canWrite() {
        return testCharacteristic.canWrite();
    }

};

BLEManager* BLEManager::instance = nullptr;

void dibujarFlechaAbajo(int x, int y) {
    M5.Lcd.drawLine(x, y, x, y+15);      // Línea vertical
    M5.Lcd.drawLine(x, y+20, x-5, y+15); // Punta izquierda
    M5.Lcd.drawLine(x, y+20, x+5, y+15); // Punta derecha
}

void mostrarOpciones() {
    M5.Lcd.clear();
    M5.Lcd.setTextSize(6);

    int x = 20;
    int y = 50;

    for (int i = 0; i < numOpciones; i++) {
        int anchoTexto = strlen(opciones[i]) * 12;
        if (i == opcionActual) {
            M5.Lcd.setTextColor(YELLOW);
            M5.Lcd.drawLine(x, 85 + 12, x + 30, 85 + 12, WHITE); // Línea justo debajo de la letra
        } else {
            M5.Lcd.setTextColor(WHITE); // Default white text on black background
        }
        
        M5.Lcd.setCursor(x, y);
        M5.Lcd.println(opciones[i]);

        x += strlen(opciones[i]) * 12 + 70; // Avanza la posición X
        
    }
}

void reiniciarPantalla() {
    opcionActual = 0;
    opcionSeleccionada = false;
    mostrarOpciones();
}


void setup() {
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    BLEManager::getInstance()->setupBLE();
    mostrarOpciones();
}

void loop() {
    M5.update();
    BLEManager::getInstance()->loopBLE(); // NO VUELVAS A COMENTAR ESTA LINEA 

    if(M5.BtnB.wasHold()) {
        reiniciarPantalla();
    }

    /*
    if(opcionSeleccionada) {
        unsigned long tiempoActual = millis();
        if (tiempoActual - tiempoSeleccion >= 12000){
            reiniciarPantalla();
        }
    }
    * */

    if (!opcionSeleccionada) {
        if (M5.BtnB.wasPressed()) {
            opcionActual = (opcionActual + 1) % numOpciones;
            mostrarOpciones();
        }
        if (M5.BtnA.wasPressed()) {
            opcionSeleccionada = true;
            //tiempoSeleccion = millis();

            M5.Lcd.clear();
            M5.Lcd.setTextColor(WHITE);
            M5.Lcd.setTextSize(2);
            M5.Lcd.setCursor(10,10);
            M5.Lcd.printf("Opcion seleccionada");

            M5.Lcd.setTextSize(6);
            M5.Lcd.setCursor(100,40);
            M5.Lcd.printf("%c", 'A'+opcionActual);

            M5.Lcd.drawLine(0, 100, M5.Lcd.width(), 100, WHITE);
            M5.Lcd.setTextSize(1.5);
            M5.Lcd.setCursor(60, 110);
            M5.Lcd.printf("Manten ");
            dibujarFlechaAbajo(M5.Lcd.getCursorX(), M5.Lcd.getCursorY());
            M5.Lcd.printf(" para volver");
            
            unsigned char valor = 'A' + opcionActual;
            
            bool writeSuccess = BLEManager::getInstance()->writeCharacteristic(valor);
  
           unsigned char valorASCII = 'A' + opcionActual;
           BLEManager::getInstance()->updateAdvertising(&valorASCII);

        }
    }

    delay(100);
}