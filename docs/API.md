# API Reference

This guide explains how BeaverPaw devices communicate with the cloud. Useful if you want to integrate with other services or build custom apps.

## Overview

Devices use **MQTT** (a lightweight messaging protocol) over **secure TLS** (bank-level encryption) to talk to AWS IoT Cloud.

**Simple version:** Your device sends messages to the cloud and receives commands back. All messages are encrypted.

## Message Topics

Think of topics like addresses for mail. Each message goes to a specific topic.

### Topics Your Device Uses

**Sending Data (Device → Cloud):**

```
devices/YOUR-DEVICE-ID/data
```

Your device sends sensor readings and status updates here every 10 seconds.

**Receiving Commands (Cloud → Device):**

```
devices/YOUR-DEVICE-ID/commands  
```

Your device listens here for commands from the dashboard or API.

## Message Formats

All messages are in JSON format (a simple text format for data).

### Status Update (Device → Cloud)

**When sent:** Every 10 seconds  
**What it contains:** Current state of everything

```json
{
  "device_id": "ESP32-A1B2C3D4",
  "version": "2.0.0",
  "temperature": 24.5,
  "battery": 87,
  "rssi": -45,
  "uptime": 3600,
  "pin_2": 1,
  "rgb_r": 255,
  "rgb_g": 0,
  "rgb_b": 0
}
```

**Field explanations:**
- `device_id`: Your unique device identifier
- `version`: Firmware version (useful for debugging)
- `temperature`: Degrees Celsius
- `battery`: Percentage (0-100)
- `rssi`: WiFi signal strength (more negative = weaker)
- `uptime`: Seconds since last restart
- `pin_X`: State of pin X (0=off, 1=on)
- `rgb_r/g/b`: RGB values (0-255)

### Commands (Cloud → Device)

**When sent:** When you click a button in dashboard or call API

#### Turn LED On

```json
{
  "command": "set_pin",
  "pin": 2,
  "value": 1
}
```

- `pin`: Which GPIO pin (2 = built-in LED)
- `value`: 1 for ON, 0 for OFF

#### Turn LED Off

```json
{
  "command": "set_pin",
  "pin": 2,
  "value": 0
}
```

#### Toggle LED

```json
{
  "command": "toggle_pin",
  "pin": 2
}
```

Switches from current state (if on → off, if off → on)

#### Blink LED

```json
{
  "command": "blink_pin",
  "pin": 2,
  "count": 5,
  "delay": 200
}
```

- `count`: How many times to blink
- `delay`: Milliseconds between blinks (200 = 0.2 seconds)

#### Set RGB Color

```json
{
  "command": "set_rgb",
  "r": 255,
  "g": 0,
  "b": 0
}
```

- `r`: Red value (0-255, 0=off, 255=full)
- `g`: Green value (0-255)
- `b`: Blue value (0-255)

**Color examples:**
- Red: r=255, g=0, b=0
- Green: r=0, g=255, b=0
- Blue: r=0, g=0, b=255
- Purple: r=128, g=0, b=255
- White: r=255, g=255, b=255
- Off: r=0, g=0, b=0

#### Control PWM (Motor Speed, LED Dimming)

```json
{
  "command": "set_pwm",
  "pin": 25,
  "duty": 75
}
```

- `pin`: Which GPIO pin (25 or 26 for PWM)
- `duty`: Speed/brightness (0-100%)

**Examples:**
- 0 = off
- 50 = half speed
- 100 = full speed

#### Move Stepper Motor

```json
{
  "command": "move_stepper",
  "steps": 200,
  "direction": 1,
  "speed": 60
}
```

- `steps`: How many steps to move (200 = 1 full rotation for most motors)
- `direction`: 1 for clockwise, 0 for counter-clockwise
- `speed`: RPM (rotations per minute)

#### Capture Image (Security Cube Only)

```json
{
  "command": "capture_image"
}
```

Camera takes a photo and sends it to cloud.

## For Developers: Using the API

Want to control your device from your own code? Here's how:

### Using REST API

**Get Device Status:**
```bash
curl -H "Authorization: Bearer YOUR_API_KEY" \
  https://beaverpaw.com/api/devices/YOUR-DEVICE-ID
```

**Send Command:**
```bash
curl -X POST \
  -H "Authorization: Bearer YOUR_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{"command":"set_pin","pin":2,"value":1}' \
  https://beaverpaw.com/api/devices/YOUR-DEVICE-ID/commands
```

### Using Python

```python
import requests

# Turn LED on
response = requests.post(
    'https://beaverpaw.com/api/devices/ESP32-ABC123/commands',
    headers={'Authorization': 'Bearer YOUR_API_KEY'},
    json={'command': 'set_pin', 'pin': 2, 'value': 1}
)

print(response.json())
```

### Using Node.js

```javascript
const axios = require('axios');

// Set RGB color to purple
await axios.post(
  'https://beaverpaw.com/api/devices/ESP32-ABC123/commands',
  {
    command: 'set_rgb',
    r: 128,
    g: 0,
    b: 255
  },
  {
    headers: { 'Authorization': 'Bearer YOUR_API_KEY' }
  }
);
```

## Rate Limits

To keep things running smoothly:

- **Device → Cloud:** Max 1 message per second
- **Cloud → Device:** Unlimited (but be reasonable!)
- **Message Size:** Max 128KB per message

## Security

Every message is:
- ✅ Encrypted with TLS 1.2+
- ✅ Authenticated with X.509 certificates
- ✅ Authorized based on device ID
- ✅ Logged for security audits

**Your device can ONLY:**
- Read messages sent to `devices/YOUR-DEVICE-ID/*`
- Send messages to `devices/YOUR-DEVICE-ID/*`

It CANNOT access other devices' data.

## Testing

Want to test without the dashboard?

### Using Serial Monitor

1. Open: Tools → Serial Monitor
2. Set baud rate to **115200**
3. Watch messages come in
4. You'll see what your device is sending/receiving

### Using AWS IoT Test Client

1. Log into AWS IoT Console
2. Go to: Test → MQTT test client
3. Subscribe to: `devices/YOUR-DEVICE-ID/#`
4. Publish commands to: `devices/YOUR-DEVICE-ID/commands`

This is great for debugging!

## Common Questions

**Q: How often does my device send updates?**  
A: Every 10 seconds. You can change this in the firmware if needed.

**Q: Can I send custom data?**  
A: Yes! Modify the `publishStatus()` function in the firmware to add your own sensor readings.

**Q: Is there a delay between command and action?**  
A: Usually under 1 second if device is online. Delay is mostly from internet latency.

**Q: Can multiple people control the same device?**  
A: Yes! Share access in: Dashboard → Devices → Share Device

**Q: What happens if WiFi goes down?**  
A: Device keeps trying to reconnect automatically. Commands will work once WiFi comes back.

## Need Help?

- 📧 Email: support@beaverpaw.com
- 💬 Forum: [Community Discussions](https://github.com/beaverpaw/beaverpaw-firmware/discussions)

---

Want to build something cool with the API? We'd love to see it! Share in the forum. 🚀
