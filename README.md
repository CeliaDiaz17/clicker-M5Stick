# clicker-M5Stick
This repository contais all the necesary steps since the beginning to end up using M5StickCPlus as a clicker in a classroom using BLE

## Cliente BLE

En la carpeta `client_scripts` se encuentra un script Python (`ble_client.py`) para conectarse al M5StickCPlus2 desde una máquina Ubuntu. Este script permite recibir datos del dispositivo a través de BLE.

Para usar el script:

1. Instala las dependencias:
    sudo apt-get update
    sudo apt-get install python3-pip libglib2.0-dev
    sudo pip3 install bleak

2. Ajusta la dirección MAC del dispositivo en el script.

3. Ejecuta el script:
    python3 client_scripts/ble_client.py