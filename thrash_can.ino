#include <Servo.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>

// Pin Definitions
const int TRIG_PIN = 6;    // Ultrasonic Sensor TRIG pin
const int ECHO_PIN = 7;    // Ultrasonic Sensor ECHO pin
const int SERVO_LID_PIN = 9;  // Servo for lid
const int SERVO_THROW_PIN = 10; // Servo for throwing mechanism (optional)
const int RX_PIN = 2;      // DFPlayer RX pin
const int TX_PIN = 3;      // DFPlayer TX pin

// Constants
const int DISTANCE_THRESHOLD = 50;   // Distance in centimeters to trigger lid
const int TRASH_DETECT_THRESHOLD = 20; // Distance to detect trash being thrown
const int LID_OPEN_ANGLE = 180;
const int LID_CLOSE_ANGLE = 0;

// Object Declarations
Servo servoLid;
Servo servoThrow;  // Optional: for automatic trash throwing mechanism
SoftwareSerial mySerial(RX_PIN, TX_PIN);
DFPlayerMini_Fast myMP3;

// Variables
float duration_us, distance_cm;
unsigned long lastTrashDetectTime = 0;
const int TRASH_DETECT_DELAY = 3000; // 3 seconds between thank you messages

void setup() {
  Serial.begin(9600);
  
  // Ultrasonic Sensor Setup
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Servo Setup
  servoLid.attach(SERVO_LID_PIN);
  servoLid.write(LID_CLOSE_ANGLE);
  
  // Optional: Throw Servo Setup
  // servoThrow.attach(SERVO_THROW_PIN);
  
  // DFPlayer Setup
  mySerial.begin(9600);
  if (!myMP3.begin(mySerial)) {
    Serial.println(F("DFPlayer initialization failed!"));
    while(true); // Stop further execution
  }
  
  myMP3.volume(20); // Set volume (0-30)
}

void loop() {
  // Measure distance
  distance_cm = measureDistance();
  
  // Control lid based on proximity
  if (distance_cm < DISTANCE_THRESHOLD) {
    servoLid.write(LID_OPEN_ANGLE);
  } else {
    servoLid.write(LID_CLOSE_ANGLE);
  }
  
  // Detect trash being thrown
  if (distance_cm < TRASH_DETECT_THRESHOLD) {
    if (millis() - lastTrashDetectTime > TRASH_DETECT_DELAY) {
      playThankYouMessage();
      lastTrashDetectTime = millis();
    }
  }
  
  delay(500);
}

float measureDistance() {
  // Generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate the distance
  return 0.017 * duration_us;
}

void playThankYouMessage() {
  // Assuming you have a file named "thanks.mp3" in the root directory of the SD card
  myMP3.play(1);  // Play first track
}
