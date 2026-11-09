import asyncio
from bleak import BleakScanner

async def main():
    print("Scanning for Bluetooth devices...")
    devices = await BleakScanner.discover()
    for d in devices:
        if d.name and "Polar" in d.name:
            print(f"FOUND POLAR: Name={d.name}, Address={d.address}")

asyncio.run(main())