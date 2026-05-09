#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150
#define SERVOMAX  600
#define SERVO_CHANNEL_RIPE 4      // Ripe Servo
#define SERVO_CHANNEL_RAW 5       // Raw Servo
#define SERVO_CHANNEL_RIPE_SIZE 2 // Ripe 360 Size Servo
#define SERVO_CHANNEL_RAW_SIZE 3  // Raw 360 Size Servo

const int trigPin = 7, echoPin = 6;    // Ultrasonic Sensor Pins D7 and D6

int qualityServoStart = 90;       // Ripe & Raw Servo Start Position
int sizeServoStart = 90;         // Size Servo Start Position
int currentSizeServoAngle = 90;

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);  // Set PWM frequency
  delay(15);

  // Initialize the Ripe & Raw servo
  int pulseLength2 = map(qualityServoStart, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(SERVO_CHANNEL_RIPE, 0, pulseLength2);
  pwm.setPWM(SERVO_CHANNEL_RAW, 0, pulseLength2);

  // Initialize the Ripe & Raw Size Sevo
  int pulseLength3 = map(sizeServoStart, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(SERVO_CHANNEL_RIPE_SIZE, 0, pulseLength3);
  pwm.setPWM(SERVO_CHANNEL_RAW_SIZE, 0, pulseLength3);

  // Initialize Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  if (Serial.available()) {
    char received = Serial.read();
    if (received == 'R' || received == 'P') {
      if (received == 'R') {
        // Move from Servo 90Â° to 160Â°
        for (int angle = 90; angle <= 160; angle++) {
          int pulseLength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
          pwm.setPWM(SERVO_CHANNEL_RAW, 0, pulseLength);
          delay(1);
        }
        
        // Categorized Size Small, Medium, Large
        while (Serial.available() < 1);
        char size = Serial.read();
        
        int targetAngle;
        if (size == 'S') {
            targetAngle = -20;
        } else if (size == 'M') {
            targetAngle = 90;
        } else if (size == 'L') {
            targetAngle = 220;
        } else {
            return;
        }

        int currentAngle = currentSizeServoAngle;
        while (currentAngle != targetAngle) {
            currentAngle += (currentAngle < targetAngle) ? 1 : -1;
            int pulseLength = map(currentAngle, 0, 180, SERVOMIN, SERVOMAX);
            pwm.setPWM(SERVO_CHANNEL_RAW_SIZE, 0, pulseLength);
            delay(5);
        }
        currentSizeServoAngle = targetAngle;

        long distance; // Scan Object
        while (true) {  // Continuous scan until valid distance is found
          distance = getUltrasonicDistance();
          
          if (distance != -1) { // Valid distance received
            Serial.print("Distance: ");
            Serial.print(distance);
            Serial.println(" inches.");
          }
          
          if (distance <= 10 && distance != -1) { // If distance is below 10 inches, break the loop
            Serial.println("Object detected within 10 inches!");
            break;
          }
          
          delay(200);  // Small delay to avoid excessive scanning
        }

        delay(8000);  // Hold for 8 seconds

        // Move back from 160Â° to 90Â°
        for (int angle = 160; angle >= 90; angle--) {
          int pulseLength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
          pwm.setPWM(SERVO_CHANNEL_RAW, 0, pulseLength);
          delay(10);
        }
      } else if (received == 'P') {
        // Move from 90Â° to 30Â°
        for (int angle = 90; angle >= 30; angle--) {
          int pulseLength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
          pwm.setPWM(SERVO_CHANNEL_RIPE, 0, pulseLength);
          delay(1);
        }
        
        // Categorized Size Small, Medium, Large
        while (Serial.available() < 1);
        char size = Serial.read();
        
        int targetAngle;
        if (size == 'S') {
            targetAngle = -20;
        } else if (size == 'M') {
            targetAngle = 90;
        } else if (size == 'L') {
            targetAngle = 220;
        } else {
            return;
        }

        int currentAngle = currentSizeServoAngle;
        while (currentAngle != targetAngle) {
            currentAngle += (currentAngle < targetAngle) ? 1 : -1;
            int pulseLength = map(currentAngle, 0, 180, SERVOMIN, SERVOMAX);
            pwm.setPWM(SERVO_CHANNEL_RIPE_SIZE, 0, pulseLength);
            delay(5);
        }
        currentSizeServoAngle = targetAngle;

        long distance; // Scan Object
        while (true) {  // Continuous scan until valid distance is found
          distance = getUltrasonicDistance();
          
          if (distance != -1) { // Valid distance received
            Serial.print("Distance: ");
            Serial.print(distance);
            Serial.println(" inches.");
          }
          
          if (distance <= 10 && distance != -1) { // If distance is below 10 inches, break the loop
            Serial.println("Object detected within 10 inches!");
            break;
          }
          
          delay(200);  // Small delay to avoid excessive scanning
        }

        delay(8000);  // Hold for 8 seconds

        // Move back from 30Â° to 90Â°
        for (int angle = 30; angle <= 90; angle++) {
          int pulseLength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
          pwm.setPWM(SERVO_CHANNEL_RIPE, 0, pulseLength);
          delay(10);
        }
      }
    } else {
    Serial.print("Unknown command received: ");
    Serial.println(received);
    }
  }
}

long getUltrasonicDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);  // 30ms timeout

  if (duration == 0) {
    Serial.println("Ultrasonic sensor timeout! Check wiring.");
    return -1;  // Return -1 if no pulse received
  }

  return duration * 0.0133 / 2;  // Convert to inches
}
