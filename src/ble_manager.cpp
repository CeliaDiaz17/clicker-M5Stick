#include <M5StickCPlus2.h>
#include <ArduinoBLE.h>

BLEService testService("180D");
BLECharacteristic testCharacteristic("00002a37-0000-1000-8000-00805f9b34fb", BLERead | BLENotify, 2);

bool isBLEConnected();
int getLastValue();

bool bleConnected = false;
int lastValue = 0;

void setupBLE(){
    if (!BLE.begin()) {
        M5.Lcd.println("BLE couldn't be initialized");
        while (1);
    }

    BLE.setLocalName("M5StickCPlus2");
    BLE.setAdvertisedService(testService);
    testService.addCharacteristic(testCharacteristic);
    BLE.addService(testService);

    testCharacteristic.writeValue(0);
    BLE.advertise();

    M5.Lcd.println("BLE device active");
    M5.Lcd.println("Waiting for BLE conections...");
}

void loopBLE(){
    BLEDevice central = BLE.central();
    if (central) {
        bleConnected = true;
        M5.Lcd.println("Client connected");
        M5.Lcd.println(central.address());

        while (central.connected()) {
            int value = random(100);
            testCharacteristic.writeValue(value);
            lastValue = value;
            M5.Lcd.printf("Value: %d ", value);
            delay(1000);
        }
        bleConnected = false;
        M5.Lcd.println("Client disconnected");
    }

    if (!bleConnected) {
    BLE.advertise();
    }

}


bool isBLEConnected(){
    return bleConnected;
}

int getLastValue(){
    return lastValue;
}