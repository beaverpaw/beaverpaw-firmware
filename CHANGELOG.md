# Changelog

All notable changes to BeaverPaw firmware are documented here.

## [2.0.0] - 2025-01-10

### 🎉 Major Update - iPhone Support!

#### Added
- **WiFi Access Point Mode** - Device creates its own temporary WiFi network for easy setup
- **Captive Portal Interface** - Beautiful mobile-friendly setup page that opens automatically
- **Cloud Auto-Provisioning** - Device fetches security credentials from cloud automatically
- **Dual Setup Methods** - Choose between WiFi AP (iPhone/all) or Bluetooth (Android)
- **DNS Server** for captive portal redirect
- **HTTP Setup Server** with responsive mobile design
- **Comprehensive Troubleshooting Guide** for both setup methods

#### Changed
- Setup flow simplified - no need to manually enter AWS credentials
- LED blink patterns updated to show AP mode (fast blink) vs connected (solid)
- Firmware size increased from ~800 lines to ~1500 lines (WiFi AP support)
- Memory usage optimized for dual-mode operation

#### Security
- No security changes - still uses X.509 certificate authentication
- Credentials transmitted securely over HTTPS when polling cloud
- Private keys never leave device
- WiFi credentials stored encrypted in flash memory

#### Dependencies Added
- ESPAsyncWebServer @ 1.2.0
- AsyncTCP @ 1.1.0
- DNSServer (built-in)
- HTTPClient (built-in)

## [1.0.0] - 2025-01-05

### Initial Production Release

#### Features
- Bluetooth WiFi provisioning for Android/desktop
- AWS IoT Core integration with X.509 certificates
- Motion detection (Security Cube)
- Remote LED control (ESP32 Generic)
- Battery and temperature monitoring
- MQTT real-time communication
- Secure credential storage

#### Supported Hardware
- ESP32 38-pin development boards
- ESP32-CAM (AI-Thinker) modules

---

For detailed release notes, see [GitHub Releases](https://github.com/beaverpaw/beaverpaw-firmware/releases).
