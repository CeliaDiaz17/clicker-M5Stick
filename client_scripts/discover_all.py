import dbus
import dbus.mainloop.glib
try:
    from gi.repository import GLib
except ImportError:
    import gobject as GLib

def list_detected_devices():
    bus = dbus.SystemBus()
    om = dbus.Interface(bus.get_object("org.bluez", "/"), "org.freedesktop.DBus.ObjectManager")
    objects = om.GetManagedObjects()

    for path, interfaces in objects.items():
        if "org.bluez.Device1" in interfaces:
            properties = interfaces["org.bluez.Device1"]
            addr = properties.get("Address", "")
            name = properties.get("Name", "Unknown")
            rssi = properties.get("RSSI", "N/A")
            print(f'Dispositivo detectado: {name} [{addr}] RSSI: {rssi}')

def on_property_changed(interface, changed, invalidated, path):
    if interface != 'org.bluez.Device1':
        return
    bus = dbus.SystemBus()
    device = dbus.Interface(bus.get_object("org.bluez", path), "org.freedesktop.DBus.Properties")
    properties = device.GetAll("org.bluez.Device1")
    
    addr = properties.get("Address", "")
    name = properties.get("Name", "Unknown")
    rssi = properties.get("RSSI", "N/A")
    print(f"Dispositivo actualizado: {name} [{addr}] RSSI: {rssi}")

def on_interfaces_added(path, interfaces):
    if "org.bluez.Device1" in interfaces:
        properties = interfaces["org.bluez.Device1"]
        addr = properties.get("Address", "")
        name = properties.get("Name", "Unknown")
        rssi = properties.get("RSSI", "N/A")
        print(f"Nuevo dispositivo: {name} [{addr}] RSSI: {rssi}")

def start_discovery():
    bus = dbus.SystemBus()
    adapter_proxy = bus.get_object('org.bluez', '/org/bluez/hci0')
    adapter = dbus.Interface(adapter_proxy, 'org.bluez.Adapter1')
    
    print("Estado actual del adaptador:")
    props = dbus.Interface(adapter_proxy, 'org.freedesktop.DBus.Properties')
    print(f"  Powered: {props.Get('org.bluez.Adapter1', 'Powered')}")
    print(f"  Discovering: {props.Get('org.bluez.Adapter1', 'Discovering')}")
    
    if not props.Get('org.bluez.Adapter1', 'Powered'):
        print("Encendiendo el adaptador Bluetooth...")
        props.Set('org.bluez.Adapter1', 'Powered', dbus.Boolean(1))
    
    print("Iniciando escaneo de dispositivos...")
    adapter.StartDiscovery()

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)
    bus = dbus.SystemBus()
    
    bus.add_signal_receiver(
        on_property_changed,
        dbus_interface="org.freedesktop.DBus.Properties",
        signal_name="PropertiesChanged",
        arg0="org.bluez.Device1",
        path_keyword="path"
    )
    
    bus.add_signal_receiver(
        on_interfaces_added,
        dbus_interface="org.freedesktop.DBus.ObjectManager",
        signal_name="InterfacesAdded"
    )
    
    start_discovery()
    list_detected_devices()
    
    
    print("Escaneando... Presiona Ctrl+C para detener.")
    
    try:
        mainloop = GLib.MainLoop()
        mainloop.run()
    except KeyboardInterrupt:
        print("Escaneo detenido por el usuario.")