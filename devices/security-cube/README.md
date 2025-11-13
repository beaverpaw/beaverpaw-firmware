# Security Cube Camera

Turn your home security up a notch with a smart camera you can actually trust!

## What Is It?

The Security Cube is a smart security camera built on an ESP32-CAM module. Unlike commercial cameras that send your video to unknown servers, this one sends everything to YOUR Amazon cloud account. You own your data, period.

## What Can It Do?

- **📹 Live Video** - Watch from anywhere on your phone
- **🚨 Motion Alerts** - Get notified instantly when something moves
- **🔒 Secure** - Bank-level encryption, your data stays private
- **🌙 Night Vision** - IR illumination for dark areas (with IR LEDs)
- **💾 Recording** - Save clips to MicroSD card
- **🔋 Battery Option** - Can run on battery power

Perfect for: Home security, baby monitoring, pet cam, package delivery alerts

## Hardware You'll Need

### Required:
- **ESP32-CAM module** (AI-Thinker version recommended)
- **USB-to-Serial adapter** (FTDI, CP2102, or CH340G)
- **5V/2A power supply** (camera needs good power!)
- **MicroSD card** (optional, for local storage)

### Where to Buy:
- Amazon: Search "ESP32-CAM AI-Thinker"
- AliExpress, eBay also have them
- Cost: About $10-15

## Quick Start

### 1. Install Software

Follow the same Arduino IDE setup as ESP32 Generic (see that guide).

Install the same libraries PLUS the camera library (built-in with ESP32 board support).

### 2. Hardware Setup for Programming

ESP32-CAM doesn't have a USB port, so you need an adapter:

**Wiring Table:**

| ESP32-CAM Pin | FTDI Adapter Pin |
|---------------|------------------|
| 5V            | VCC (5V)         |
| GND           | GND              |
| U0R           | TX               |
| U0T           | RX               |
| GPIO 0        | GND (upload only!)|

**Important:** Only connect GPIO 0 to GND when uploading firmware. Disconnect it afterward!

### 3. Upload Firmware

- Open `security-cube.ino` in Arduino IDE
- Select: Tools → Board → AI Thinker ESP32-CAM
- Select your USB-to-Serial adapter port
- **Connect GPIO 0 to GND** (see wiring above)
- Click Upload
- When you see "Connecting......" in the console, **press the RESET button** on ESP32-CAM
- Wait for "Done uploading"
- **Disconnect GPIO 0 from GND**
- Press RESET button again to boot normally

### 4. Setup WiFi

**iPhone/iPad Users:**
1. Device creates WiFi: "BeaverPaw-CUBE-XXXX"
2. Connect to it
3. Setup page opens automatically
4. Enter your WiFi password
5. Done!

**Android Users:**
- Go to [https://beaverpaw.com/mobiledevicesetup](https://beaverpaw.com/mobiledevicesetup) in Chrome
- Use Bluetooth method (same as ESP32 Generic)
- Or use WiFi method above (both work!)

### 5. Claim Your Device

- Login to BeaverPaw at [https://beaverpaw.com](https://beaverpaw.com)
- Claim Device → Scan QR code (or enter ID)
- Device auto-configures and comes online!

## Using Your Camera

### View Live Stream
- Dashboard → Devices → Click your Security Cube
- Click "Live View"
- Stream loads in browser

### Set Up Motion Alerts
- Dashboard → Alerts → Create Alert Rule
- Select your camera
- Choose "Motion Detected"
- Set notification method (email, SMS, push)
- Save

You'll get instant notifications when motion is detected!

### Recording
- Recordings are stored in your AWS account
- Access via Dashboard → Recordings
- Download anytime

## Mounting Your Camera

### Indoor Mounting:
- Use 3M strips or small screws
- Point at entry points (doors, windows)
- Keep out of reach (device has no security features if accessed physically)

### Outdoor Mounting:
- Use weatherproof enclosure
- Ensure WiFi signal reaches location
- Consider solar panel + battery for power

## Camera Settings

You can adjust these in the firmware code:

**Resolution:**
```cpp
config.frame_size = FRAMESIZE_VGA;     // 640x480 (good balance)
// Options: QVGA, VGA, SVGA, XGA, HD, FHD
```

**Quality:**
```cpp
config.jpeg_quality = 12;   // 0-63 (lower = better quality, bigger file)
```

**Frame Rate:**
Lower quality number = higher frame rate but uses more bandwidth.

## Power Consumption

| Mode | Current Draw | Battery Life* |
|------|--------------|---------------|
| Streaming | ~200mA | ~5 hours |
| Idle (WiFi on) | ~80mA | ~12 hours |
| Deep sleep | ~10mA | ~100 hours |

*With 1000mAh battery

## LED Indicators

| LED Pattern | Meaning |
|-------------|---------|
| Fast blink | Setup mode (waiting for WiFi) |
| Slow blink | Connecting to WiFi |
| Solid on | Connected to cloud ✓ |
| Off | Error or no power |

## Privacy & Security

**Your video, your control:**
- Video streams to YOUR AWS account only
- We can't see your video (even if we wanted to)
- Encrypted in transit and at rest
- You can delete anytime
- No third-party access

**Open source:**
- Review all code yourself
- No hidden backdoors
- Community audited

## Troubleshooting

### "Camera init failed" Error

**Try these in order:**
1. Check camera ribbon cable is fully inserted
2. Make sure you selected "AI Thinker ESP32-CAM" board
3. Use a 2A power supply (camera needs good power)
4. Try different USB cable
5. Lower resolution in code: `FRAMESIZE_QVGA`

### Upload Fails

- Make sure GPIO 0 is connected to GND during upload
- Press RESET button when console shows "Connecting....."
- Try upload speed 115200 (slower is more reliable)
- Check FTDI adapter has drivers installed

### No Video Stream

- Check camera is initialized (look for ✓ in serial monitor)
- Verify device shows "online" in dashboard
- Try lower quality/resolution
- Check internet upload speed (need at least 1 Mbps)

**More help:** [Troubleshooting Guide](../../docs/TROUBLESHOOTING.md)

## Example Projects

**Baby Monitor:**
- Mount camera over crib
- Set up motion alerts
- Check on baby from anywhere

**Package Delivery Monitor:**
- Point at front door
- Get alerts when packages arrive
- Review footage if something goes missing

**Pet Cam:**
- Watch your pet while at work
- Get motion alerts when they're active
- Share access with pet sitter

## Going Further

**Want to add:**
- PIR motion sensor → More accurate detection
- IR LEDs → Night vision
- Microphone → Two-way audio (ESP32-CAM doesn't have mic, but you can add one)
- SD card → Local backup recording

All these are possible! Check our community forum for projects.

## Support

We're here to help:
- 📧 Email: support@beaverpaw.com (24hr response)
- 💬 Forum: [Community discussions](https://github.com/beaverpaw/beaverpaw-firmware/discussions)
- 🐛 Issues: [Report bugs](https://github.com/beaverpaw/beaverpaw-firmware/issues)

## Safety & Legal

- **Privacy Laws:** Check local laws before recording
- **Notification:** Inform people if area is monitored
- **Indoor Use:** Recommended for home use
- **Power:** Use proper 5V/2A supply, not USB from computer

---

Questions? Ask us anything! We love helping people build cool stuff. 🐕
