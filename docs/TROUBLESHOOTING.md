# Troubleshooting Guide

Having trouble? Don't worry - we've got you covered! This guide solves 99% of issues.

## Quick Diagnostics

### What's Your LED Doing?

Look at your device's LED. The blinking pattern tells you what's happening:

| LED Pattern | What It Means | What To Do |
|-------------|---------------|------------|
| **Fast blink** (5x per second) | Setup mode active | ✅ Good! Connect to setup WiFi or Bluetooth |
| **Slow blink** (1x per second) | Trying to connect to WiFi | Wait 30 seconds, should connect |
| **Solid ON** | Connected to cloud | ✅ Perfect! Device is online |
| **OFF** | No power or crashed | Check power supply, try RESET button |

## WiFi Access Point Issues (iPhone Method)

### Problem: Can't Find "BeaverPaw-XXXX" Network

**Solution 1: Wait and Refresh**
- Device takes 30-45 seconds to create WiFi network
- Pull down to refresh WiFi list on your phone
- Look carefully - network name is "BeaverPaw-" + 4 characters

**Solution 2: Reset Device**
- Press the RESET button on your device
- Wait 45 seconds
- Check WiFi list again

**Solution 3: Check LED**
- LED should be blinking rapidly (fast blink)
- If not blinking, device might already be configured
- Try power cycling (unplug and plug back in)

**Solution 4: Check If Already Configured**
- Device might already have WiFi saved
- Check your dashboard - is it already showing online?
- To force setup mode: re-flash firmware

### Problem: Setup Page Doesn't Open Automatically

**Solution 1: Manual Navigation**
- Connect to BeaverPaw WiFi first
- Open Safari or Chrome
- Type in address bar: **192.168.4.1**
- Press Go

**Solution 2: iPhone Specific**
- After connecting to BeaverPaw WiFi
- Open Settings → WiFi
- Tap the (i) icon next to BeaverPaw network
- Look for "Sign in to network" or similar
- Tap it

**Solution 3: Android Specific**
- Pull down notification shade
- Look for "Network requires sign-in" notification
- Tap it

**Solution 4: Disable Cellular Data**
- Turn off mobile data temporarily
- Phone will be forced to use WiFi
- Captive portal should appear

### Problem: WiFi Password Accepted But Device Doesn't Connect

**Solution 1: Double-Check Password**
- Passwords are case-sensitive!
- Check for spaces before/after password
- Try typing carefully again

**Solution 2: Verify 2.4GHz Network**
- ESP32 ONLY works with 2.4GHz WiFi
- 5GHz will NOT work
- Check router settings or try different network
- Many routers have separate 2.4GHz and 5GHz networks

**Solution 3: Check Router Compatibility**
- Some routers block IoT devices by default
- Look for "IoT Network" or "Guest Network" in router settings
- Disable "AP Isolation" if enabled
- Try connecting other smart devices - if they work, ESP32 should too

**Solution 4: Simplify Network Name**
- Some special characters cause problems
- Try renaming WiFi to use only: letters, numbers, hyphen, underscore
- Avoid: @ # $ % ^ & * ( ) [ ]

### Problem: WiFi Connects But Device Stays Offline

**Solution 1: Claim Device First! (Most Common)**
- Device MUST be claimed before it can get credentials
- Go to: Dashboard → Devices → Claim Device
- Scan QR code or enter Device ID
- This generates the security credentials
- Then restart your device

**Solution 2: Be Patient**
- After WiFi connects, device polls cloud for credentials
- This takes 30-90 seconds
- Watch the serial monitor for "✓ Credentials received" message
- Check dashboard after 2-3 minutes

**Solution 3: Check Network Restrictions**
- Device needs HTTPS access to internet
- Some corporate/school networks block this
- Try on home WiFi first for testing
- Check if other smart devices work on your network

## Bluetooth Issues (Android Method)

### Problem: "Bluetooth Not Supported" Message

**Solution 1: Use Supported Browser**

✅ **These work:**
- Chrome (recommended)
- Edge
- Opera

❌ **These DON'T work:**
- Safari (use WiFi method instead)
- Firefox
- Samsung Internet

**Solution 2: iPhone Users**
- iPhone doesn't support Web Bluetooth at all
- Use WiFi Access Point method instead
- See "WiFi Access Point" section above

### Problem: No Devices Found When Scanning

**Solution 1: Enable Bluetooth**
- Android: Settings → Bluetooth → ON
- Also enable Location Services (Android requirement for BLE)

**Solution 2: Check Device is Ready**
- LED should be blinking
- If solid, device might already be configured
- Power cycle to restart Bluetooth

**Solution 3: Get Closer**
- Bluetooth range is about 10 meters (30 feet)
- Walls and metal reduce range
- Try within 2-3 meters for first connection

**Solution 4: Grant Permissions**
- Browser needs Bluetooth permission
- Look for permission popup when you click Scan
- If blocked, go to browser settings and allow Bluetooth
- Chrome: Settings → Site Settings → Bluetooth

### Problem: Connection Drops During Setup

**Solution 1: Stay Close**
- Don't walk away during setup
- Keep phone within 2 meters of device
- Don't lock phone screen during credential transfer

**Solution 2: Retry**
- Bluetooth can be finicky
- Refresh page and try again
- Usually works on 2nd or 3rd attempt

**Solution 3: Use WiFi Method Instead**
- If Bluetooth keeps failing
- Switch to WiFi Access Point method
- Works more reliably on some phones

## Firmware Upload Issues

### Problem: "Failed to Connect" During Upload

**For ESP32 Generic:**

**Solution 1: Press BOOT Button**
- Some ESP32 boards need this
- Hold down BOOT button
- Click Upload in Arduino
- Keep holding until upload starts
- Release button

**Solution 2: Try Different Cable**
- Must be a DATA cable, not charge-only
- Try another USB cable
- Try different USB port on computer

**Solution 3: Install Drivers**
- Windows may need CP210x or CH340 drivers
- Google "CP210x driver download"
- Install and restart computer

**For ESP32-CAM:**

**Solution 1: GPIO 0 to GND**
- MUST connect GPIO 0 to GND for upload
- Use a jumper wire
- Remove after upload completes!

**Solution 2: Press RESET at Right Time**
- Click Upload in Arduino
- Wait for "Connecting......" message
- THEN press RESET button on ESP32-CAM
- Works almost every time!

**Solution 3: Slower Upload Speed**
- Tools → Upload Speed → **115200**
- Slower is more reliable
- Try again

### Problem: Code Won't Compile

**Solution 1: Install All Libraries**
- Go back to Part 3 of setup guide
- Make sure ALL 5 libraries are installed
- Check version numbers match

**Solution 2: Update ESP32 Board**
- Tools → Board → Boards Manager
- Find ESP32
- Update to latest version

**Solution 3: Restart Arduino IDE**
- Sometimes it just needs a restart
- Close and reopen Arduino IDE

## Device Operation Issues

### Problem: Device Was Working, Now Offline

**Solution 1: WiFi Router Restarted**
- Device will reconnect automatically
- Wait 2-3 minutes
- Should come back online

**Solution 2: Power Issue**
- Check power supply is connected
- Try different power source
- Make sure voltage is 5V (not 3.3V or 12V)

**Solution 3: Too Far From Router**
- Move device closer to WiFi router
- Or add WiFi extender
- Check signal strength in dashboard

### Problem: Commands Not Working

**For ESP32 Generic:**

**Check Wiring:**
- LED connected to right pin? (default is GPIO 2)
- Resistors in place? (220Ω for LEDs)
- Power connected?

**Check Dashboard:**
- Is device showing as "Online"?
- Try the command again
- Check serial monitor for error messages

**For Security Cube:**

**Camera Issues:**
- Check camera cable is fully inserted
- Make sure using good 2A power supply
- Camera needs a lot of power!

## Factory Reset

Need to start fresh? Here's how:

### Method 1: Re-Flash Firmware
- Re-upload firmware from Arduino IDE
- This clears all saved settings
- Device will start like new

### Method 2: Clear Flash (Advanced)
- Install esptool: `pip install esptool`
- Run: `esptool.py --port COM3 erase_flash`
- Replace COM3 with your port
- Re-upload firmware

## Error Messages Explained

### "Camera init failed: 0x105"
- Camera cable loose or damaged
- Try disconnecting and reconnecting camera
- Use better power supply (2A minimum)

### "MQTT connection failed, rc=-2"
- Security credentials wrong
- Re-claim device in dashboard
- Download new config.h

### "WiFi connection failed"
- Wrong password
- Or network is 5GHz (need 2.4GHz)
- Check router settings

## Still Stuck?

### Before Contacting Support

Please collect this info:

1. **Device Model:** ESP32 Generic or Security Cube?
2. **Device ID:** (shown in serial monitor or on QR code)
3. **What's happening:** Describe the problem
4. **LED pattern:** What is the LED doing?
5. **Serial Monitor output:** Copy/paste any error messages
6. **What you've tried:** Which solutions above did you try?

### Contact Us

**📧 Email Support:** support@beaverpaw.com
- Include the info above
- We'll respond within 24 hours (usually faster!)
- We're friendly and here to help

**💬 Community Forum:** [Ask the Community](https://github.com/beaverpaw/beaverpaw-firmware/discussions)
- Other users might have solved your exact problem
- Share solutions that worked for you
- Help others too!

**🐛 Bug Report:** [GitHub Issues](https://github.com/beaverpaw/beaverpaw-firmware/issues)
- If you think you found a bug in the firmware
- We'll investigate and fix it

## Tips for Success

✅ **Take your time** - rushing causes mistakes  
✅ **Read error messages** - they usually tell you what's wrong  
✅ **Try the simplest solution first** - usually it's something simple  
✅ **Check connections twice** - loose wires are common  
✅ **Use good power supply** - cheap power = weird problems  
✅ **Ask for help** - we're friendly and want you to succeed!

---

Remember: Everyone gets stuck sometimes. Don't give up - the solution is usually simpler than you think! 🐕
