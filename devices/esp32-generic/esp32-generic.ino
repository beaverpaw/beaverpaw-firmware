// BeaverPaw ESP32 Generic Controller - v2.0.0
// 
// This firmware gives your ESP32 board full remote control capabilities.
// Control LEDs, RGB strips, motors, and more from anywhere in the world!
//
// NEW in v2.0.0: Works with iPhone! Two setup methods:
//   1. WiFi Access Point (works on ALL phones including iPhone)
//   2. Bluetooth (works on Android and desktop browsers)
//
// Hardware: ESP32 38-pin development board
// License: Apache 2.0
// Support: support@beaverpaw.com

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <NimBLEDevice.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <HTTPClient.h>
#include <DNSServer.h>

// ==========================================
// PIN CONFIGURATION (customize these!)
// ==========================================
#define BUILTIN_LED 2          // Most ESP32 boards have LED on pin 2
#define RGB_RED_PIN 12         // Connect red pin of RGB LED here
#define RGB_GREEN_PIN 13       // Connect green pin here  
#define RGB_BLUE_PIN 14        // Connect blue pin here
#define PWM_PIN1 25            // PWM output 1 (motor speed, LED dimming, etc.)
#define PWM_PIN2 26            // PWM output 2
#define STEPPER_STEP 18        // Stepper motor step pin
#define STEPPER_DIR 19         // Stepper motor direction pin
#define STEPPER_ENABLE 21      // Stepper motor enable pin

// PWM settings
#define PWM_FREQ 5000          // 5kHz PWM frequency
#define PWM_RESOLUTION 8       // 8-bit resolution (0-255)

// ==========================================
// BLUETOOTH CONFIGURATION
// ==========================================
// These UUIDs allow phones to find and connect to your device
#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WIFI_SSID_UUID         "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define WIFI_PASSWORD_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define WIFI_STATUS_UUID       "beb5483e-36e1-4688-b7f5-ea07361b26aa"
#define DEVICE_INFO_UUID       "beb5483e-36e1-4688-b7f5-ea07361b26ab"
#define AWS_ENDPOINT_UUID      "beb5483e-36e1-4688-b7f5-ea07361b26ac"
#define AWS_CERT_UUID          "beb5483e-36e1-4688-b7f5-ea07361b26ad"
#define AWS_KEY_UUID           "beb5483e-36e1-4688-b7f5-ea07361b26ae"

// ==========================================
// WiFi ACCESS POINT CONFIGURATION
// ==========================================
#define AP_SSID_PREFIX "BeaverPaw-"  // Device creates WiFi named "BeaverPaw-XXXX"
#define AP_TIMEOUT_MS 300000         // 5 minutes timeout for AP mode
#define AP_IP IPAddress(192, 168, 4, 1)

// ==========================================
// CLOUD API CONFIGURATION
// ==========================================
// BeaverPaw cloud API for automatic credential provisioning
#define API_HOST "beaverpaw.com"
#define API_PATH "/functions/get_device_credentials_poll"

// ==========================================
// GLOBAL VARIABLES (don't change these)
// ==========================================
Preferences prefs;
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
AsyncWebServer server(80);
DNSServer dnsServer;

String deviceId;
String wifiSSID = "";
String wifiPassword = "";
String awsEndpoint = "";
String awsCert = "";
String awsKey = "";
String awsRootCA = "";

bool credentialsConfigured = false;
bool wifiConnected = false;
bool apMode = false;
unsigned long apStartTime = 0;

// Bluetooth
NimBLEServer* bleServer = nullptr;
NimBLECharacteristic* statusChar = nullptr;

// Device state
int rgbR = 0, rgbG = 0, rgbB = 0;

// Forward declarations
void startBluetooth();
void startAccessPoint();
void connectWiFi();
void connectAWS();
void fetchCredentialsFromCloud();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishStatus();
void setupPWM();
void handleRoot(AsyncWebServerRequest *request);
void handleSave(AsyncWebServerRequest *request);

// ==========================================
// SETUP - Runs once when device powers on
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Configure pins
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);
  pinMode(STEPPER_STEP, OUTPUT);
  pinMode(STEPPER_DIR, OUTPUT);
  pinMode(STEPPER_ENABLE, OUTPUT);
  digitalWrite(STEPPER_ENABLE, HIGH); // Stepper disabled by default
  
  // Setup PWM for smooth LED dimming and motor control
  setupPWM();
  
  // Generate unique device ID from chip's MAC address
  uint64_t chipid = ESP.getEfuseMac();
  deviceId = "ESP32-" + String((uint32_t)(chipid >> 32), HEX) + String((uint32_t)chipid, HEX);
  deviceId.toUpperCase();
  
  Serial.println("\n=================================");
  Serial.println("BeaverPaw ESP32 Generic v2.0.0");
  Serial.println("Device ID: " + deviceId);
  Serial.println("=================================\n");
  
  // Load saved configuration from flash memory
  prefs.begin("beaverpaw", false);
  wifiSSID = prefs.getString("wifi_ssid", "");
  wifiPassword = prefs.getString("wifi_pass", "");
  awsEndpoint = prefs.getString("aws_endpoint", "");
  awsCert = prefs.getString("aws_cert", "");
  awsKey = prefs.getString("aws_key", "");
  awsRootCA = prefs.getString("aws_root_ca", "");
  
  if (wifiSSID.length() > 0) {
    // WiFi credentials found - try to connect
    Serial.println("✓ WiFi credentials found in memory");
    Serial.println("Connecting to: " + wifiSSID);
    connectWiFi();
    
    // If connected but don't have AWS credentials yet, fetch them
    if (wifiConnected && awsEndpoint.length() == 0) {
      Serial.println("⚡ Fetching security credentials from cloud...");
      fetchCredentialsFromCloud();
    }
    
    // If we have AWS credentials, connect to cloud
    if (awsEndpoint.length() > 0 && awsCert.length() > 0) {
      credentialsConfigured = true;
      connectAWS();
    }
  } else {
    // No WiFi configured - start setup modes
    Serial.println("⚠ No WiFi configured");
    Serial.println("Starting WiFi Access Point (for iPhone)...");
    Serial.println("Starting Bluetooth (for Android)...\n");
    startAccessPoint();
    delay(2000);
    startBluetooth();
  }
}

// ==========================================
// MAIN LOOP - Runs continuously
// ==========================================
void loop() {
  if (apMode) {
    // In Access Point mode - handle DNS requests for captive portal
    dnsServer.processNextRequest();
    // Blink LED fast to show we're in setup mode
    digitalWrite(BUILTIN_LED, millis() % 200 < 100);
    
    // Exit AP mode after 5 minutes to save power
    if (millis() - apStartTime > AP_TIMEOUT_MS) {
      Serial.println("Setup timeout - restarting...");
      ESP.restart();
    }
  } 
  else if (wifiConnected) {
    // WiFi connected - maintain cloud connection
    if (!mqttClient.connected()) {
      connectAWS();
    }
    
    if (mqttClient.connected()) {
      mqttClient.loop();          // Process incoming messages
      publishStatus();            // Send our status to cloud
    }
  } 
  else {
    // Not connected - blink LED slowly
    digitalWrite(BUILTIN_LED, millis() % 1000 < 500);
  }
  
  delay(100);
}

// ==========================================
// PWM SETUP - For smooth LED and motor control
// ==========================================
void setupPWM() {
  ledcAttach(PWM_PIN1, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(PWM_PIN2, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(RGB_RED_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(RGB_GREEN_PIN, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(RGB_BLUE_PIN, PWM_FREQ, PWM_RESOLUTION);
  Serial.println("✓ PWM configured for smooth control");
}

// ==========================================
// WiFi ACCESS POINT MODE (iPhone method)
// ==========================================
void startAccessPoint() {
  apMode = true;
  apStartTime = millis();
  
  // Create WiFi network name using last 4 chars of device ID
  String apSSID = AP_SSID_PREFIX + deviceId.substring(deviceId.length() - 4);
  
  Serial.println("Creating WiFi network: " + apSSID);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apSSID.c_str());
  
  Serial.println("✓ WiFi network created!");
  Serial.println("  Network name: " + apSSID);
  Serial.println("  IP address: " + WiFi.softAPIP().toString());
  Serial.println("\nTo connect from iPhone:");
  Serial.println("  1. Open WiFi settings");
  Serial.println("  2. Connect to: " + apSSID);
  Serial.println("  3. Setup page will open automatically\n");
  
  // DNS server redirects all requests to our setup page (captive portal)
  dnsServer.start(53, "*", AP_IP);
  
  // Setup web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", "{\"status\":\"waiting\"}");
  });
  
  // Redirect any unknown URLs to setup page
  server.onNotFound([](AsyncWebServerRequest *request){
    request->redirect("/");
  });
  
  server.begin();
  Serial.println("✓ Setup page ready at: http://192.168.4.1");
}

// Setup page HTML - opens when user connects to device WiFi
void handleRoot(AsyncWebServerRequest *request) {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta charset="UTF-8">
  <title>BeaverPaw Setup</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body { 
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Arial, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 20px;
    }
    .container { 
      max-width: 420px;
      width: 100%;
      background: white;
      padding: 32px;
      border-radius: 16px;
      box-shadow: 0 20px 60px rgba(0,0,0,0.3);
    }
    h1 { 
      color: #667eea;
      margin-bottom: 8px;
      font-size: 28px;
      font-weight: 700;
    }
    .subtitle { 
      color: #666;
      font-size: 14px;
      margin-bottom: 24px;
    }
    .info { 
      background: linear-gradient(135deg, #e7f3ff 0%, #f0e7ff 100%);
      padding: 16px;
      border-radius: 10px;
      margin-bottom: 24px;
      font-size: 13px;
      line-height: 1.6;
      border-left: 4px solid #667eea;
    }
    .info strong { color: #667eea; }
    label { 
      display: block;
      margin: 20px 0 8px;
      font-weight: 600;
      font-size: 14px;
      color: #333;
    }
    input { 
      width: 100%;
      padding: 14px;
      border: 2px solid #e0e0e0;
      border-radius: 10px;
      font-size: 15px;
      transition: all 0.3s;
      font-family: inherit;
    }
    input:focus { 
      outline: none;
      border-color: #667eea;
      box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
    }
    button { 
      width: 100%;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      color: white;
      padding: 16px;
      border: none;
      border-radius: 10px;
      cursor: pointer;
      font-size: 16px;
      font-weight: 600;
      margin-top: 24px;
      transition: transform 0.2s, box-shadow 0.2s;
      font-family: inherit;
    }
    button:hover { 
      transform: translateY(-2px);
      box-shadow: 0 10px 20px rgba(102, 126, 234, 0.3);
    }
    button:active { transform: translateY(0); }
    button:disabled { 
      background: #ccc;
      cursor: not-allowed;
      transform: none;
    }
    .footer { 
      text-align: center;
      margin-top: 24px;
      font-size: 12px;
      color: #999;
    }
    .success {
      background: linear-gradient(135deg, #10b981 0%, #059669 100%);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🐕 BeaverPaw Setup</h1>
    <div class="subtitle">Let's connect your device</div>
    
    <div class="info">
      <strong>Device ID:</strong> )rawliteral" + deviceId + R"rawliteral(<br>
      <strong>Version:</strong> 2.0.0<br><br>
      Enter your home WiFi password below. Your device will connect and set itself up automatically - no technical knowledge needed!
    </div>
    
    <form id="setupForm">
      <label for="ssid">Your WiFi Network Name *</label>
      <input 
        type="text" 
        id="ssid" 
        name="ssid" 
        required 
        placeholder="MyHomeWiFi" 
        autocapitalize="none" 
        autocomplete="off">
      
      <label for="password">WiFi Password</label>
      <input 
        type="password" 
        id="password" 
        name="password" 
        placeholder="Leave blank if no password">
      
      <button type="submit" id="submitBtn">Connect My Device</button>
    </form>
    
    <div class="footer">
      BeaverPaw Labs • Secure by Design
    </div>
  </div>
  
  <script>
    const form = document.getElementById('setupForm');
    const btn = document.getElementById('submitBtn');
    
    form.onsubmit = async (e) => {
      e.preventDefault();
      
      const ssid = document.getElementById('ssid').value.trim();
      const password = document.getElementById('password').value;
      
      if (!ssid) {
        alert('Please enter your WiFi network name');
        return;
      }
      
      btn.textContent = 'Connecting...';
      btn.disabled = true;
      
      const formData = new FormData();
      formData.append('ssid', ssid);
      formData.append('password', password);
      
      try {
        const response = await fetch('/save', { 
          method: 'POST', 
          body: formData 
        });
        
        if (response.ok) {
          btn.className = 'success';
          btn.textContent = '✓ Connected! Setting up...';
          
          setTimeout(() => {
            document.body.innerHTML = `
              <div class="container" style="text-align:center;">
                <h1 style="color:#10b981;margin:0;">✓ All Set!</h1>
                <p style="margin:20px 0;color:#666;font-size:16px;">
                  Your device is connecting to WiFi and will appear in your dashboard in about a minute.
                </p>
                <p style="font-size:14px;color:#999;">
                  You can close this window and return to the BeaverPaw app.
                </p>
              </div>
            `;
          }, 2000);
        } else {
          throw new Error('Save failed');
        }
      } catch (error) {
        btn.textContent = 'Connect My Device';
        btn.disabled = false;
        alert('Connection failed. Please check your WiFi name and try again.');
      }
    };
  </script>
</body>
</html>
)rawliteral";
  
  request->send(200, "text/html", html);
}

// Save WiFi credentials and restart
void handleSave(AsyncWebServerRequest *request) {
  if (request->hasParam("ssid", true)) {
    wifiSSID = request->getParam("ssid", true)->value();
    wifiPassword = request->hasParam("password", true) ? 
                   request->getParam("password", true)->value() : "";
    
    // Save to flash memory so it persists after power off
    prefs.putString("wifi_ssid", wifiSSID);
    prefs.putString("wifi_pass", wifiPassword);
    
    Serial.println("\n✓ WiFi credentials saved!");
    Serial.println("  Network: " + wifiSSID);
    Serial.println("  Restarting to connect...\n");
    
    request->send(200, "application/json", "{\"status\":\"saved\"}");
    
    delay(1000);
    ESP.restart();
  } else {
    request->send(400, "application/json", "{\"error\":\"missing_ssid\"}");
  }
}

// ==========================================
// BLUETOOTH SETUP (Android method)
// ==========================================

// Bluetooth callback classes
class WiFiSSIDCallbacks: public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    wifiSSID = String(value.c_str());
    Serial.println("BLE: Received WiFi name: " + wifiSSID);
  }
};

class WiFiPasswordCallbacks: public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    wifiPassword = String(value.c_str());
    Serial.println("BLE: Received WiFi password");
    connectWiFi();
  }
};

class AWSEndpointCallbacks: public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    awsEndpoint = String(value.c_str());
    prefs.putString("aws_endpoint", awsEndpoint);
    Serial.println("BLE: AWS endpoint saved");
  }
};

class AWSCertCallbacks: public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    awsCert += String(value.c_str());
    if (awsCert.length() > 1000) {
      prefs.putString("aws_cert", awsCert);
      Serial.println("BLE: Certificate saved (" + String(awsCert.length()) + " bytes)");
    }
  }
};

class AWSKeyCallbacks: public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic* pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    awsKey += String(value.c_str());
    if (awsKey.length() > 1000) {
      prefs.putString("aws_key", awsKey);
      credentialsConfigured = true;
      Serial.println("✓ Private key saved - fully configured!");
      connectAWS();
    }
  }
};

void startBluetooth() {
  NimBLEDevice::init("BeaverPaw-" + deviceId);
  bleServer = NimBLEDevice::createServer();
  NimBLEService* service = bleServer->createService(SERVICE_UUID);
  
  // WiFi configuration characteristics
  auto ssidChar = service->createCharacteristic(
    WIFI_SSID_UUID, 
    NIMBLE_PROPERTY::WRITE
  );
  ssidChar->setCallbacks(new WiFiSSIDCallbacks());
  
  auto passChar = service->createCharacteristic(
    WIFI_PASSWORD_UUID, 
    NIMBLE_PROPERTY::WRITE
  );
  passChar->setCallbacks(new WiFiPasswordCallbacks());
  
  // Status characteristic - tells phone if WiFi connected
  statusChar = service->createCharacteristic(
    WIFI_STATUS_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
  );
  statusChar->setValue("waiting");
  
  // Device info - tells phone our ID and version
  auto infoChar = service->createCharacteristic(
    DEVICE_INFO_UUID,
    NIMBLE_PROPERTY::READ
  );
  String info = "{\"device_id\":\"" + deviceId + "\",\"version\":\"2.0.0\"}";
  infoChar->setValue(info.c_str());
  
  // AWS credentials characteristics (for Bluetooth method)
  auto endpointChar = service->createCharacteristic(
    AWS_ENDPOINT_UUID,
    NIMBLE_PROPERTY::WRITE
  );
  endpointChar->setCallbacks(new AWSEndpointCallbacks());
  
  auto certChar = service->createCharacteristic(
    AWS_CERT_UUID,
    NIMBLE_PROPERTY::WRITE
  );
  certChar->setCallbacks(new AWSCertCallbacks());
  
  auto keyChar = service->createCharacteristic(
    AWS_KEY_UUID,
    NIMBLE_PROPERTY::WRITE
  );
  keyChar->setCallbacks(new AWSKeyCallbacks());
  
  service->start();
  
  // Start advertising so phones can find us
  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->start();
  
  Serial.println("✓ Bluetooth advertising started");
  Serial.println("  Device name: BeaverPaw-" + deviceId);
}

// ==========================================
// WiFi CONNECTION
// ==========================================
void connectWiFi() {
  Serial.println("\nConnecting to WiFi: " + wifiSSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\n✓ WiFi connected successfully!");
    Serial.println("  IP address: " + WiFi.localIP().toString());
    Serial.println("  Signal strength: " + String(WiFi.RSSI()) + " dBm");
    
    // Save credentials for next time
    prefs.putString("wifi_ssid", wifiSSID);
    prefs.putString("wifi_pass", wifiPassword);
    
    // Notify phone via Bluetooth if connected
    if (statusChar) {
      statusChar->setValue("connected");
      statusChar->notify();
    }
  } else {
    Serial.println("\n✗ WiFi connection failed");
    Serial.println("  Please check:");
    Serial.println("  - WiFi name is correct");
    Serial.println("  - Password is correct");
    Serial.println("  - Network is 2.4GHz (not 5GHz)");
    
    if (statusChar) {
      statusChar->setValue("failed");
      statusChar->notify();
    }
  }
}

// ==========================================
// FETCH CREDENTIALS FROM CLOUD
// ==========================================
void fetchCredentialsFromCloud() {
  if (!wifiConnected) {
    Serial.println("Cannot fetch credentials - WiFi not connected");
    return;
  }
  
  Serial.println("\nContacting BeaverPaw cloud...");
  Serial.println("Requesting security credentials for: " + deviceId);
  
  HTTPClient http;
  http.begin("https://" + String(API_HOST) + String(API_PATH));
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(15000);
  
  // Create request
  StaticJsonDocument<256> requestDoc;
  requestDoc["device_id"] = deviceId;
  String requestBody;
  serializeJson(requestDoc, requestBody);
  
  // Send request
  int httpCode = http.POST(requestBody);
  
  if (httpCode == 200) {
    String response = http.getString();
    
    StaticJsonDocument<6144> doc;
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error && doc["success"]) {
      // Extract credentials
      awsEndpoint = doc["credentials"]["endpoint"].as<String>();
      awsCert = doc["credentials"]["certificate"].as<String>();
      awsKey = doc["credentials"]["privateKey"].as<String>();
      awsRootCA = doc["credentials"]["rootCA"].as<String>();
      
      // Save to flash memory
      prefs.putString("aws_endpoint", awsEndpoint);
      prefs.putString("aws_cert", awsCert);
      prefs.putString("aws_key", awsKey);
      prefs.putString("aws_root_ca", awsRootCA);
      
      credentialsConfigured = true;
      
      Serial.println("✓ Security credentials received and saved!");
      Serial.println("  Device is now ready to connect to cloud");
      
      connectAWS();
    } else {
      const char* errorMsg = doc["error"] | "unknown error";
      Serial.println("⚠ Could not get credentials: " + String(errorMsg));
      Serial.println("\nThis usually means:");
      Serial.println("  1. Device hasn't been claimed yet");
      Serial.println("  2. Go to your BeaverPaw dashboard");
      Serial.println("  3. Click 'Claim Device' and scan your QR code");
      Serial.println("  4. Then restart this device");
    }
  } else if (httpCode == 404) {
    Serial.println("⚠ Device not found in cloud (404)");
    Serial.println("  Please claim this device in your dashboard first!");
  } else {
    Serial.println("✗ Cloud request failed: HTTP " + String(httpCode));
  }
  
  http.end();
}

// ==========================================
// AWS IoT CONNECTION
// ==========================================
void connectAWS() {
  if (!credentialsConfigured || awsEndpoint.length() == 0) {
    Serial.println("\n⚠ Cannot connect to cloud - credentials not configured");
    return;
  }
  
  Serial.println("\nConnecting to AWS IoT Cloud...");
  
  // Set up secure connection with certificates
  if (awsRootCA.length() > 0) {
    wifiClient.setCACert(awsRootCA.c_str());
  }
  wifiClient.setCertificate(awsCert.c_str());
  wifiClient.setPrivateKey(awsKey.c_str());
  
  mqttClient.setServer(awsEndpoint.c_str(), 8883);
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(60);
  mqttClient.setSocketTimeout(30);
  
  if (mqttClient.connect(deviceId.c_str())) {
    Serial.println("✓ Connected to cloud!");
    Serial.println("  Your device is now online and controllable");
    
    // Subscribe to command topic
    String commandTopic = "devices/" + deviceId + "/commands";
    mqttClient.subscribe(commandTopic.c_str());
    Serial.println("  Listening for commands on: " + commandTopic);
    
    // Turn on LED solid to show we're online
    digitalWrite(BUILTIN_LED, HIGH);
  } else {
    Serial.println("✗ Cloud connection failed");
    Serial.println("  Error code: " + String(mqttClient.state()));
    Serial.println("  Will retry automatically...");
  }
}

// ==========================================
// HANDLE INCOMING COMMANDS
// ==========================================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println("\n📩 Command received!");
  
  // Parse command JSON
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  
  if (error) {
    Serial.println("✗ Invalid command format");
    return;
  }
  
  String command = doc["command"];
  Serial.println("  Command: " + command);
  
  // DIGITAL PIN CONTROL
  if (command == "set_pin") {
    int pin = doc["pin"];
    int value = doc["value"];
    digitalWrite(pin, value);
    Serial.println("  ✓ Set pin " + String(pin) + " to " + String(value ? "ON" : "OFF"));
  }
  else if (command == "toggle_pin") {
    int pin = doc["pin"];
    digitalWrite(pin, !digitalRead(pin));
    Serial.println("  ✓ Toggled pin " + String(pin));
  }
  else if (command == "blink_pin") {
    int pin = doc["pin"];
    int count = doc["count"] | 3;
    int delayMs = doc["delay"] | 200;
    Serial.println("  ✓ Blinking pin " + String(pin) + " " + String(count) + " times");
    for (int i = 0; i < count; i++) {
      digitalWrite(pin, HIGH);
      delay(delayMs);
      digitalWrite(pin, LOW);
      delay(delayMs);
    }
  }
  
  // RGB LED CONTROL
  else if (command == "set_rgb") {
    rgbR = doc["r"];
    rgbG = doc["g"];
    rgbB = doc["b"];
    ledcWrite(RGB_RED_PIN, rgbR);
    ledcWrite(RGB_GREEN_PIN, rgbG);
    ledcWrite(RGB_BLUE_PIN, rgbB);
    Serial.println("  ✓ RGB set to R:" + String(rgbR) + " G:" + String(rgbG) + " B:" + String(rgbB));
  }
  
  // PWM CONTROL (motor speed, LED dimming)
  else if (command == "set_pwm") {
    int pin = doc["pin"];
    int duty = doc["duty"]; // 0-100%
    int dutyCycle = map(duty, 0, 100, 0, 255);
    ledcWrite(pin, dutyCycle);
    Serial.println("  ✓ PWM on pin " + String(pin) + " set to " + String(duty) + "%");
  }
  
  // STEPPER MOTOR CONTROL
  else if (command == "move_stepper") {
    int steps = doc["steps"] | 200;
    int direction = doc["direction"] | 1;
    int speed = doc["speed"] | 60;
    
    digitalWrite(STEPPER_DIR, direction);
    digitalWrite(STEPPER_ENABLE, LOW); // Enable motor
    
    int delayMicros = (60 * 1000000) / (200 * speed);
    
    for (int i = 0; i < steps; i++) {
      digitalWrite(STEPPER_STEP, HIGH);
      delayMicroseconds(delayMicros / 2);
      digitalWrite(STEPPER_STEP, LOW);
      delayMicroseconds(delayMicros / 2);
    }
    
    digitalWrite(STEPPER_ENABLE, HIGH); // Disable motor
    Serial.println("  ✓ Stepper moved " + String(steps) + " steps");
  }
  
  else {
    Serial.println("  ⚠ Unknown command: " + command);
  }
}

// ==========================================
// PUBLISH STATUS TO CLOUD
// ==========================================
void publishStatus() {
  static unsigned long lastPublish = 0;
  
  // Only publish every 10 seconds
  if (millis() - lastPublish < 10000) return;
  
  StaticJsonDocument<512> doc;
  doc["device_id"] = deviceId;
  doc["version"] = "2.0.0";
  doc["temperature"] = random(20, 30);  // Replace with real sensor if you have one
  doc["battery"] = random(80, 100);      // Replace with real battery monitor
  doc["rssi"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  doc["free_heap"] = ESP.getFreeHeap();
  
  // Pin states
  doc["pin_2"] = digitalRead(BUILTIN_LED);
  doc["rgb_r"] = rgbR;
  doc["rgb_g"] = rgbG;
  doc["rgb_b"] = rgbB;
  
  char buffer[512];
  serializeJson(doc, buffer);
  
  String topic = "devices/" + deviceId + "/data";
  mqttClient.publish(topic.c_str(), buffer);
  
  lastPublish = millis();
}
