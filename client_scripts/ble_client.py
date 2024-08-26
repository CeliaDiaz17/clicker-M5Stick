import asyncio
from bleak import BleakClient, BleakScanner

DEVICE_ADDRESS = "14:2b:2f:b0:70:c0"
CHARACTERISTIC_UUID = "00002a37-0000-1000-8000-00805f9b34fb"

async def run():
    try:
        device = await BleakScanner.find_device_by_address(DEVICE_ADDRESS, timeout=20.0)
        if device is None:
            print(f"No se pudo encontrar el dispositivo con la dirección {DEVICE_ADDRESS}")
            return

        async with BleakClient(device) as client:
            print(f"Conectado: {client.is_connected}")

            def notification_handler(sender, data):
                value = int.from_bytes(data, byteorder='little')
                print(f"Valor recibido: {value}")

            await client.start_notify(CHARACTERISTIC_UUID, notification_handler)
            print("Escuchando notificaciones. Presiona Ctrl+C para salir.")
            while True:
                await asyncio.sleep(1.0)
    except Exception as e:
        print(f"Ocurrió un error: {e}")

asyncio.run(run())
