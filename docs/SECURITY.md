# Security & Privacy

Your security and privacy are our top priorities. Here's how we protect you.

## Simple Explanation

Every BeaverPaw device has its own unique "certificate" - think of it like a passport. This certificate proves the device is really yours and encrypts all communication so nobody can snoop.

**Bottom line:** Your data is encrypted, private, and secure. We can't see it, hackers can't see it, only you can see it.

## How Security Works

### Unique ID for Every Device

Each device has a unique certificate (like a fingerprint). Even if someone copies your firmware, they can't impersonate your device.

**What this means for you:**
- ✅ Nobody can fake your device
- ✅ Each device is individually controllable
- ✅ Compromising one device doesn't affect others

### Bank-Level Encryption

All communication uses TLS 1.2+ (the same encryption that protects your bank account online).

**What this means for you:**
- ✅ Your commands are private
- ✅ Your video can't be intercepted
- ✅ Your WiFi password is protected during setup

### Your Data, Your Control

**Where your data goes:**
- Your video → YOUR Amazon AWS account (not ours)
- Your sensor data → YOUR AWS account
- Your commands → YOUR devices directly

**What we CAN'T see:**
- ❌ Your video streams
- ❌ Your sensor readings
- ❌ Your device commands
- ❌ Your WiFi password

**What we CAN see:**
- ✅ When you claimed a device (date/time)
- ✅ Your email address (from account)
- ✅ Device online/offline status

We only see what's necessary to make the service work!

## How We Protect You

### 1. Certificate-Based Authentication

**Normal websites:** Use username + password (can be stolen)  
**BeaverPaw:** Uses cryptographic certificates (can't be stolen remotely)

When your device connects:
1. Device shows its certificate to cloud
2. Cloud verifies certificate is valid
3. Cloud shows ITS certificate to device
4. Device verifies cloud is real AWS
5. Both sides agree on encryption keys
6. Communication starts (fully encrypted)

This is called "mutual TLS" - both sides prove who they are.

### 2. Private Keys Stay Private

Every device has two parts:
- **Certificate (public)** - Can be shared, proves identity
- **Private key (secret)** - NEVER leaves device, used for encryption

**Your private key:**
- ✅ Generated when you claim device
- ✅ Stored ONLY on your device
- ✅ Never sent over network
- ✅ Can't be extracted remotely

Losing physical access to device? Revoke its certificate in dashboard. Done!

### 3. Topic-Based Authorization

Your device can ONLY access its own topics:

**Device ESP32-ABC123 can:**
- ✅ Send to: `devices/ESP32-ABC123/*`
- ✅ Read from: `devices/ESP32-ABC123/*`

**Device ESP32-ABC123 CANNOT:**
- ❌ Access: `devices/OTHER-DEVICE/*`
- ❌ See other users' data
- ❌ Control other devices

This is enforced by AWS IoT (not just our code) - even WE can't bypass it!

### 4. Encrypted Storage

**On device:**
- WiFi passwords stored encrypted in flash memory
- Security keys stored in secure partition
- Can't be easily extracted

**In cloud (your AWS account):**
- Video encrypted at rest (AES-256)
- Data encrypted in DynamoDB
- Access logs for auditing

## WiFi Setup Security

You might wonder: "Is it safe to send my WiFi password?"

### WiFi Access Point Method (iPhone)

- ✅ Password sent over local WiFi connection (device to phone, no internet)
- ✅ Not transmitted to BeaverPaw servers
- ✅ Saved encrypted on device
- ✅ Only you can access the setup page (requires connecting to device WiFi first)

### Bluetooth Method (Android)

- ✅ Bluetooth Low Energy (BLE) connection
- ✅ Short range (10 meters max)
- ✅ Encrypted at BLE protocol level
- ✅ Not transmitted to BeaverPaw servers
- ✅ Saved encrypted on device

**Both methods are secure!** Your WiFi password never leaves your local network.

## What About the Cloud API?

When using WiFi Access Point method, device polls cloud for AWS credentials:

**What's sent:**
- Device ID (not sensitive - printed on your device)

**What's received:**
- AWS IoT endpoint URL (not sensitive)
- Device certificate (public, like a business card)
- Device private key (sent once, then deleted from server)

**Security measures:**
- ✅ HTTPS encryption
- ✅ Private key only sent once
- ✅ Private key deleted from server after delivery
- ✅ Cannot be retrieved again (device stores it)

## Open Source = Auditable

**You can verify everything:**
- All firmware code is here on GitHub
- You can read every line
- No hidden functions or backdoors
- Community reviews and audits code

**Don't trust us?** Read the code! That's why it's open source.

## Best Practices for Users

### Keep Your Devices Secure

✅ **Use strong WiFi password** - WPA3 or WPA2  
✅ **Update firmware** - We fix security issues fast  
✅ **Physical security** - Keep devices out of reach  
✅ **Review access** - Check who has access to your devices  
✅ **Revoke lost devices** - Lost a device? Revoke it in dashboard  

### Keep Your Account Secure

✅ **Strong password** - Unique, 12+ characters  
✅ **Don't share login** - Each person should have their own account  
✅ **Log out on shared computers**  
✅ **Review activity** - Check activity log regularly  

## Privacy Commitments

**We promise:**

1. **We don't sell your data** - Ever. Not our business model.
2. **We don't train AI on your data** - Your video is yours.
3. **We don't access your devices** - We technically can't, even if subpoenaed.
4. **We're transparent** - Open source code, clear privacy policy.

**Your data rights:**
- ✅ Export all your data anytime
- ✅ Delete your data anytime
- ✅ See what data we have (spoiler: almost nothing)

## Security Incident Response

**Found a security issue? Please tell us privately:**

📧 **security@beaverpaw.com**

**Do NOT post publicly** - we'll fix it first, then announce.

**What we'll do:**
1. Acknowledge within 48 hours
2. Investigate and develop fix
3. Release patch within 1 week
4. Notify all users
5. Credit you in release notes (if you want)

**What you'll get:**
- Our gratitude!
- Recognition (if you want it)
- Maybe a free device (for serious issues)

## Compliance & Standards

**Standards we follow:**
- **TLS 1.2+** - Modern encryption
- **X.509 v3** - Industry standard certificates
- **RSA 2048** - Strong key length
- **AES-256** - Government-grade encryption

**Privacy regulations:**
- **GDPR** compliant (European privacy law)
- **CCPA** compliant (California privacy law)
- **SOC 2** controls (security best practices)

## Common Questions

**Q: Can BeaverPaw employees see my video?**  
A: No. Video goes to YOUR AWS account. We literally cannot access it.

**Q: What if BeaverPaw shuts down?**  
A: Your devices keep working! They connect to YOUR AWS account, not ours.

**Q: Can police request my data?**  
A: They'd need to subpoena YOU and YOUR AWS account. We don't have your data to give.

**Q: What if my device is hacked?**  
A: Revoke its certificate in dashboard. Device can no longer connect. Get a new one.

**Q: Is WiFi setup secure?**  
A: Yes! Password never goes to our servers, only to your device over local connection.

**Q: What data do you collect?**  
A: Only: email, claimed devices list, account creation date. That's it.

**Q: Do you use analytics/tracking?**  
A: Minimal. We track: logins, device claims, errors (to improve service). No advertising trackers.

## Security Roadmap

**Coming soon:**
- Hardware security module support
- Signed firmware updates
- Tamper detection
- Encrypted local storage

Want to help? Contribute on GitHub!

## Learn More

- 📖 [AWS IoT Security](https://docs.aws.com/iot/latest/developerguide/security.html)
- 📖 [TLS Explained](https://tls.ulfheim.net/)
- 📖 [X.509 Certificates](https://www.ssl.com/faqs/what-is-an-x-509-certificate/)

---

Security is a journey, not a destination. We're constantly improving. Have suggestions? Email us! 🔒
