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
const char* opciones[] = {"Opcion A", "Opcion B", "Opcion C"};
int numOpciones = 3;
int opcionActual = -1;
bool opcionSeleccionada = false;
unsigned long tiempoSeleccion = 0;
const unsigned long tiempoEspera = 7000;
bool bleConnected = false;


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

    void loopBLE() {
      BLEDevice central = BLE.central();
      if (central) {
        bleConnected = true;
        //M5.Lcd.println("Connected to central");
      }

      if (!bleConnected) {
        //M5.Lcd.println("Central disconnected. Waiting for new connections...");
        BLE.advertise();
      }
    }

    bool writeCharacteristic(unsigned char valor) {
        return testCharacteristic.writeValue(valor);
    }

    bool canWrite() {
        return testCharacteristic.canWrite();
    }

};

BLEManager* BLEManager::instance = nullptr;

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

    if(M5.BtnB.wasDoubleClicked()) {
        reiniciarPantalla();
    }

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
            
            
            unsigned char valor = 'A' + opcionActual;
            
            bool writeSuccess = BLEManager::getInstance()->writeCharacteristic(valor);
            if (writeSuccess) {
                M5.Lcd.printf("\nValor escrito correctamente: %c", valor);
            } else {
                M5.Lcd.println("\nError al escribir el valor");
                if (!BLEManager::getInstance()->canWrite()) {
                    M5.Lcd.println("La caracteristica no esta lista para escribir");
                }
            }
    
           //const char* opcion = opciones[opcionActual];
           unsigned char valorASCII = 'A' + opcionActual;
           BLEManager::getInstance()->updateAdvertising(&valorASCII);

           M5.Lcd.printf("\nEnviando opcion: %c en el advertising", valorASCII);
        }
    }

    delay(100);
}