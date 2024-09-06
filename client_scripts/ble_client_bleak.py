import asyncio
from bleak import BleakClient, BleakScanner

address = "14:2B:2F:B0:70:C2"

async def main(address):
    async with BleakClient(address) as client:
        x = await client.is_connected()
        print("Connected: {0}".format(x))

        services = await client.get_services()
        for service in services:
            print(service)
            for char in service.characteristics:
                if "read" in char.properties:
                    value = bytes(await client.read_gatt_char(char.uuid))
                    print("Characteristic: {0} ({1}) | Value: {2}".format(char.description, char.uuid, value.decode()))

        await client.disconnect()

asyncio.run(main(address))