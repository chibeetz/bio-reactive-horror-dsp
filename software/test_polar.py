import asyncio
from bleak import BleakClient

# Standard Bluetooth SIG Heart Rate Measurement UUID
HR_UUID = "00002a37-0000-1000-8000-00805f9b34fb"

def callback(sender, data):
    # Parse standard 2-byte BLE Heart Rate packet
    flags = data[0]
    hr_format = flags & 0x01

    if hr_format == 0:
        bpm = data[1] # 8-bit BPM value
    else:
        bpm = (data[2] << 8) | data[1] # 16-bit BPM value

    print(f"[LIVE HEART RATE] {bpm} BPM")

async def main():
    address = "YOUR_POLAR_ADDRESS_HERE"
    print(f"Connecting to Polar H10 at {address}...")

    async with BleakClient(address) as client:
        print("Connected! Listening for live cardiac stream...")
        await client.start_notify(HR_UUID, callback)
        await asyncio.sleep(30.0) # Run test for 30 seconds
        await client.stop_notify(HR_UUID)

asyncio.run(main())