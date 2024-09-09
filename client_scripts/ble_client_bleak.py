import asyncio
from bleak import BleakClient, BleakScanner
from queue import Queue

address_1 = "14:2B:2F:B0:70:C2"
address_2 = "10:06:1c:27:6e:a2"
address_3 = "10:06:1c:28:63:76"
address_queue = Queue()
for address in [address_1, address_2, address_3]:
    address_queue.put(address)
    
data_list = []

async def scan_device(address):
    try:
        async with BleakClient(address) as client:
            print(f"Connected to: {address}")
            x = await client.is_connected()
            #print("Connected: {0}".format(x))

            services = await client.get_services()
            for service in services:
                print(service)
                for char in service.characteristics:
                    decoded_value = None
                    if "read" in char.properties:
                        value = bytes(await client.read_gatt_char(char.uuid))
                        decoded_value = value.decode() 
                        print("Characteristic: {0} ({1}) | Value: {2}".format(char.description, char.uuid, value.decode()))

                    #meter datos en cola
                    if decoded_value is not None:
                        data_list.append(decoded_value)
                        print("Data list:")
                        for item in data_list:
                            print(item)
                    
            await client.disconnect()
    
    except Exception as e:
         print(f"Error al conectar con {address}: {str(e)}")
    
async def main(address):
    while not address_queue.empty():
        address = address_queue.get()
        await scan_device(address)
        
    print("Data in queue:")
    for item in data_list:
        print(item)

asyncio.run(main(address))