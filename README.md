# BeaverPaw Firmware

[![License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](LICENSE)
[![Version](https://img.shields.io/badge/version-2.0.0-green.svg)](https://github.com/beaverpaw/beaverpaw-firmware/releases)
[![Platform](https://img.shields.io/badge/platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)

Open-source firmware for BeaverPaw IoT devices. Designed for transparency, security, and ease of use.

## 🎉 Version 2.0.0 - Now iPhone Compatible!

We've added **WiFi Access Point mode** so you can set up your device from ANY phone - including iPhone!

### Two Ways to Set Up Your Device:

**🍎 WiFi Access Point (Perfect for iPhone)**
- Device creates its own WiFi network called "BeaverPaw-XXXX"
- Connect your phone to it (like connecting to hotel WiFi)
- A setup page opens automatically
- Enter your home WiFi password
- Device connects and configures itself automatically
- Done in under 2 minutes!

**🤖 Bluetooth (Android & Desktop)**
- Open the BeaverPaw setup page on your phone
- Tap "Scan for Devices"
- Select your device
- Enter WiFi password
- Done!

Both methods are equally secure and work perfectly. Choose whichever is easier for you!

## 📱 Supported Devices

### ESP32 Generic Controller
Perfect for home automation projects. Control lights, motors, sensors, and more.

**What can it do?**
- Turn lights on/off remotely
- Control RGB LED strips with any color
- Adjust motor speeds (fans, pumps, etc.)
- Move stepper motors precisely (like in 3D printers)

[View full documentation →](devices/esp32-generic/README.md)

### Security Cube Camera
A smart security camera that watches your home and alerts you to motion.

**What can it do?**
- Live video streaming to your phone
- Motion detection with instant alerts
- Works with voice assistants (Alexa, Google Home)
- Battery-powered option available

[View full documentation →](devices/security-cube/README.md)

## 🚀 Getting Started (5 Minutes)

### What You'll Need
- Your BeaverPaw device
- A computer with a USB port
- WiFi network (2.4GHz)
- 5 minutes of your time

### Quick Setup Steps

1. **Download & Install Arduino IDE** (free software)
   - Go to [arduino.cc](https://www.arduino.cc/en/software)
   - Download for Windows, Mac, or Linux
   - Install and open

2. **Get Our Code**
   - Download this repository as ZIP
   - Or if you know Git: `git clone https://github.com/beaverpaw/beaverpaw-firmware.git`

3. **Flash Your Device**
   - Follow our [step-by-step guide](docs/SETUP.md)
   - Takes about 5 minutes
   - We've made it as simple as possible!

4. **Set Up WiFi**
   - Power on your device
   - Use the method that works for your phone (WiFi AP or Bluetooth)
   - Enter your WiFi password
   - Device connects automatically!

5. **Start Using It!**
   - Log into your BeaverPaw account
   - See your device online
   - Control it from anywhere!

📖 **Need help?** See our [detailed setup guide](docs/SETUP.md)

## 🔒 Your Privacy & Security

We take security seriously. Here's how we protect you:

- **Unique encryption for every device** - Each device has its own security certificate
- **Bank-level encryption** - Same technology that protects your credit card online
- **No backdoors** - All code is open for you to review
- **Your data stays yours** - Stored in YOUR AWS account, not ours

Want to know more? Read our [security documentation](docs/SECURITY.md)

## 💬 Need Help?

We're here for you:

- **📧 Email Support:** support@beaverpaw.com (usually respond within 24 hours)
- **💬 Community Forum:** [Ask questions and share ideas](https://github.com/beaverpaw/beaverpaw-firmware/discussions)
- **🐛 Found a Bug?** [Report it here](https://github.com/beaverpaw/beaverpaw-firmware/issues)
- **📚 Documentation:** Check our [docs folder](docs/) for detailed guides

## 🤝 Contributing

We welcome contributions from the community! Whether you're fixing bugs, adding features, or improving documentation, we'd love your help.

See [CONTRIBUTING.md](CONTRIBUTING.md) for how to get started.

## 📄 License

This project is licensed under the Apache 2.0 License - see [LICENSE](LICENSE) for details.

This means you can:
- ✅ Use it for free, even commercially
- ✅ Modify it however you want
- ✅ Share it with others
- ✅ Use it in your products

## 🐕 About BeaverPaw

BeaverPaw makes thoughtfully designed electronics for people who value quality and transparency. Every product we make is:

- 🇺🇸 Designed and supported in the USA
- 🔓 Open source - no secrets
- 🛡️ Secure by design
- 💚 Built to last

**Website:** https://beaverpaw.com  
**Support:** support@beaverpaw.com

---

Made with care in the USA 🇺🇸 • Inspired by Beaver the dog 🐕
