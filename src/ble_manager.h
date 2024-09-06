#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H
#include <Arduino.h>
#include <ArduinoBLE.h>

class BLEManager {
    public:
        BLEManager();
        void setupBLE();
        void loopBLE();
        bool isBLEConnected();
        int getLastValue();
        void writeValueToCharacteristic(const uint8_t value);
        BLECharacteristic testCharacteristic;
    
    private:
        BLEService testService;
        bool bleConnected;
        int lastValue;

};

#endif // BLE_MANAGER_H