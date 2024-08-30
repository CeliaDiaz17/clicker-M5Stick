//Esta clase es responsable de manejar la comunicación BLE del dispositivo

#include <M5StickCPlus2.h>
#include <ArduinoBLE.h>
#include <ble_manager.h>

bool isBLEConnected();
int getLastValue();

bool bleConnected = false;
int lastValue = 0;

void BLEManager::setupBLE(){
    if (!BLE.begin()) {
        M5.Lcd.println("BLE couldn't be initialized");
        while (1);
    }

    BLE.setLocalName("M5StickCPlus2");
    BLEService testService("34d9a23f-2249-42a3-bb7e-6aa0640154a9");
    BLECharacteristic testCharacteristic("92b5d163-1322-4be4-b163-f775e211259f", BLERead | BLENotify, 2);
    BLE.setAdvertisedService(testService);
    testService.addCharacteristic(testCharacteristic);
    BLE.addService(testService);
    BLE.advertise();

    //M5.Lcd.println("BLE device active");
    //M5.Lcd.println("Waiting for BLE conections...");
}

void BLEManager::loopBLE(){
    BLEDevice central = BLE.central();
    if (central) {
        bleConnected = true;
        //M5.Lcd.println("Client connected");
        M5.Lcd.println(central.address());

        while (central.connected()) {
            delay(1000);
        }
        bleConnected = false;
        M5.Lcd.println("Client disconnected");
    }

    if (!bleConnected) {
    BLE.advertise();
    }

}

bool BLEManager::isBLEConnected(){
    return bleConnected;
}

int BLEManager::getLastValue(){
    return lastValue;
}

void BLEManager::writeValueToCharacteristic(const char* value){
    testCharacteristic.writeValue(value);
    lastValue = atoi(value); //Asume que el valor es un numero en formato de cadena, cambiar luego si es necesario
}