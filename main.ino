#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Pin definitions
const int METAL_DETECTOR_PIN = 2;  // Digital pin connected to metal detector output
const int LED_PIN = 25;            // Built-in LED or external LED
const int BUZZER_PIN = 26;         // Optional buzzer pin

// Web server on port 80
WebServer server(80);

// Detection variables
bool metalDetected = false;
unsigned long lastDetectionTime = 0;
int detectionCount = 0;
unsigned long systemStartTime = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(METAL_DETECTOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize variables
  systemStartTime = millis();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/api/status", handleStatus);
  server.on("/api/detection", handleDetection);
  
  // Enable CORS for all routes
  server.enableCORS(true);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
  // Read metal detector sensor
  int sensorValue = digitalRead(METAL_DETECTOR_PIN);
  
  // Check for metal detection (assuming HIGH means metal detected)
  if (sensorValue == HIGH && !metalDetected) {
    metalDetected = true;
    lastDetectionTime = millis();
    detectionCount++;
    
    // Activate LED and buzzer
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    
    Serial.println("METAL DETECTED!");
    Serial.print("Detection count: ");
    Serial.println(detectionCount);
    
    delay(100); // Short delay to avoid multiple triggers
  }
  else if (sensorValue == LOW && metalDetected) {
    metalDetected = false;
    
    // Deactivate LED and buzzer
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    Serial.println("Metal detection cleared");
  }
  
  delay(50); // Small delay for stability
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Metal Detector</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            align-items: center;
            justify-content: center;
            padding: 20px;
        }
        
        .container {
            background: rgba(255, 255, 255, 0.95);
            backdrop-filter: blur(10px);
            border-radius: 20px;
            padding: 40px;
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.1);
            max-width: 600px;
            width: 100%;
            text-align: center;
        }
        
        .header {
            margin-bottom: 30px;
        }
        
        .title {
            font-size: 2.5em;
            color: #333;
            margin-bottom: 10px;
            background: linear-gradient(45deg, #667eea, #764ba2);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
        }
        
        .subtitle {
            color: #666;
            font-size: 1.1em;
        }
        
        .status-card {
            background: #f8f9fa;
            border-radius: 15px;
            padding: 30px;
            margin: 20px 0;
            border-left: 5px solid #667eea;
        }
        
        .detection-indicator {
            width: 120px;
            height: 120px;
            border-radius: 50%;
            margin: 0 auto 20px;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 2em;
            font-weight: bold;
            transition: all 0.3s ease;
            border: 4px solid;
        }
        
        .safe {
            background: linear-gradient(45deg, #4CAF50, #45a049);
            color: white;
            border-color: #4CAF50;
            animation: pulse-green 2s infinite;
        }
        
        .detected {
            background: linear-gradient(45deg, #f44336, #d32f2f);
            color: white;
            border-color: #f44336;
            animation: pulse-red 0.5s infinite;
        }
        
        @keyframes pulse-green {
            0% { transform: scale(1); }
            50% { transform: scale(1.05); }
            100% { transform: scale(1); }
        }
        
        @keyframes pulse-red {
            0% { transform: scale(1); }
            50% { transform: scale(1.1); }
            100% { transform: scale(1); }
        }
        
        .stats {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin: 20px 0;
        }
        
        .stat-item {
            background: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 5px 15px rgba(0, 0, 0, 0.1);
        }
        
        .stat-value {
            font-size: 2em;
            font-weight: bold;
            color: #667eea;
            margin-bottom: 5px;
        }
        
        .stat-label {
            color: #666;
            font-size: 0.9em;
        }
        
        .refresh-btn {
            background: linear-gradient(45deg, #667eea, #764ba2);
            color: white;
            border: none;
            padding: 12px 30px;
            border-radius: 25px;
            font-size: 1em;
            cursor: pointer;
            transition: transform 0.2s ease;
            margin-top: 20px;
        }
        
        .refresh-btn:hover {
            transform: translateY(-2px);
        }
        
        .connection-status {
            position: absolute;
            top: 20px;
            right: 20px;
            padding: 8px 16px;
            border-radius: 20px;
            font-size: 0.8em;
            font-weight: bold;
        }
        
        .connected {
            background: #4CAF50;
            color: white;
        }
        
        .disconnected {
            background: #f44336;
            color: white;
        }
    </style>
</head>
<body>
    <div class="connection-status" id="connectionStatus">Disconnected</div>
    
    <div class="container">
        <div class="header">
            <h1 class="title">🔍 Metal Detector</h1>
            <p class="subtitle">ESP32 Based Detection System</p>
        </div>
        
        <div class="status-card">
            <div class="detection-indicator safe" id="indicator">
                SAFE
            </div>
            <h2 id="statusText">No Metal Detected</h2>
            <p id="lastDetection">Last Detection: Never</p>
        </div>
        
        <div class="stats">
            <div class="stat-item">
                <div class="stat-value" id="detectionCount">0</div>
                <div class="stat-label">Total Detections</div>
            </div>
            <div class="stat-item">
                <div class="stat-value" id="uptime">00:00:00</div>
                <div class="stat-label">System Uptime</div>
            </div>
        </div>
        
        <button class="refresh-btn" onclick="updateStatus()">🔄 Refresh Status</button>
    </div>

    <script>
        let isConnected = false;
        
        function updateConnectionStatus(connected) {
            const statusEl = document.getElementById('connectionStatus');
            isConnected = connected;
            
            if (connected) {
                statusEl.textContent = 'Connected';
                statusEl.className = 'connection-status connected';
            } else {
                statusEl.textContent = 'Disconnected';
                statusEl.className = 'connection-status disconnected';
            }
        }
        
        function formatUptime(seconds) {
            const hours = Math.floor(seconds / 3600);
            const minutes = Math.floor((seconds % 3600) / 60);
            const secs = seconds % 60;
            return `${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
        }
        
        function formatTime(timestamp) {
            const date = new Date(timestamp);
            return date.toLocaleTimeString();
        }
        
        async function updateStatus() {
            try {
                const response = await fetch('/api/status');
                const data = await response.json();
                
                updateConnectionStatus(true);
                
                const indicator = document.getElementById('indicator');
                const statusText = document.getElementById('statusText');
                const lastDetection = document.getElementById('lastDetection');
                const detectionCount = document.getElementById('detectionCount');
                const uptime = document.getElementById('uptime');
                
                if (data.metalDetected) {
                    indicator.className = 'detection-indicator detected';
                    indicator.textContent = 'ALERT';
                    statusText.textContent = 'METAL DETECTED!';
                } else {
                    indicator.className = 'detection-indicator safe';
                    indicator.textContent = 'SAFE';
                    statusText.textContent = 'No Metal Detected';
                }
                
                lastDetection.textContent = data.lastDetectionTime > 0 ? 
                    `Last Detection: ${formatTime(data.lastDetectionTime)}` : 
                    'Last Detection: Never';
                
                detectionCount.textContent = data.detectionCount;
                uptime.textContent = formatUptime(Math.floor(data.uptime / 1000));
                
            } catch (error) {
                console.error('Error fetching status:', error);
                updateConnectionStatus(false);
            }
        }
        
        // Auto-update every 500ms
        setInterval(updateStatus, 500);
        
        // Initial update
        updateStatus();
    </script>
</body>
</html>
)rawliteral";
  
  server.send(200, "text/html", html);
}

void handleStatus() {
  StaticJsonDocument<200> doc;
  
  doc["metalDetected"] = metalDetected;
  doc["detectionCount"] = detectionCount;
  doc["lastDetectionTime"] = lastDetectionTime;
  doc["uptime"] = millis() - systemStartTime;
  doc["wifiConnected"] = WiFi.status() == WL_CONNECTED;
  doc["ipAddress"] = WiFi.localIP().toString();
  
  String response;
  serializeJson(doc, response);
  
  server.send(200, "application/json", response);
}

void handleDetection() {
  StaticJsonDocument<100> doc;
  
  doc["detected"] = metalDetected;
  doc["timestamp"] = millis();
  
  String response;
  serializeJson(doc, response);
  
  server.send(200, "application/json", response);
}  for this
