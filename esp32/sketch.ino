#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// =================================================
// PIN DEFINITIONS
// =================================================

#define GAS_PIN       34
#define LDR_PIN       27
#define DHT_PIN       4
#define PIR_PIN       26
#define BUTTON_PIN    33

#define BUZZER_PIN    25

#define GREEN_LED     12
#define YELLOW_LED    13
#define RED_LED       14

#define I2C_SDA       21
#define I2C_SCL       22

// =================================================
// SENSOR / LCD
// =================================================

#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

LiquidCrystal_I2C lcd(0x27, 20, 4);

// =================================================
// THRESHOLDS
// =================================================

const int GAS_WARNING  = 1800;
const int GAS_CRITICAL = 2800;

const float TEMP_WARNING  = 35.0;
const float TEMP_CRITICAL = 45.0;

// =================================================
// WIFI
// =================================================

const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

const char* API_URL = "https://YOUR-TUNNEL-URL/api/events";

// =================================================
// TIMING
// =================================================

unsigned long lastLCDUpdate = 0;
const unsigned long LCD_INTERVAL = 1000;

unsigned long lastNotification = 0;
const unsigned long NOTIFICATION_INTERVAL = 2000;

// =================================================
// BUTTON
// =================================================

bool lastButtonState = HIGH;

unsigned long lastButtonPress = 0;
const unsigned long BUTTON_DEBOUNCE = 250;

// =================================================
// ALARM MUTE
// =================================================

// When true, automatic alarm is temporarily muted
bool alarmMuted = false;
bool manualEmergency = false;
bool buzzerActive = false;

unsigned long alarmMuteStart = 0;

// Mute duration = 5 seconds
const unsigned long ALARM_MUTE_TIME = 5000;

// =================================================
// SYSTEM
// =================================================

String previousState = "";


// =================================================
// SETUP
// =================================================

void setup() {

  Serial.begin(115200);

  pinMode(GAS_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  dht.begin();

  Wire.begin(I2C_SDA, I2C_SCL);

  lcd.init();
  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("    SAFEHOSTEL");

  lcd.setCursor(0, 1);
  lcd.print("Safety Monitoring");

  lcd.setCursor(0, 2);
  lcd.print("System Starting");

  delay(2000);

  allOff();

  connectWiFi();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SYSTEM READY");

  lcd.setCursor(0, 1);

  if (WiFi.status() == WL_CONNECTED)
    lcd.print("WiFi: CONNECTED");
  else
    lcd.print("WiFi: OFFLINE");

  delay(1500);

  lcd.clear();
}


// =================================================
// MAIN LOOP
// =================================================

void loop() {

  // =================================================
  // READ SENSORS
  // =================================================

  int gasValue = analogRead(GAS_PIN);

  int ldrValue = digitalRead(LDR_PIN);

  int motion = digitalRead(PIR_PIN);

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature))
    temperature = 0;

  if (isnan(humidity))
    humidity = 0;


  // Wokwi LDR = flame/light proxy
  bool flameDetected = (ldrValue == LOW);


  // =================================================
  // CHECK MUTE TIMER
  // =================================================

  if (alarmMuted) {

    if (millis() - alarmMuteStart >= ALARM_MUTE_TIME) {

      alarmMuted = false;

      Serial.println("5 SECOND MUTE FINISHED");
      Serial.println("AUTOMATIC DETECTION RESUMED");
    }
  }


  // =================================================
  // DETECT HAZARDS
  // =================================================

  bool gasWarning =
    gasValue >= GAS_WARNING;

  bool gasCritical =
    gasValue >= GAS_CRITICAL;


  bool temperatureWarning =
    temperature >= TEMP_WARNING;

  bool temperatureCritical =
    temperature >= TEMP_CRITICAL;


  // =================================================
  // CALCULATE RISK
  // =================================================

  int riskScore = 0;


  if (gasCritical)
    riskScore += 60;

  else if (gasWarning)
    riskScore += 30;


  if (temperatureCritical)
    riskScore += 40;

  else if (temperatureWarning)
    riskScore += 20;


  if (flameDetected)
    riskScore += 60;

  if (riskScore > 100)
    riskScore = 100;


  // =================================================
  // ANY AUTOMATIC HAZARD?
  // =================================================

  bool hazardDetected =
    gasWarning ||
    temperatureWarning ||
    flameDetected;

  handleButton(buzzerActive);


  // =================================================
  // ALARM REASON
  // =================================================

  String alarmReason = manualEmergency
    ? "MANUAL EMERGENCY"
    : getAlarmReason(gasWarning, temperatureWarning, flameDetected);


  // =================================================
  // STATUS
  // =================================================

  String status;


  if (manualEmergency) {

    status = "MANUAL";

  }

  else if (flameDetected) {

    status = "FIRE ALERT";

  }

  else if (riskScore >= 100) {

    status = "CRITICAL";

  }

  else if (riskScore >= 60) {

    status = "HIGH";

  }

  else if (riskScore >= 30) {

    status = "WARNING";

  }

  else {

    status = "SAFE";

  }


  // =================================================
  // BUZZER LOGIC
  // =================================================

  bool alarmActive = hazardDetected || manualEmergency;
  buzzerActive = alarmActive && !alarmMuted;
  digitalWrite(BUZZER_PIN, buzzerActive ? HIGH : LOW);


  // =================================================
  // LED LOGIC
  // =================================================

  if (alarmMuted) {

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);

  }

  else if (!hazardDetected && !manualEmergency) {

    // SAFE

    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, LOW);

  }

  else if (manualEmergency || riskScore >= 60 || flameDetected) {

    // Serious hazard

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(RED_LED, HIGH);

  }

  else {

    // Warning

    digitalWrite(GREEN_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  }


  // =================================================
  // LCD
  // =================================================

  if (millis() - lastLCDUpdate >= LCD_INTERVAL) {

    lastLCDUpdate = millis();

    updateLCD(
      status,
      alarmReason,
      gasValue,
      temperature,
      humidity,
      motion,
      flameDetected,
      riskScore,
      manualEmergency
    );
  }


  // =================================================
  // API NOTIFICATION
  // =================================================

  String currentState =
    status + "|" +
    alarmReason + "|" +
    String(gasValue) + "|" +
    String(temperature, 1) + "|" +
    String(humidity, 0) + "|" +
    String(flameDetected) + "|" +
    String(riskScore) + "|" +
    String(alarmMuted) + "|" +
    String(manualEmergency) + "|" +
    String(buzzerActive);


  if (currentState != previousState ||
      millis() - lastNotification >= NOTIFICATION_INTERVAL) {

    if (millis() - lastNotification >= NOTIFICATION_INTERVAL ||
        lastNotification == 0) {

      sendNotification(
        status,
        alarmReason,
        gasValue,
        temperature,
        humidity,
        motion,
        flameDetected,
        riskScore,
        alarmMuted,
        manualEmergency,
        buzzerActive
      );

      lastNotification = millis();
      previousState = currentState;
    }
  }


  // =================================================
  // SERIAL MONITOR
  // =================================================

  Serial.println("--------------------------------");

  Serial.print("Gas: ");
  Serial.println(gasValue);

  Serial.print("Temperature: ");
  Serial.println(temperature, 1);

  Serial.print("Humidity: ");
  Serial.println(humidity, 0);

  Serial.print("PIR: ");
  Serial.println(motion ? "YES" : "NO");

  Serial.print("Flame: ");
  Serial.println(flameDetected ? "YES" : "NO");

  Serial.print("Risk: ");
  Serial.println(riskScore);

  Serial.print("Status: ");
  Serial.println(status);

  Serial.print("Reason: ");
  Serial.println(alarmReason);

  Serial.print("Alarm Muted: ");
  Serial.println(alarmMuted ? "YES" : "NO");

  Serial.print("Manual Emergency: ");
  Serial.println(manualEmergency ? "YES" : "NO");

  delay(50);
}


// =================================================
// BUTTON HANDLER
// =================================================

void handleButton(bool alarmIsActive) {

  bool currentButtonState =
    digitalRead(BUTTON_PIN);


  // Detect button press
  if (lastButtonState == HIGH &&
      currentButtonState == LOW) {

    if (millis() - lastButtonPress >
        BUTTON_DEBOUNCE) {

      lastButtonPress = millis();


      // ---------------------------------------------
      // BUTTON PRESSED
      // ---------------------------------------------

      if (alarmIsActive) {

        alarmMuted = true;
        manualEmergency = false;
        alarmMuteStart = millis();

        Serial.println("ALARM MUTED FOR 5 SECONDS");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ALARM MUTED");
        lcd.setCursor(0, 1);
        lcd.print("AUTO RESUMES IN 5S");

      }

      else {

        alarmMuted = false;
        manualEmergency = true;

        Serial.println("MANUAL EMERGENCY ACTIVATED");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MANUAL EMERGENCY");
        lcd.setCursor(0, 1);
        lcd.print("BUZZER ACTIVE");
      }
    }
  }


  lastButtonState =
    currentButtonState;
}


// =================================================
// ALARM REASON
// =================================================

String getAlarmReason(
  bool gas,
  bool temperature,
  bool flame
) {

  String reason = "";


  if (gas) {

    reason += "GAS";
  }


  if (flame) {

    if (reason != "")
      reason += "+";

    reason += "FIRE";
  }


  if (temperature) {

    if (reason != "")
      reason += "+";

    reason += "TEMP";
  }


  if (reason == "")
    reason = "NONE";


  return reason;
}


// =================================================
// LCD UPDATE
// =================================================

void updateLCD(
  String status,
  String reason,
  int gasValue,
  float temperature,
  float humidity,
  int motion,
  bool flameDetected,
  int riskScore,
  bool manual
) {

  // -----------------------------------------------
  // LINE 1
  // -----------------------------------------------

  lcd.setCursor(0, 0);

  lcd.print("STATUS:");
  lcd.print(status);

  lcd.print("             ");


  // -----------------------------------------------
  // LINE 2
  // -----------------------------------------------

  lcd.setCursor(0, 1);

  lcd.print("G:");
  lcd.print(gasValue);

  lcd.print(" T:");
  lcd.print(temperature, 1);

  lcd.print("C       ");


  // -----------------------------------------------
  // LINE 3
  // -----------------------------------------------

  lcd.setCursor(0, 2);

  lcd.print("H:");
  lcd.print(humidity, 0);

  lcd.print("% R:");
  lcd.print(riskScore);

  lcd.print(" PIR:");
  lcd.print(motion ? "Y" : "N");

  lcd.print(" ");


  // -----------------------------------------------
  // LINE 4
  // -----------------------------------------------

  lcd.setCursor(0, 3);


  if (alarmMuted) {

    lcd.print("MUTED: ");
    lcd.print(5 -
      ((millis() - alarmMuteStart) / 1000));

    lcd.print("S AUTO RESUME");
  }

  else if (manual) {

    lcd.print("MANUAL EMERGENCY");
  }

  else if (reason != "NONE") {

    lcd.print("ALARM:");
    lcd.print(reason);

    lcd.print("          ");
  }

  else {

    lcd.print("SAFE  FIRE:");
    lcd.print(flameDetected ? "YES" : "NO");

    lcd.print("    ");
  }
}


// =================================================
// WIFI CONNECTION
// =================================================

void connectWiFi() {

  Serial.println();
  Serial.print("Connecting WiFi...");

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );


  int attempts = 0;


  while (
    WiFi.status() != WL_CONNECTED &&
    attempts < 30
  ) {

    delay(500);

    Serial.print(".");

    attempts++;
  }


  Serial.println();


  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi Connected!");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

  }

  else {

    Serial.println(
      "WiFi connection failed."
    );

    Serial.println(
      "Running in offline mode."
    );
  }
}


// =================================================
// API NOTIFICATION
// =================================================

void sendNotification(
  String status,
  String reason,
  int gasValue,
  float temperature,
  float humidity,
  int motion,
  bool flameDetected,
  int riskScore,
  bool muted,
  bool manual,
  bool alarm
) {

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println(
      "WiFi unavailable."
    );

    return;
  }


  HTTPClient http;
  WiFiClientSecure secureClient;
  secureClient.setInsecure();

  http.begin(secureClient, API_URL);

  http.addHeader(
    "Content-Type",
    "application/json"
  );


  String json = "{";


  json += "\"status\":\"";
  json += status;
  json += "\",";


  json += "\"reason\":\"";
  json += reason;
  json += "\",";


  json += "\"gas\":";
  json += String(gasValue);
  json += ",";


  json += "\"temperature\":";
  json += String(temperature, 1);
  json += ",";


  json += "\"humidity\":";
  json += String(humidity, 0);
  json += ",";


  json += "\"pir\":";
  json += motion ? "true" : "false";
  json += ",";


  json += "\"fire\":";
  json += flameDetected
    ? "true,"
    : "false,";


  json += "\"risk_score\":";
  json += String(riskScore);
  json += ",";


  json += "\"alarm_muted\":";
  json += muted
    ? "true,"
    : "false,";

  json += "\"manual_emergency\":";
  json += manual
    ? "true,"
    : "false,";

  json += "\"alarm\":";
  json += alarm
    ? "true,"
    : "false,";

  unsigned long muteRemaining = muted
    ? (ALARM_MUTE_TIME - (millis() - alarmMuteStart) + 999) / 1000
    : 0;

  json += "\"mute_remaining_seconds\":";
  json += String(muteRemaining);


  json += "}";


  Serial.println();
  Serial.println("API DATA:");
  Serial.println(json);


  int responseCode =
    http.POST(json);


  Serial.print(
    "API Response: "
  );

  Serial.println(responseCode);


  http.end();
}


// =================================================
// ALL OUTPUTS OFF
// =================================================

void allOff() {

  digitalWrite(
    GREEN_LED,
    LOW
  );

  digitalWrite(
    YELLOW_LED,
    LOW
  );

  digitalWrite(
    RED_LED,
    LOW
  );

  digitalWrite(
    BUZZER_PIN,
    LOW
  );
}