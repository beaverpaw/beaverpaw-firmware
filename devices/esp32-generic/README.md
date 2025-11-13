# ESP32 Generic Controller

Transform any ESP32 board into a powerful remote-controlled IoT device!

## What Can It Do?

Control these from anywhere in the world:

- **💡 Lights** - Turn on/off, or set any color with RGB LEDs
- **🎨 LED Strips** - Full color control (16 million colors!)
- **⚡ Motors** - Control speed from 0-100%
- **🔄 Steppers** - Precise motor control (like in 3D printers)
- **🔌 Relays** - Control high-power devices safely

Perfect for: Home automation, garden irrigation, pet feeders, art installations, robotics projects

## Hardware You'll Need

### Required:
- ESP32 development board (38-pin version recommended)
- USB cable (for programming)
- 5V power supply

### Optional (choose what you want to control):
- LEDs + resistors (220Ω)
- RGB LED strip (common cathode type)
- DC motors + motor driver
- Stepper motor + driver (A4988 or similar)
- Relay modules
- Breadboard and jumper wires

## Quick Start

### 1. Install Software (5 minutes)

**Download Arduino IDE:**
- Go to [arduino.cc/en/software](https://www.arduino.cc/en/software)
- Download for Windows, Mac, or Linux
- Install and open it

**Add ESP32 Support:**
- In Arduino IDE: File → Preferences
- Find "Additional Board Manager URLs"
- Paste this: `https://dl.espressif.com/dl/package_esp32_index.json`
- Click OK
- Go to: Tools → Board → Boards Manager
- Search for "esp32"
- Click Install on "ESP32 by Espressif Systems"

**Install Required Libraries:**
- Go to: Sketch → Include Library → Manage Libraries
- Install these (search by name):
  - ArduinoJson (version 6.21.0 or newer)
  - PubSubClient (version 2.8.0 or newer)
  - NimBLE-Arduino (version 1.4.0 or newer)
  - ESPAsyncWebServer (version 1.2.0 or newer)
  - AsyncTCP (version 1.1.0 or newer)

### 2. Flash Firmware (5 minutes)

- Download this repository
- Open `esp32-generic.ino` in Arduino IDE
- Connect your ESP32 to computer via USB
- Select: Tools → Board → ESP32 Dev Module
- Select your USB port: Tools → Port → (your ESP32's port)
- Click the Upload button (→)
- Wait for "Done uploading" message

### 3. Connect to WiFi (2 minutes)

**iPhone Users (WiFi Method):**
1. Device creates WiFi network: "BeaverPaw-XXXX"
2. Connect to it from your phone's WiFi settings
3. Setup page opens automatically
4. Enter your home WiFi password
5. Tap "Connect"
6. Done!

**Android Users (Bluetooth Method):**
1. Go to: [https://beaverpaw.com/mobiledevicesetup](https://beaverpaw.com/mobiledevicesetup) (in Chrome browser)
2. Tap "Scan for Devices"
3. Select your device
4. Enter WiFi password
5. Done!

### 4. Claim Your Device (1 minute)

- Log into your BeaverPaw account at [https://beaverpaw.com](https://beaverpaw.com)
- Go to: Dashboard → Devices → Claim Device
- Scan the QR code on your device (or enter Device ID from serial monitor)
- Give it a friendly name
- Click Claim

Your device will automatically fetch its security credentials and come online!

## Wiring Examples

### Simple LED (Easiest)

No wiring needed - just use the built-in LED!

The built-in LED on most ESP32 boards is on GPIO 2.

### External LED

```
ESP32 Pin 5  →  220Ω Resistor  →  LED (+)
LED (-)  →  GND
```

### RGB LED (Common Cathode)

```
ESP32 Pin 12  →  220Ω Resistor  →  Red pin
ESP32 Pin 13  →  220Ω Resistor  →  Green pin
ESP32 Pin 14  →  220Ω Resistor  →  Blue pin
Common cathode (longest pin)  →  GND
```

### Relay Module (for controlling high power)

```
Relay VCC  →  ESP32 5V
Relay GND  →  ESP32 GND
Relay IN   →  ESP32 Pin 5
```

⚠️ **Safety:** Use relays to control mains voltage (110V/220V). Never connect mains directly to ESP32!

### DC Motor (with L293D motor driver)

```
L293D Pin 1 (Enable)  →  ESP32 Pin 25
L293D Pin 2 (Input 1) →  ESP32 Pin 26
L293D Pin 16 (VCC1)   →  ESP32 5V
L293D Pin 8 (VCC2)    →  External 12V
L293D Pins 4,5,12,13  →  GND
Motor connects to L293D output pins
```

### Stepper Motor (with A4988 driver)

```
A4988 STEP  →  ESP32 Pin 18
A4988 DIR   →  ESP32 Pin 19
A4988 EN    →  ESP32 Pin 21
A4988 VDD   →  ESP32 3.3V
A4988 GND   →  ESP32 GND
A4988 VMOT  →  External 12V (motor power)
Motor coils →  A4988 outputs (1A, 1B, 2A, 2B)
```

## Controlling Your Device

Once your device is online, you can control it from:

- 🌐 **Web Dashboard** - [https://beaverpaw.com](https://beaverpaw.com)
- 📱 **Mobile Browser** - Works on any phone
- 🗣️ **Voice** - "Alexa, turn on living room light"
- 🔗 **API** - Integrate with your own apps

### Example Commands

**Turn LED On:**
- Go to dashboard → Select your device → Toggle switch

**Change RGB Color:**
- Use the color picker to set any color
- Or use presets (red, green, blue, purple, etc.)

**Control Motor Speed:**
- Use the slider (0-100%)
- Perfect for fans, pumps, dimming

**Move Stepper:**
- Enter number of steps (e.g., 200 = one full rotation)
- Choose direction (clockwise/counter-clockwise)
- Set speed in RPM

## Troubleshooting

### LED Not Blinking After Upload
- Check USB cable is connected
- Try pressing the RESET button on ESP32
- Re-upload firmware

### Can't Find WiFi Network
- Wait 30 seconds after powering on
- Look for "BeaverPaw-XXXX" (last 4 chars of your device ID)
- Try pressing RESET button

### WiFi Connects But Device Stays Offline
- Make sure you claimed the device first!
- Device needs to be claimed to get security credentials
- Check Serial Monitor for error messages (Tools → Serial Monitor, set to 115200 baud)

### Commands Not Working
- Check wiring connections
- Make sure pins match the code (GPIO 2, 12, 13, etc.)
- Test with multimeter if you have one

**More help:** See [Troubleshooting Guide](../../docs/TROUBLESHOOTING.md)

## Advanced Customization

Want to use different pins? Edit the firmware:

```cpp
// At the top of esp32-generic.ino, change these:
#define BUILTIN_LED 2      // Change to your LED pin
#define RGB_RED_PIN 12     // Change to your red pin
// ... etc
```

## Safety Tips

⚠️ **Important Safety Information:**

- **Power:** ESP32 is 3.3V logic. Don't connect 5V signals to GPIO pins!
- **Current:** Each GPIO pin can handle max 12mA. Use transistors/relays for high-power loads
- **Motors:** Always use external power for motors (don't power from ESP32)
- **Mains Voltage:** NEVER connect 110V/220V directly to ESP32. Use relays!

## Need Help?

- 📧 Email: support@beaverpaw.com
- 💬 Forum: [github.com/beaverpaw/beaverpaw-firmware/discussions](https://github.com/beaverpaw/beaverpaw-firmware/discussions)
- 📚 Docs: [Full documentation](../../docs/SETUP.md)

## What's Next?

- Try controlling your device from the dashboard
- Set up automations and schedules
- Connect voice assistants (Alexa, Google Home)
- Share access with family members
- Build something awesome and share it with the community!

---

Happy building! 🛠️ Questions? We're here to help.
