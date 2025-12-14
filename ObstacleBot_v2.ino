#include <AFMotor.h>
#include <NewPing.h>
#include <Servo.h>

// --- 🎯 MOTOR CALIBRATION FACTORS (YOU MUST TUNE THESE!) ---
// If the robot veers RIGHT, reduce LEFT_CALIBRATION (e.g., 255 -> 230)
// If the robot veers LEFT, reduce RIGHT_CALIBRATION (e.g., 255 -> 230)
// Start at 255 and reduce the value for the stronger motor until the bot drives straight.
#define LEFT_CALIBRATION  255 
#define RIGHT_CALIBRATION 255 
// ----------------------------------------------------

AF_DCMotor motorLeft(1);
AF_DCMotor motorRight(4);

#define TRIG_PIN A1
#define ECHO_PIN A2
#define MAX_DISTANCE 400
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

Servo scanServo;
#define SERVO_PIN 10

// --- Utility Functions ---

unsigned int readAvg() {
  unsigned long sum = 0;
  for (int i=0; i<3; i++) {
    unsigned int d = sonar.ping_cm();
    if (d == 0) d = 400; 
    sum += d;
    delay(40); // Delay between pings for stability
  }
  return sum / 3;
}

void stopBot() {
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
  delay(150);
}

// Full power FORWARD, uses calibration to ensure straight line
void boostForward(int ms) {
  motorLeft.setSpeed(LEFT_CALIBRATION);
  motorRight.setSpeed(RIGHT_CALIBRATION);
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
  delay(ms);
}

// Strong reverse to quickly gain distance from obstacle
void reverseSlow(int ms) {
  // Use maximum power possible (255) if the low-current batteries allow it, 
  // adjusted by the calibration factor.
  motorLeft.setSpeed(LEFT_CALIBRATION); 
  motorRight.setSpeed(RIGHT_CALIBRATION); 
  motorLeft.run(BACKWARD);
  motorRight.run(BACKWARD);
  delay(ms);
  stopBot();
}

// Aggressive Power Pivot Left (Left drags, Right pushes)
void pivotLeftPower() {
  motorLeft.run(RELEASE); 
  motorRight.setSpeed(RIGHT_CALIBRATION); 
  motorRight.run(FORWARD);
  delay(2000);   // AGGRESSIVE DURATION for heavy chassis
  stopBot();
}

// Aggressive Power Pivot Right (Right drags, Left pushes)
void pivotRightPower() {
  motorRight.run(RELEASE); 
  motorLeft.setSpeed(LEFT_CALIBRATION); 
  motorLeft.run(FORWARD);
  delay(2000); // AGGRESSIVE DURATION for heavy chassis
  stopBot();
}

// --- Setup and Loop ---

void setup() {
  Serial.begin(9600);
  scanServo.attach(SERVO_PIN);
  scanServo.write(90); 
  stopBot();
}

void loop() {
  scanServo.write(90);
  delay(200);
  unsigned int front = readAvg();

  // 1. Go Forward if Clear
  if (front > 30) { 
    boostForward(80);
    return;
  }

  // 2. Stop and Reverse
  stopBot();
  reverseSlow(400);  // Reverse before scan

  // 3. Scan Left
  scanServo.write(30);
  delay(450);
  unsigned int left = readAvg();

  // 4. Scan Right
  scanServo.write(150);
  delay(450);
  unsigned int right = readAvg();

  scanServo.write(90); // Return servo to center
  delay(200);

  // 5. Decision Making
  // Use a safety buffer (5cm) to prevent flickering
  if (left > right + 5) {
    pivotLeftPower();
  } else if (right > left + 5) {
    pivotRightPower();
  } else {
    // If distances are similar (dead-end), reverse again and choose a default turn.
    reverseSlow(500); 
    pivotRightPower(); // Default turn bias is Right
  }
}
