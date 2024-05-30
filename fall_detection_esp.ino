#include <FastLED.h>
#include "random_forest.h" // Include the Random Forest model header file
#include <Wire.h>
#include "FastIMU.h" // Include the FastIMU library for handling IMU sensors
#include <WiFi.h>
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h>
#include "config.h" 
#include <string>
#include <WiFiClientSecure.h>

#define RED_PIN 26 
#define GREEN_PIN 18 
#define BLUE_PIN 19 
#define NUM_LEDS 1 // Number of NeoPixel LEDs

CRGB leds[NUM_LEDS]; // Array to store color information for each LED

int32_t length = 6; // Number of inputs expected by the model

// IMU configuration
#define IMU_ADDRESS 0x68 // Change to the address of the IMU
#define PERFORM_CALIBRATION // Comment to disable startup calibration
MPU6500 IMU; // Change to the name of any supported IMU!

// Calibration data structure
calData calib = { 0 };
AccelData accelData; // Sensor data structure for acceleration
GyroData gyroData; // Sensor data structure for gyroscope

// Telegram Bot
WiFiClientSecure client;
UniversalTelegramBot bot(token, client);


void setup() {
  // Initialize LEDs
  FastLED.addLeds<WS2812B, RED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // Set pin modes
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  // Initialize I2C communication
  Wire.begin();
  Wire.setClock(400000); // 400kHz clock speed

  // Start serial communication
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }

  // Initialize IMU
  int err = IMU.init(calib, IMU_ADDRESS);
  if (err!= 0) {
    Serial.print("Error initializing IMU: ");
    Serial.println(err);
    while (true) {
      ; // Stop execution on error
    }
  }
  
  // Perform calibration if enabled
  #ifdef PERFORM_CALIBRATION

  delay(5000);
  Serial.println("Keep IMU level.");
  delay(5000);
  IMU.calibrateAccelGyro(&calib);
  Serial.println("Calibration done!");
  Serial.println("Accel biases X/Y/Z: ");
  Serial.print(calib.accelBias[0]);
  Serial.print(", ");
  Serial.print(calib.accelBias[1]);
  Serial.print(", ");
  Serial.println(calib.accelBias[2]);
  Serial.println("Gyro biases X/Y/Z: ");
  Serial.print(calib.gyroBias[0]);
  Serial.print(", ");
  Serial.print(calib.gyroBias[1]);
  Serial.print(", ");
  Serial.println(calib.gyroBias[2]);

  delay(5000);
  IMU.init(calib, IMU_ADDRESS);
  #endif

  // Set IMU ranges (optional)
  // err = IMU.setGyroRange(500); // Set gyro range to ±500 dps
  // err = IMU.setAccelRange(2); // Set accelerometer range to ±2g

  if (err!= 0) {
    Serial.print("Error Setting range: ");
    Serial.println(err);
    while (true) {
      ; // Stop execution on error
    }
  }

  // WiFi Connection
  WiFi.begin(ssid, password);
  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // Collect data from sensor
  IMU.update();
  IMU.getAccel(&accelData);
  Serial.print(accelData.accelX);
  Serial.print("\t");
  Serial.print(accelData.accelY);
  Serial.print("\t");
  Serial.print(accelData.accelZ);
  Serial.print("\t");
  IMU.getGyro(&gyroData);
  Serial.print(gyroData.gyroX);
  Serial.print("\t");
  Serial.print(gyroData.gyroY);
  Serial.print("\t");
  Serial.print(gyroData.gyroZ);
  Serial.print("\n");
  delay(10);

  // Make inference with the model
  float input[6] = {accelData.accelX, accelData.accelY, accelData.accelZ, gyroData.gyroX, gyroData.gyroY, gyroData.gyroZ};
  const int32_t predicted_class = random_forest_predict(input, length);
  Serial.print(predicted_class);
  
  // Check if a fall was detected
  if (predicted_class == 1) {
    // Blink the red LED
    leds[0] = CRGB(255, 0, 0); // Set the LED to full brightness red
    FastLED.show();
    delay(1000); // Wait for 1 second

    leds[0] = CRGB::Black; // Set the LED to off
    FastLED.show();
    delay(1000); // Wait for 1 second

    // Print the result to the Serial Monitor
    Serial.println("Possible fall detected.");

    // Send Message
    String message = "Possible fall detected.";
    String chatIdString = String(chat_id);
    bot.sendMessage(chatIdString, message,"");
  } else {
    // Print the result to the Serial Monitor
    Serial.println("Daily activity detected!");
  }

  delay(100); // Adjust the delay time as needed
}
