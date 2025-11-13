# Complete Setup Guide

Welcome! This guide will walk you through setting up your BeaverPaw device from start to finish. No technical experience needed - we'll explain everything step by step.

## What You'll Need

✅ Your BeaverPaw device  
✅ A computer (Windows, Mac, or Linux)  
✅ A USB cable  
✅ WiFi network (must be 2.4GHz, not 5GHz)  
✅ 15-20 minutes  
✅ A phone (iPhone or Android - both work!)  

## Part 1: Install Arduino IDE (5 minutes)

Arduino IDE is free software that lets you program your ESP32 board.

### Step 1: Download

- Go to: [arduino.cc/en/software](https://www.arduino.cc/en/software)
- Click the download button for your operating system
- Windows: Download the installer (.exe file)
- Mac: Download the .dmg file
- Linux: Download the .AppImage file

### Step 2: Install

- **Windows:** Run the installer, click "I Agree", then "Install"
- **Mac:** Open the .dmg and drag Arduino to Applications folder
- **Linux:** Make the .AppImage executable and run it

### Step 3: Open Arduino IDE

- Find Arduino IDE in your applications and open it
- You should see a window with a code editor

✅ **Checkpoint:** You should see Arduino IDE open with a blank sketch.

## Part 2: Add ESP32 Support (5 minutes)

Your ESP32 board needs special software to work with Arduino IDE.

### Step 1: Open Preferences

- Click: **File** → **Preferences** (or Arduino IDE → Settings on Mac)

### Step 2: Add ESP32 Board Manager URL

- Find the box labeled "Additional Board Manager URLs"
- Paste this entire line:
  ```
  https://dl.espressif.com/dl/package_esp32_index.json
  ```
- Click **OK**

### Step 3: Install ESP32 Board

- Click: **Tools** → **Board** → **Boards Manager**
- In the search box, type: **esp32**
- Find "ESP32 by Espressif Systems"
- Click **Install** (may take a few minutes)
- Close the Boards Manager when done

✅ **Checkpoint:** You should now see ESP32 boards in Tools → Board menu.

## Part 3: Install Required Libraries (5 minutes)

Libraries are pre-written code that makes the firmware work.

### Step 1: Open Library Manager

- Click: **Sketch** → **Include Library** → **Manage Libraries**

### Step 2: Install Each Library

Search for and install these (one at a time):

1. **ArduinoJson**
   - Search: ArduinoJson
   - Install version 6.21.0 or newer

2. **PubSubClient**
   - Search: PubSubClient
   - Install version 2.8.0 or newer

3. **NimBLE-Arduino**
   - Search: NimBLE-Arduino
   - Install version 1.4.0 or newer

4. **ESPAsyncWebServer**
   - Search: ESPAsyncWebServer
   - Install version 1.2.0 or newer

5. **AsyncTCP**
   - Search: AsyncTCP
   - Install version 1.1.0 or newer

### Step 3: Close Library Manager

✅ **Checkpoint:** All 5 libraries should show "INSTALLED".

## Part 4: Download Firmware (2 minutes)

### Option A: Download ZIP (Easier)

- Go to: [github.com/beaverpaw/beaverpaw-firmware](https://github.com/beaverpaw/beaverpaw-firmware)
- Click the green **Code** button
- Click **Download ZIP**
- Extract the ZIP file to your Documents folder

### Option B: Use Git (If You Know Git)

```bash
git clone https://github.com/beaverpaw/beaverpaw-firmware.git
```

✅ **Checkpoint:** You should have a folder called "beaverpaw-firmware" with "devices" folder inside.

## Part 5: Flash Firmware (5 minutes)

Now we'll upload the firmware to your device.

### For ESP32 Generic (Standard ESP32 Board):

**Step 1: Open Firmware**
- In Arduino IDE: File → Open
- Navigate to: beaverpaw-firmware/devices/esp32-generic/
- Open: **esp32-generic.ino**

**Step 2: Select Board**
- Tools → Board → ESP32 Arduino → **ESP32 Dev Module**

**Step 3: Connect Device**
- Plug ESP32 into computer via USB cable
- Tools → Port → Select your ESP32 (usually called "USB Serial" or "COM3/COM4")

**Step 4: Upload**
- Click the **Upload** button (right arrow →)
- Wait 1-2 minutes
- Look for "Done uploading" at the bottom

✅ **Success!** Your device is now programmed.

### For Security Cube (ESP32-CAM):

ESP32-CAM is a bit trickier because it doesn't have USB built-in.

**Step 1: Wire Up FTDI Adapter**

| ESP32-CAM | FTDI Adapter |
|-----------|--------------|
| 5V        | VCC (5V)     |
| GND       | GND          |
| U0R       | TX           |
| U0T       | RX           |
| GPIO 0    | GND          |

⚠️ **Important:** GPIO 0 to GND is only for uploading. Remove this connection afterward!

**Step 2: Open Firmware**
- File → Open
- Navigate to: beaverpaw-firmware/devices/security-cube/
- Open: **security-cube.ino**

**Step 3: Select Board**
- Tools → Board → ESP32 Arduino → **AI Thinker ESP32-CAM**

**Step 4: Upload**
- Click Upload button
- When you see "Connecting......" → **Press RESET button** on ESP32-CAM
- Wait for "Done uploading"
- **Disconnect GPIO 0 from GND wire**
- Press RESET button again to start device

✅ **Success!** Your camera is now programmed.

## Part 6: Connect to WiFi (2 minutes)

Your device needs WiFi to connect to the cloud. Choose the method that works for your phone:

### Method 1: WiFi Access Point (iPhone/All Phones)

**This is the easiest method and works on ALL phones!**

1. **Power on your device**
   - Plug in USB power or batteries
   - Wait 30 seconds

2. **Find the WiFi network**
   - On your phone: Settings → WiFi
   - Look for network named: **BeaverPaw-XXXX**
   - (XXXX = last 4 characters of your device ID)

3. **Connect to it**
   - Tap the BeaverPaw network
   - No password needed
   - Wait a few seconds

4. **Setup page opens automatically**
   - Should pop up like hotel WiFi
   - If not, open browser and go to: **192.168.4.1**

5. **Enter your home WiFi**
   - Enter your WiFi network name
   - Enter your WiFi password
   - Click "Connect My Device"
   - Wait for success message

6. **Return to your home WiFi**
   - Go back to WiFi settings
   - Reconnect to your home WiFi
   - Device will connect automatically in background

✅ **Done!** Your device is now on your WiFi network.

### Method 2: Bluetooth (Android/Chrome Users)

1. Open Chrome browser on your Android phone
2. Go to: **app.beaverpaw.com/setup**
3. Tap "Scan for Devices"
4. Select your BeaverPaw device
5. Enter WiFi name and password
6. Tap Connect

✅ **Done!** Device connects to WiFi.

## Part 7: Claim Your Device (1 minute)

This step links the device to your BeaverPaw account.

1. **Log into BeaverPaw**
   - Go to: app.beaverpaw.com
   - Sign in with your account

2. **Go to Claim Device**
   - Dashboard → Devices → Claim Device

3. **Scan QR Code OR Enter ID**
   - **Option A:** Scan the QR code sticker on your device
   - **Option B:** Type in the Device ID (shown in serial monitor)

4. **Give it a name**
   - Example: "Front Door Camera" or "Living Room Light"

5. **Click Claim**

The device will automatically fetch its security credentials and connect to the cloud!

✅ **Success!** Your device should show as "Online" within 1-2 minutes.

## Part 8: Start Using It!

Go to Dashboard → Devices → Click your device

**For ESP32 Generic:**
- Toggle LEDs on/off
- Change RGB colors
- Control motor speeds
- Move steppers

**For Security Cube:**
- Watch live video
- Review recorded clips
- Set up motion alerts
- Configure recording schedules

## Troubleshooting

### WiFi Network Not Showing Up

**Wait longer:**
- Device takes 30-45 seconds to start
- Try pressing RESET button
- Look for rapid LED blinking (means setup mode is active)

**Power cycle:**
- Unplug device
- Wait 10 seconds
- Plug back in
- Wait 45 seconds

### Setup Page Doesn't Open

**Try manually:**
- Connect to BeaverPaw WiFi first
- Open your phone's browser
- Type in address bar: **192.168.4.1**
- Setup page should load

**iPhone users:**
- Go to Settings → WiFi
- Tap the (i) icon next to BeaverPaw network
- Look for "Sign in to network" button
- Tap it

**Android users:**
- Pull down notification shade
- Look for "Network requires sign-in" notification
- Tap it

### WiFi Connection Fails

**Check these:**
- ✓ WiFi password is correct (case-sensitive!)
- ✓ Network is 2.4GHz (not 5GHz) - ESP32 doesn't support 5GHz
- ✓ No special characters in WiFi name
- ✓ Router allows new devices to connect

### Device Shows Offline in Dashboard

**Most common causes:**

1. **Device not claimed yet**
   - Go to Dashboard → Claim Device
   - Scan QR code or enter Device ID

2. **Still connecting**
   - First connection takes 1-2 minutes
   - Be patient!
   - Check back in 5 minutes

3. **WiFi lost connection**
   - Check device is still powered on
   - Make sure WiFi router is working
   - Try rebooting device

### Upload Failed

**ESP32 Generic:**
- Try different USB cable (must be data cable, not charge-only)
- Try different USB port
- Press and hold BOOT button during upload

**ESP32-CAM:**
- Make sure GPIO 0 connected to GND before upload
- Press RESET button when "Connecting..." appears
- Check FTDI adapter has correct drivers installed

## Advanced: Serial Monitor

Want to see what your device is doing? Use the Serial Monitor!

1. Tools → Serial Monitor
2. Set baud rate to: **115200**
3. You'll see debug messages from your device

This is super helpful for troubleshooting!

## Getting Help

Stuck? We're here to help!

- **📧 Email:** support@beaverpaw.com
  - We usually respond within 24 hours
  - Include your device ID and what's not working

- **💬 Community Forum:** [GitHub Discussions](https://github.com/beaverpaw/beaverpaw-firmware/discussions)
  - Ask questions
  - Share your projects
  - Help others

- **🐛 Found a Bug?** [Report it](https://github.com/beaverpaw/beaverpaw-firmware/issues)

## Tips for Success

✅ **Take your time** - rushing causes mistakes  
✅ **Read error messages** - they usually tell you what's wrong  
✅ **Try the simplest solution first** - usually it's something simple  
✅ **Check connections twice** - loose wires are common  
✅ **Use good power supply** - cheap power = weird problems  
✅ **Ask for help** - we're friendly and want you to succeed!

---

Remember: Everyone gets stuck sometimes. Don't give up - the solution is usually simpler than you think! 🐕
