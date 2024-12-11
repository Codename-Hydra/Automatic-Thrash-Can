#include <Servo.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>

// Pin Definitions
const int TRIG_PIN = 6;    // Ultrasonic Sensor TRIG pin
const int ECHO_PIN = 7;    // Ultrasonic Sensor ECHO pin
const int SERVO_LID_PIN = 9;   // Servo untuk tutup tempat sampah
const int SERVO_THROW_PIN = 10; // Servo untuk mekanisme membuang sampah
const int RX_PIN = 2;      // DFPlayer RX pin
const int TX_PIN = 3;      // DFPlayer TX pin

// Constants
const int DISTANCE_THRESHOLD = 50;   // Jarak dalam centimeter untuk membuka tutup
const int TRASH_DETECT_THRESHOLD = 20; // Jarak untuk mendeteksi sampah dibuang
const int SERVO_OPEN_ANGLE = 90;     // Sudut buka servo
const int SERVO_CLOSE_ANGLE = 0;     // Sudut tutup servo

// Object Declarations
Servo servoLid;
Servo servoThrow;
SoftwareSerial mySerial(RX_PIN, TX_PIN);
DFPlayerMini_Fast myMP3;

// Variables
float duration_us, distance_cm;
unsigned long lastTrashDetectTime = 0;
const int TRASH_DETECT_DELAY = 3000; // Jeda 3 detik antara ucapan terima kasih

void setup() {
  Serial.begin(9600);
  
  // Ultrasonic Sensor Setup
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Servo Setup
  servoLid.attach(SERVO_LID_PIN);
  servoLid.write(SERVO_CLOSE_ANGLE);
  
  servoThrow.attach(SERVO_THROW_PIN);
  servoThrow.write(SERVO_CLOSE_ANGLE);
  
  // DFPlayer Setup
  mySerial.begin(9600);
  if (!myMP3.begin(mySerial)) {
    Serial.println(F("Inisialisasi DFPlayer gagal!"));
    while(true); // Hentikan eksekusi lebih lanjut
  }
  
  myMP3.volume(20); // Atur volume (0-30)
}

void loop() {
  // Ukur jarak
  distance_cm = measureDistance();
  
  // Kontrol servo berdasarkan jarak
  if (distance_cm < DISTANCE_THRESHOLD) {
    // Buka kedua servo 90 derajat secara bersamaan
    servoLid.write(SERVO_OPEN_ANGLE);
    servoThrow.write(SERVO_OPEN_ANGLE);
  } else {
    // Tutup kedua servo
    servoLid.write(SERVO_CLOSE_ANGLE);
    servoThrow.write(SERVO_CLOSE_ANGLE);
  }
  
  // Deteksi sampah dibuang
  if (distance_cm < TRASH_DETECT_THRESHOLD) {
    if (millis() - lastTrashDetectTime > TRASH_DETECT_DELAY) {
      playThankYouMessage();
      lastTrashDetectTime = millis();
    }
  }
  
  delay(500);
}

float measureDistance() {
  // Hasilkan pulsa 10-mikrodetik ke pin TRIG
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Ukur durasi pulsa dari pin ECHO
  duration_us = pulseIn(ECHO_PIN, HIGH);
  
  // Hitung jarak
  return 0.017 * duration_us;
}

void playThankYouMessage() {
  // Putar file MP3 pertama dari kartu SD
  // Pastikan Anda telah mengupload file MP3 ucapan terima kasih ke kartu SD
  myMP3.play(1);  
}
