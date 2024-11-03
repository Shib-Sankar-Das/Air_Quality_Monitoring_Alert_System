#include <WiFi.h>
#include <FirebaseESP32.h>

// WiFi credentials
#define WIFI_SSID "B_Cable_Network"
#define WIFI_PASSWORD "9903585313"

// Firebase credentials
#define FIREBASE_HOST "https://air-pollution-detection-6d311-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "nupYKl08V7bVUJwMQnR4pQAZNYYxwfKtpd18gpoH"

// Hardware pin definitions
#define MQ2_PIN 34       // Analog pin for MQ-2
#define BUZZER_PIN 13    // Digital pin for Buzzer
#define LED_PIN 2        // Digital pin for LED

// Threshold for air quality
#define AIR_QUALITY_THRESHOLD 1300

// Initialize Firebase
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// Variables to store air quality and emergency status
int airQualityValue;
bool emergency = false;

void setup() {
  Serial.begin(115200);

  // Set up WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // Set up Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Set up pins
  pinMode(MQ2_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  // Initialize buzzer and LED states
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  // Read air quality value from MQ-2
  airQualityValue = analogRead(MQ2_PIN);
  Serial.print("Air Quality: ");
  Serial.println(airQualityValue);

  // Check if air quality exceeds threshold
  if (airQualityValue > AIR_QUALITY_THRESHOLD) {
    // Trigger emergency actions
    emergency = true;
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Reset emergency status
    emergency = false;
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
  }

  // Update Firebase with air quality and emergency status
  if (Firebase.ready()) {
    if (!Firebase.setInt(firebaseData, "/AirQuality", airQualityValue)) {
      Serial.print("Error setting AirQuality: ");
      Serial.println(firebaseData.errorReason());
    }

    if (!Firebase.setBool(firebaseData, "/Emergency", emergency)) {
      Serial.print("Error setting Emergency: ");
      Serial.println(firebaseData.errorReason());
    }
  } else {
    Serial.println("Firebase not ready");
  }

  delay(5000); // Adjust delay for your needs
}
