import time
import dbus
import dbus.mainloop.glib
import sys
try:
    from gi.repository import GLib
except ImportError:
    import gobject as GLib

TARGET_DEVICE = "14:2B:2F:B0:70:C2"
SERVICE_UUID = "34d9a23f-2249-42a3-bb7e-6aa0640154a9"  
CHARACTERISTIC_UUID = "92b5d163-1322-4be4-b163-f775e211259f"  

def get_managed_objects():
    bus = dbus.SystemBus()
    manager = dbus.Interface(bus.get_object("org.bluez", "/"),
                             "org.freedesktop.DBus.ObjectManager")
    return manager.GetManagedObjects()

def find_device(devices, device_address):
    for path, ifaces in devices.items():
        device = ifaces.get("org.bluez.Device1")
        if device is None:
            continue
        if device["Address"] == device_address:
            obj = bus.get_object("org.bluez", path)
            return dbus.Interface(obj, "org.bluez.Device1")
    return None

def find_characteristic(device_path, service_uuid, characteristic_uuid):
    bus = dbus.SystemBus()
    objects = get_managed_objects()
    for path, ifaces in objects.items():
        if path.startswith(device_path):
            chrc = ifaces.get("org.bluez.GattCharacteristic1", {})
            if chrc.get("UUID") == characteristic_uuid:
                return path
    return None


def read_characteristic(characteristic_path):
    try:
        bus = dbus.SystemBus()
        chrc = bus.get_object("org.bluez", characteristic_path)
        chrc_iface = dbus.Interface(chrc, "org.bluez.GattCharacteristic1")
        
        value = chrc_iface.ReadValue({})
        return bytes(value)
    except dbus.exceptions.DBusException as e:
        print(f"Error del D-Bus al leer la caracteristica: {str(e)}")
    except Exception as e:
        print(f"Error al leer el valor de la caracteristica: {str(e)}")
    return None
    

'''
def write_characteristic(characteristic_path, value):
    bus = dbus.SystemBus()
    chrc = bus.get_object("org.bluez", characteristic_path)
    chrc_iface = dbus.Interface(chrc, "org.bluez.GattCharacteristic1")
    chrc_iface.WriteValue(value, {})
'''

def connect_and_communicate():
    bus = dbus.SystemBus()
    adapter = dbus.Interface(bus.get_object("org.bluez", "/org/bluez/hci0"), "org.bluez.Adapter1")
    
    print(f"Buscando dispositivo: {TARGET_DEVICE}")
    adapter.StartDiscovery()
    
    try:
        print("Esperando a que el dispositivo sea descubierto...")
        time.sleep(20)
        
        devices = get_managed_objects()
        device = find_device(devices, TARGET_DEVICE)
        
        if device is None:
            print("Dispositivo no encontrado")
            return

        print("Dispositivo encontrado. Intentando conectar...")
        device.Connect()
        print("Conectado exitosamente")

        print("Buscando característica...")
        time.sleep(10)
        char_path = find_characteristic(device.object_path, SERVICE_UUID, CHARACTERISTIC_UUID)
        if char_path is None:
            print("Característica no encontrada")
            return
        time.sleep(10)

        print("Característica encontrada. Leyendo valor...")
        value = read_characteristic(char_path)
        print(f"Valor leído: {value}")
        #TO-DO Recoger valor leido y meterlo en una cola

        '''
        print("Escribiendo un nuevo valor...")
        new_value = [0x48, 0x65, 0x6C, 0x6C, 0x6F]  # "Hello" en ASCII
        write_characteristic(char_path, new_value)
        print("Valor escrito exitosamente")
        '''
        
    except Exception as e:
        print(f"Error: {str(e)}")
    finally:
        adapter.StopDiscovery()

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    bus = dbus.SystemBus()
    mainloop = GLib.MainLoop()
    
    connect_and_communicate()
    
    print("Presiona Ctrl+C para salir.")
    try:
        mainloop.run()
    except KeyboardInterrupt:
        print("Programa terminado por el usuario.")

    