import asyncio
import struct
import serial
from bleak import BleakClient, BleakScanner
from pythonosc import udp_client
import serial.tools.list_ports

# 1. Serial setup to Teensy (Update COM port/device path for your machine)
# Windows: 'COM3' | macOS: '/dev/tty.usbmodem...'
def find_teensy_port():
    ports = serial.tools.list_ports.comports()
    for p in ports:
        # Looks for Teensy device descriptors or macOS usbmodem paths
        if "Teensy" in p.description or "usbmodem" in p.device:
            return p.device
    return None

# Auto-detect Teensy on macOS or Windows
TEENSY_PORT = find_teensy_port()

if TEENSY_PORT:
    try:
        teensy = serial.Serial(TEENSY_PORT, 115200)
        print(f"[OK] Connected to Teensy 4.1 on {TEENSY_PORT}")
    except Exception as e:
        print(f"[WARNING] Could not open Serial port {TEENSY_PORT}: {e}")
        teensy = None
else:
    print("[WARNING] Teensy 4.1 not detected! Check USB connection.")
    teensy = None

# 2. OSC setup for Unreal Engine 5 (Port 7000)
ue5_osc = udp_client.SimpleUDPClient("127.0.0.1", 7000)

# Standard BLE Heart Rate Measurement UUID
HR_UUID = "00002a37-0000-1000-8000-00805f9b34fb"

def handle_heart_rate(sender, data):
    # Parse standard BLE HR format byte
    flags = data[0]
    bpm = data[1] if (flags & 0x01) == 0 else (data[2] << 8) | data[1]

    # Calculate stress ratio (0.0 to 1.0)
    stress_level = min(max((bpm - 60) / 100.0, 0.0), 1.0)

    # Output A: Send OSC message to Unreal Engine 5
    ue5_osc.send_message("/filter/stress", stress_level)

    # Output B: Write binary packet to Teensy 4.1 [Header: 0xA5, Payload: BPM]
    if teensy and teensy.is_open:
        teensy.write(bytes([0xA5, bpm]))

    print(f"[BIO-FEEDBACK] Heart Rate: {bpm} BPM | Stress Normalized: {stress_level:.2f}")

async def main():
    print("Searching for Polar H10...")
    devices = await BleakScanner.discover()
    polar_device = next((d for d in devices if d.name and "Polar" in d.name), None)

    if not polar_device:
        print("[ERROR] Could not find Polar H10! Make sure strap is wet and on your chest.")
        return

    print(f"Connecting to {polar_device.name} ({polar_device.address})...")
    async with BleakClient(polar_device.address) as client:
        print("[CONNECTED] Streaming live bio-data to Teensy 4.1 and Unreal Engine 5...")
        await client.start_notify(HR_UUID, handle_heart_rate)

        # Keep process running
        while True:
            await asyncio.sleep(1)

if __name__ == "__main__":
    asyncio.run(main())