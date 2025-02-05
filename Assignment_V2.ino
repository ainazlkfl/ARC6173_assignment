//code change from using ultrasonic to IR sensor for waste level detection
//ultrasonic used to detect approaching person

// Blynk information
#define BLYNK_TEMPLATE_ID "TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Bin"
#define BLYNK_AUTH_TOKEN "token"

#include <WiFi.h>
#include <WiFiClient.h>
#include <string.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials
#define WIFI_SSID "wifi"
#define WIFI_PASS "password"

// Virtual Pin Definitions for Blynk
#define binStat V0
#define switch V1
#define personDist V2

// Hardware Pin Definitions
#define halfIRpin 25
#define fullIRpin 26
#define bLEDpin 33
#define yLEDpin 32
#define rLEDpin 23
#define I2C_SDA_pin 21
#define I2C_SCL_pin 22
#define TRIG_PIN 18
#define ECHO_PIN 19
#define SWITCH_PIN 5

// Constants
#define WIFI_TIMEOUT 30000  // 30 seconds timeout for WiFi connection
#define MAX_DISTANCE 400    // Maximum valid distance for ultrasonic sensor in cm
#define PERSON_DETECTION_THRESHOLD 30  // Distance in cm to detect person

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;
bool systemActive = true;
unsigned long previousMillis = 0;
const long INTERVAL = 1000;

// get distance from ultrasonic sensor
long getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout of 30ms
    if (duration == 0) {
        return -1;
    }
    
    long distance = duration * 0.034 / 2;
    return (distance > MAX_DISTANCE) ? -1 : distance;
}

// Function to update system status and send data to Blynk
String currentLCDText = "";
String currentLCDLine2 = "";
const String SCROLL_MESSAGE = "Please proceed to the nearest bin";
int scrollPosition = 0;
unsigned long lastScrollTime = 0;
const int SCROLL_INTERVAL = 300;  // Scroll speed in milliseconds

void updateSystemStatus() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= INTERVAL) {
        previousMillis = currentMillis;

        // Read sensor values
        int halfIR = digitalRead(halfIRpin);
        int fullIR = digitalRead(fullIRpin);
        long personDistance = getDistance();

        // Send sensor data to Blynk
        Blynk.virtualWrite(personDist, personDistance);

        // Prepare new LCD text
        String newLCDText = "";
        String newLCDLine2 = "";

        // Determine system state and update outputs
        if (!systemActive) {
            // System inactive state
            digitalWrite(rLEDpin, HIGH);
            digitalWrite(yLEDpin, LOW);
            digitalWrite(bLEDpin, LOW);
            newLCDText = "System Inactive";
            newLCDLine2 = SCROLL_MESSAGE;
            Blynk.virtualWrite(binStat, "Inactive");
        }
        // System active states
        else if (!fullIR) {
            // Bin is full, read when LOW
            digitalWrite(rLEDpin, HIGH);
            digitalWrite(yLEDpin, LOW);
            digitalWrite(bLEDpin, LOW);
            newLCDText = "Bin is full";
            newLCDLine2 = SCROLL_MESSAGE;
            Blynk.virtualWrite(binStat, "Full");
        }
        else if (!halfIR) {
            // Bin is half full
            digitalWrite(rLEDpin, LOW);
            digitalWrite(yLEDpin, HIGH);
            digitalWrite(bLEDpin, LOW);
            newLCDText = "System Active";
            newLCDLine2 = "";
            Blynk.virtualWrite(binStat, "Half full");
        }
        else {
            // Bin is empty
            digitalWrite(rLEDpin, LOW);
            digitalWrite(yLEDpin, LOW);
            digitalWrite(bLEDpin, HIGH);
            newLCDText = "System Active";
            newLCDLine2 = "";
            Blynk.virtualWrite(binStat, "Empty");
        }

        // Check for person proximity
        if (systemActive && personDistance > 0 && personDistance < PERSON_DETECTION_THRESHOLD) {
            newLCDLine2 = "Please feed me.";
        }

        // Update first line of LCD if changed
        if (newLCDText != currentLCDText) {
            lcd.setCursor(0, 0);
            lcd.print("                "); // Clear first line
            lcd.setCursor(0, 0);
            lcd.print(newLCDText);
            currentLCDText = newLCDText;
        }

        // Handle scrolling text for second line
        if (newLCDLine2 == SCROLL_MESSAGE) {
            // Scroll the text
            if (currentMillis - lastScrollTime >= SCROLL_INTERVAL) {
                lastScrollTime = currentMillis;
                
                // Clear second line
                lcd.setCursor(0, 1);
                lcd.print("                ");
                
                // Calculate the substring to display (16 characters)
                String displayText = SCROLL_MESSAGE + "    " + SCROLL_MESSAGE;
                String visiblePart = displayText.substring(scrollPosition, scrollPosition + 16);
                
                // Display the text
                lcd.setCursor(0, 1);
                lcd.print(visiblePart);
                
                // Update scroll position
                scrollPosition++;
                if (scrollPosition >= SCROLL_MESSAGE.length() + 4) { 
                    scrollPosition = 0;
                }
            }
        }
        // Handle non-scrolling second line
        else if (newLCDLine2 != currentLCDLine2) {
            lcd.setCursor(0, 1);
            lcd.print("                "); 
            lcd.setCursor(0, 1);
            lcd.print(newLCDLine2);
            currentLCDLine2 = newLCDLine2;
            scrollPosition = 0; // Reset scroll position
        }
    }
}

// Blynk virtual pin handler for system control
BLYNK_WRITE(VP_CONTROL) {
    systemActive = param.asInt();
    updateSystemStatus();
}

void setup() {
    
    Serial.begin(115200);
    
    pinMode(halfIR_pin, INPUT);
    pinMode(fullIR_pin, INPUT);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(bLEDpin, OUTPUT);
    pinMode(yLEDpin, OUTPUT);
    pinMode(rLEDpin, OUTPUT);
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    
    Wire.begin(I2C_SDA_pin, I2C_SCL_pin);

    // Initialize LCD
    lcd.begin(16, 2);  // 16 columns, 2 rows
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Smart Bin System");

    // Connect to WiFi with timeout
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    unsigned long wifiStartTime = millis();
    
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - wifiStartTime > WIFI_TIMEOUT) {
            Serial.println("WiFi connection failed!");
            delay(2000);
            ESP.restart();
        }
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nConnected to WiFi");
    delay(1000);

    // Initialize Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
    
    updateSystemStatus();
}

void loop() {
    Blynk.run();
    updateSystemStatus();
}