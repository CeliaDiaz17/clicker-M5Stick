#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

class BLEService;
class BLECharacteristic;

extern BLEService testService;
extern BLECharacteristic testCharacteristic;

void setupBLE();
void loopBLE();
bool isBLEConnected();
int getLastValue();

#endif // BLE_MANAGER_H