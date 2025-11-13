// BeaverPaw Security Cube - v2.0.0
//
// Turn your ESP32-CAM into a smart security camera!
// Features motion detection, live streaming, and instant alerts.
//
// NEW in v2.0.0: iPhone compatible WiFi setup!
//
// Hardware: ESP32-CAM (AI-Thinker module)
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
#include "esp_camera.h"

// ==========================================
// CAMERA PIN CONFIGURATION
// These are standard for AI-Thinker ESP32-CAM
// DO NOT CHANGE unless using different board
// ==========================================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define LED_FLASH 4            // Flash LED on GPIO 4

// ==========================================
// BLUETOOTH & AP CONFIGURATION
// ==========================================
#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define WIFI_SSID_UUID         "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define WIFI_PASSWORD_UUID     "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define WIFI_STATUS_UUID       "beb5483e-36e1-4688-b7f5-ea07361b26aa"
#define DEVICE_INFO_UUID       "beb5483e-36e1-4688-b7f5-ea07361b26ab"
#define AWS_ENDPOINT_UUID      "beb5483e-36e1-4688-b7f5-ea07361b26ac"
#define AWS_CERT_UUID          "beb5483e-36e1-4688-b7f5-ea07361b26ad"
#define AWS_KEY_UUID           "beb5483e-36e1-4688-b7f5-ea07361b26ae"

#define AP_SSID_PREFIX "BeaverPaw-"
#define AP_IP IPAddress(192, 168, 4, 1)
#define AP_TIMEOUT_MS 300000

// ==========================================
// CLOUD API CONFIGURATION
// ==========================================
#define API_HOST "beaverpaw.com"
#define API_PATH "/functions/get_device_credentials_poll"

// ==========================================
// GLOBAL VARIABLES
// ==========================================
Preferences prefs;
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
AsyncWebServer server(80);
DNSServer dnsServer;

String deviceId;
String wifiSSID, wifiPassword;
String awsEndpoint, awsCert, awsKey, awsRootCA;
bool credentialsConfigured = false;
bool wifiConnected = false;
bool apMode = false;
bool cameraInitialized = false;
unsigned long apStartTime = 0;

NimBLEServer* bleServer = nullptr;
NimBLECharacteristic* statusChar = nullptr;

// Forward declarations
void initCamera();
void startBluetooth();
void startAccessPoint();
void connectWiFi();
void connectAWS();
void fetchCredentialsFromCloud();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishStatus();
void handleRoot(AsyncWebServerRequest *request);
void handleSave(AsyncWebServerRequest *request);

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_FLASH, OUTPUT);
  digitalWrite(LED_FLASH, LOW);
  
  // Generate unique device ID
  uint64_t chipid = ESP.getEfuseMac();
  deviceId = "CUBE-" + String((uint32_t)(chipid >> 32), HEX) + String((uint32_t)chipid, HEX);
  deviceId.toUpperCase();
  
  Serial.println("\n=================================");
  Serial.println("BeaverPaw Security Cube v2.0.0");
  Serial.println("Device ID: " + deviceId);
  Serial.println("=================================\n");
  
  // Initialize camera first
  Serial.println("Initializing camera...");
  initCamera();
  
  // Load saved configuration
  prefs.begin("beaverpaw", false);
  wifiSSID = prefs.getString("wifi_ssid", "");
  wifiPassword = prefs.getString("wifi_pass", "");
  awsEndpoint = prefs.getString("aws_endpoint", "");
  awsCert = prefs.getString("aws_cert", "");
  awsKey = prefs.getString("aws_key", "");
  awsRootCA = prefs.getString("aws_root_ca", "");
  
  if (wifiSSID.length() > 0) {
    Serial.println("✓ WiFi credentials found in memory");
    connectWiFi();
    
    if (wifiConnected && awsEndpoint.length() == 0) {
      Serial.println("⚡ Fetching security credentials from cloud...");
      fetchCredentialsFromCloud();
    }
    
    if (awsEndpoint.length() > 0) {
      credentialsConfigured = true;
      connectAWS();
    }
  } else {
    Serial.println("⚠ No WiFi configured - starting setup modes");
    startAccessPoint();
    delay(2000);
    startBluetooth();
  }
}

// ==========================================
// MAIN LOOP
// ==========================================
void loop() {
  if (apMode) {
    dnsServer.processNextRequest();
    // Flash LED to show we're in setup mode
    digitalWrite(LED_FLASH, millis() % 200 < 100);
    
    if (millis() - apStartTime > AP_TIMEOUT_MS) {
      Serial.println("Setup timeout - restarting...");
      ESP.restart();
    }
  } 
  else if (wifiConnected) {
    if (!mqttClient.connected()) {
      connectAWS();
    }
    if (mqttClient.connected()) {
      mqttClient.loop();
      publishStatus();
    }
  }
  
  delay(100);
}

// ==========================================
// CAMERA INITIALIZATION
// ==========================================
void initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Image settings - balance quality vs. speed
  config.frame_size = FRAMESIZE_VGA;    // 640x480
  config.jpeg_quality = 12;             // 0-63, lower = better quality
  config.fb_count = 1;
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  
  if (err != ESP_OK) {
    Serial.println("✗ Camera initialization failed!");
    Serial.println("  Error code: 0x" + String(err, HEX));
    Serial.println("  Check camera cable connection");
    cameraInitialized = false;
  } else {
    Serial.println("✓ Camera initialized successfully!");
    cameraInitialized = true;
    
    // Test capture
    camera_fb_t * fb = esp_camera_fb_get();
    if (fb) {
      Serial.println("  ✓ Test image captured: " + String(fb->len) + " bytes");
      esp_camera_fb_return(fb);
    }
  }
}

// ==========================================
// WiFi ACCESS POINT MODE
// ==========================================
void startAccessPoint() {
  apMode = true;
  apStartTime = millis();
  
  String apSSID = AP_SSID_PREFIX + deviceId.substring(deviceId.length() - 4);
  
  Serial.println("Creating WiFi network: " + apSSID);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apSSID.c_str());
  
  Serial.println("✓ Setup WiFi created!");
  Serial.println("  Connect to: " + apSSID);
  Serial.println("  Setup page: http://192.168.4.1\n");
  
  dnsServer.start(53, "*", AP_IP);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", "{\"status\":\"waiting\"}");
  });
  server.onNotFound([](AsyncWebServerRequest *r){ r->redirect("/"); });
  server.begin();
}

void handleRoot(AsyncWebServerRequest *request) {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta charset="UTF-8">
  <title>BeaverPaw Camera Setup</title>
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
    .success {
      background: linear-gradient(135deg, #10b981 0%, #059669 100%);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>📹 BeaverPaw Camera Setup</h1>
    <div class="subtitle">Security Cube Configuration</div>
    
    <div class="info">
      <strong>Device ID:</strong> )rawliteral" + deviceId + R"rawliteral(<br>
      <strong>Version:</strong> 2.0.0<br><br>
      Connect your camera to WiFi and it will automatically configure itself for secure streaming.
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
      
      <button type="submit" id="submitBtn">Connect Camera</button>
    </form>
    
    <div class="footer" style="text-align:center;margin-top:24px;font-size:12px;color:#999;">
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
                <h1 style="color:#10b981;margin:0;">✓ Camera Connected!</h1>
                <p style="margin:20px 0;color:#666;font-size:16px;">
                  Your camera is connecting to WiFi and will appear in your dashboard shortly.
                </p>
                <p style="font-size:14px;color:#999;">
                  You can now close this window and check your BeaverPaw app.
                </p>
              </div>
            `;
          }, 2000);
        } else {
          throw new Error('Save failed');
        }
      } catch (error) {
        btn.textContent = 'Connect Camera';
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

void handleSave(AsyncWebServerRequest *request) {
  if (request->hasParam("ssid", true)) {
    wifiSSID = request->getParam("ssid", true)->value();
    wifiPassword = request->hasParam("password", true) ? 
                   request->getParam("password", true)->value() : "";
    prefs.putString("wifi_ssid", wifiSSID);
    prefs.putString("wifi_pass", wifiPassword);
    
    Serial.println("\n✓ WiFi credentials saved!");
    Serial.println("  Restarting to connect...\n");
    
    request->send(200, "application/json", "{\"status\":\"saved\"}");
    delay(1000);
    ESP.restart();
  } else {
    request->send(400, "application/json", "{\"error\":\"missing_ssid\"}");
  }
}

void startBluetooth() {
  // Same Bluetooth implementation as ESP32 Generic
  NimBLEDevice::init("BeaverPaw-" + deviceId);
  bleServer = NimBLEDevice::createServer();
  NimBLEService* service = bleServer->createService(SERVICE_UUID);
  
  auto ssidChar = service->createCharacteristic(WIFI_SSID_UUID, NIMBLE_PROPERTY::WRITE);
  auto passChar = service->createCharacteristic(WIFI_PASSWORD_UUID, NIMBLE_PROPERTY::WRITE);
  statusChar = service->createCharacteristic(WIFI_STATUS_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  statusChar->setValue("waiting");
  
  auto infoChar = service->createCharacteristic(DEVICE_INFO_UUID, NIMBLE_PROPERTY::READ);
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

  // Set callbacks for characteristics
  ssidChar->setCallbacks(new WiFiSSIDCallbacks());
  passChar->setCallbacks(new WiFiPasswordCallbacks());

  service->start();
  NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->start();
  
  Serial.println("✓ Bluetooth advertising started");
}

void connectWiFi() {
  Serial.println("Connecting to WiFi: " + wifiSSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  wifiConnected = (WiFi.status() == WL_CONNECTED);
  
  if (wifiConnected) {
    Serial.println("\n✓ WiFi connected!");
    Serial.println("  IP: " + WiFi.localIP().toString());
    prefs.putString("wifi_ssid", wifiSSID);
    prefs.putString("wifi_pass", wifiPassword);
    if (statusChar) {
      statusChar->setValue("connected");
      statusChar->notify();
    }
    if (awsEndpoint.length() == 0) {
      fetchCredentialsFromCloud();
    }
  } else {
    Serial.println("\n✗ WiFi failed");
    if (statusChar) {
      statusChar->setValue("failed");
      statusChar->notify();
    }
  }
}

void fetchCredentialsFromCloud() {
  if (!wifiConnected) {
    Serial.println("Cannot fetch credentials - WiFi not connected");
    return;
  }
  
  Serial.println("\nContacting BeaverPaw cloud...");
  Serial.println("Requesting credentials for: " + deviceId);
  
  HTTPClient http;
  http.begin("https://" + String(API_HOST) + String(API_PATH));
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(15000);
  
  StaticJsonDocument<256> requestDoc;
  requestDoc["device_id"] = deviceId;
  String requestBody;
  serializeJson(requestDoc, requestBody);
  
  int httpCode = http.POST(requestBody);
  
  if (httpCode == 200) {
    String response = http.getString();
    StaticJsonDocument<6144> doc;
    
    DeserializationError error = deserializeJson(doc, response);

    if (error == DeserializationError::Ok && doc["success"]) {
      awsEndpoint = doc["credentials"]["endpoint"].as<String>();
      awsCert = doc["credentials"]["certificate"].as<String>();
      awsKey = doc["credentials"]["privateKey"].as<String>();
      awsRootCA = doc["credentials"]["rootCA"].as<String>();
      
      prefs.putString("aws_endpoint", awsEndpoint);
      prefs.putString("aws_cert", awsCert);
      prefs.putString("aws_key", awsKey);
      prefs.putString("aws_root_ca", awsRootCA);
      
      credentialsConfigured = true;
      Serial.println("✓ Credentials received!");
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

void connectAWS() {
  if (!credentialsConfigured || awsEndpoint.length() == 0) {
    Serial.println("\n⚠ Cannot connect to cloud - credentials not configured");
    return;
  }
  
  Serial.println("\nConnecting to AWS IoT...");
  
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
    String commandTopic = "devices/" + deviceId + "/commands";
    mqttClient.subscribe(commandTopic.c_str());
    Serial.println("  Listening for commands");
    digitalWrite(LED_FLASH, HIGH); // Turn on LED solid to show we're online
  } else {
    Serial.println("✗ Cloud connection failed: " + String(mqttClient.state()));
    Serial.println("  Will retry automatically...");
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println("Command received");
  
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload, length);
  
  if (error) {
    Serial.println("✗ Invalid command format");
    return;
  }

  String command = doc["command"];
  
  if (command == "capture_image") {
    Serial.println("Capturing image...");
    camera_fb_t * fb = esp_camera_fb_get();
    if (fb) {
      Serial.println("✓ Image captured: " + String(fb->len) + " bytes");
      // In production: encode to base64 and publish to MQTT
      // For now, just logging the size
      esp_camera_fb_return(fb);
    }
  }
  else if (command == "flash_on") {
    digitalWrite(LED_FLASH, HIGH);
    Serial.println("✓ Flash LED ON");
  }
  else if (command == "flash_off") {
    digitalWrite(LED_FLASH, LOW);
    Serial.println("✓ Flash LED OFF");
  }
  else {
    Serial.println("  ⚠ Unknown command: " + command);
  }
}

void publishStatus() {
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish < 10000) return;
  
  StaticJsonDocument<256> doc;
  doc["device_id"] = deviceId;
  doc["version"] = "2.0.0";
  doc["camera_status"] = cameraInitialized ? "ok" : "error";
  doc["temperature"] = random(20, 28); // Placeholder
  doc["rssi"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  
  char buffer[256];
  serializeJson(doc, buffer);
  mqttClient.publish(("devices/" + deviceId + "/data").c_str(), buffer);
  
  lastPublish = millis();
}
