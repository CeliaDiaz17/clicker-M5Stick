#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H
#include <Arduino.h>

class BLEManager {
    public:
        void setupBLE();
        void loopBLE();
        bool isBLEConnected();
        int getLastValue();
        void writeValueToCharacteristic(const char* value);
    
    private:
        BLEService testService;
        BLECharacteristic testCharacteristic;
        bool bleConnected;
        int lastValue;

};

#endif // BLE_MANAGER_H