from bluepy.btle import Peripheral, UUID
from bluepy.btle import Scanner, DefaultDelegate

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)
    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            print ("Discovered device", dev.addr)
        elif isNewData:
            print ("Received new data from", dev.addr)

class NotifyDelegate(DefaultDelegate):
    def __init__(self, params):
        DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        print("Notification from Handle: 0x" + format(cHandle, '02X'))
        print(int.from_bytes(data, "big"))


scanner = Scanner().withDelegate(ScanDelegate())
devices = scanner.scan(10.0)
n=0
addr = []
for dev in devices:
    print ("%d: Device %s (%s), RSSI=%d dB " % (n, dev.addr, dev.addrType, dev.rssi))
    addr.append(dev.addr)
    n += 1
    for (adtype, desc, value) in dev.getScanData():
        print (" %s = %s" % (desc, value))

number = input('Enter your device number: ')
print ('Device', number)
num = int(number)
print (addr[num])
#
print ("Connecting...")
dev = Peripheral(addr[num], 'random') #
with open("output.txt", "w") as f:
    for des in dev.getDescriptors():
        f.write(str(des))

print ("Services...")
for svc in dev.services:
    print (str(svc)) #
    print (type(svc))
    for ch in svc.getCharacteristics():
        print(str(ch))
print("Service end")
try:
    testService = dev.getServiceByUUID(UUID(0x1809))
    heartService = dev.getServiceByUUID(UUID(0x180d))
    for ch in testService.getCharacteristics():
        print (str(ch)) #

    ch1 = dev.getCharacteristics(uuid=UUID(0x2719))[0]
    ch2 = dev.getCharacteristics(uuid=UUID(0x271A))[0]
    ch3 = dev.getCharacteristics(uuid=UUID(0x271B))[0]
    heart_ch = dev.getCharacteristics(uuid=UUID(0x2a37))[0]

    ch1_notify_handle = ch1.getHandle()+1
    ch2_notify_handle = ch2.getHandle()+1
    ch3_notify_handle = ch3.getHandle()+1
    heart_ch_notify_handle = heart_ch.getHandle()+1

    dev.writeCharacteristic(ch1_notify_handle, b"\x01\x00")
    dev.writeCharacteristic(ch2_notify_handle, b"\x01\x00")
    dev.writeCharacteristic(ch3_notify_handle, b"\x01\x00")
    dev.writeCharacteristic(heart_ch_notify_handle, b"\x01\x00")

    dev.setDelegate(NotifyDelegate(ch1.getHandle()))
    dev.setDelegate(NotifyDelegate(ch2.getHandle()))
    dev.setDelegate(NotifyDelegate(ch3.getHandle()))
    dev.setDelegate(NotifyDelegate(heart_ch.getHandle()))

    while True:
        if (dev.waitForNotifications(1.0)):
            print("Received notification!")
            continue

finally:
    dev.disconnect()
