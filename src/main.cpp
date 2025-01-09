/*
This class implements a BLE peripheral that allows to select an option from a list of options using the buttons of the M5StickC Plus 2.
*/

#include <M5StickCPlus2.h>
#include <ArduinoBLE.h>
#include <string.h>

#define BATTERY_MIN_VOLTAGE 3.3
#define BATTERY_MAX_VOLTAGE 4.2

// Declaraciones de funciones
void mostrarOpciones();
void reiniciarPantalla();
float obtenerPorcentajeBateria();

// Declaraciones globales
const char* opciones[] = {"A", "B", "C"};
int numOpciones = 3;
int opcionActual = -1;
bool opcionSeleccionada = false;
unsigned long tiempoSeleccion = 0;
const unsigned long tiempoEspera = 7000;
bool bleConnected = false;
unsigned long lastUpdateTime = 0; //momento d la ultima actualizacion
const unsigned long resetInterval = 20000; 
unsigned char currentValue = 0x00;

float porcentajeBateriaAnterior = -1.0;
bool prevBleConnected = false;  // Para trackear el estado anterior
unsigned long lastConnectionCheck = 0;
const unsigned long CONNECTION_CHECK_INTERVAL = 1000;



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
        unsigned long currentMillis = millis();
        
        // Solo actualizamos el estado cada CONNECTION_CHECK_INTERVAL
        if (currentMillis - lastConnectionCheck >= CONNECTION_CHECK_INTERVAL) {
            lastConnectionCheck = currentMillis;
            
            BLEDevice central = BLE.central();
            bool currentlyConnected = central && central.connected();
            
            // Solo actualizamos el círculo si hay un cambio de estado
            if (currentlyConnected != prevBleConnected) {
                bleConnected = currentlyConnected;
                dibujarConexion(bleConnected);
                prevBleConnected = bleConnected;
            }
            
            // Si no está conectado, aseguramos que siga en modo advertising
            if (!bleConnected) {
                BLE.advertise();
            }
        }
    }

    /*
    void loopBLE() {
      BLEDevice central = BLE.central();
      if (central) {
        bleConnected = true;
        dibujarConexion(bleConnected);
      }
      if (!bleConnected) {
        BLE.advertise();
        dibujarConexion(bleConnected);
      }
    }
    */

    bool canWrite() {
        return testCharacteristic.canWrite();
    }
    

void dibujarConexion(bool conectado) {
    int x = 230; //Coordenadas del círculo
    int y = 10;
    int radio = 10;

    M5.Lcd.fillCircle(x, y, radio, BLACK); 
    if (conectado) {
        M5.Lcd.fillCircle(x, y, radio, GREEN); //Círculo verde
    } else {
        M5.Lcd.fillCircle(x, y, radio, RED); 
    }
}
};

BLEManager* BLEManager::instance = nullptr;

float obtenerPorcentajeBateria(float &voltage)
{
    int rawReading = analogRead(38);
    voltage = rawReading * (3.3 / 4095.0) * 2;

    float batteryPercent = map(voltage * 100, BATTERY_MIN_VOLTAGE * 100, BATTERY_MAX_VOLTAGE * 100, 0, 100);
    if (batteryPercent > 100) batteryPercent = 100;
    if (batteryPercent < 0) batteryPercent = 0;

    return batteryPercent;
}

void mostrarBateria() {
    float voltage = 0.0;
    float porcentajeBateria = obtenerPorcentajeBateria(voltage);
    int x = 5;
    int y = 5;

    if (porcentajeBateria != porcentajeBateriaAnterior){
        porcentajeBateriaAnterior = porcentajeBateria;
    }

    // Muestra los datos en la pantalla
    M5.Lcd.fillRect(0, 0, 40, 15, BLACK);
    M5.Lcd.setTextSize(1.5);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.setCursor(x,y);
    M5.Lcd.printf("%.1f%%", porcentajeBateria);}

void dibujarFlechaAbajo(int x, int y) {
    M5.Lcd.drawLine(x, y, x, y+15);      
    M5.Lcd.drawLine(x, y+20, x-5, y+15); 
    M5.Lcd.drawLine(x, y+20, x+5, y+15); 
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
            M5.Lcd.drawLine(x, 85 + 12, x + 30, 85 + 12, WHITE); //subrayado
        } else {
            M5.Lcd.setTextColor(WHITE); 
        }
        
        M5.Lcd.setCursor(x, y);
        M5.Lcd.println(opciones[i]);

        x += strlen(opciones[i]) * 12 + 70; //avanza pos x
        
    }
}

void reiniciarPantalla() {
    opcionActual = 0;
    opcionSeleccionada = false;
    mostrarOpciones();
    BLEManager::getInstance() -> dibujarConexion(bleConnected);
}

void loop() {
    M5.update();
    BLEManager::getInstance()->loopBLE(); // NO VUELVAS A COMENTAR ESTA LINEA 

    mostrarBateria();
    BLEManager::getInstance() -> dibujarConexion(bleConnected);


    if(M5.BtnB.wasHold()) {
        reiniciarPantalla();
        BLEManager::getInstance()->writeCharacteristic('N');
    }  

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
            M5.Lcd.setCursor(10,25);
            M5.Lcd.printf("Opcion seleccionada");

            M5.Lcd.setTextSize(6);
            M5.Lcd.setCursor(110,50);
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
void setup() {
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(2);
    BLEManager::getInstance()->setupBLE();
    mostrarOpciones();
}
