#include <AFMotor.h>
#include <Servo.h>

// Moters on M1 and M2
AF_DCMotor motorLeft(1);
AF_DCMotor motorRight(2);

// Servvo for scannig left/right
Servo scanServo;

// Ultrasonic pns
#define TRIG A1
#define ECHO A2

// Stuck detction helpres
int stuckCount = 0;   // keeps incrasing if bot isnt moving proparly
long lastDist = 0;    // prevous distnce value

// -----------------------
// Get distnce in cm
// -----------------------
long getDistance() {

  // Send triggr pulss
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // Read echo with timout (stops freezng)
  long duration = pulseIn(ECHO, HIGH, 60000); // longer timeout

  // Convert microsecs → cm
  long dist = duration * 0.034 / 2;

  // If bad readng, limit to mx 400 cm
  if (dist == 0 || dist > 400) dist = 400;

  return dist;
}

// -----------------------
// Moter helper functons
// -----------------------
void forward() {
  motorLeft.setSpeed(180);
  motorRight.setSpeed(180);
  motorLeft.run(FORWARD);
  motorRight.run(FORWARD);
}

void stopBot() {
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
}

void turnLeft() {
  motorLeft.setSpeed(180);
  motorRight.setSpeed(180);
  motorLeft.run(BACKWARD);
  motorRight.run(FORWARD);
  delay(400);
  stopBot();
  delay(150); 
}

void turnRight() {
  motorLeft.setSpeed(180);
  motorRight.setSpeed(180);
  motorLeft.run(FORWARD);
  motorRight.run(BACKWARD);
  delay(400);
  stopBot();
  delay(150);
}

// -----------------------
// Detect if bot is stck
// -----------------------
void checkStuck(long d) {

  // If distnce isnt changng much → bot might be stuk
  if (abs(d - lastDist) < 5) {
    stuckCount++;
  } else {
    stuckCount = 0;
  }

  // If stuck too lng → revese out
  if (stuckCount >= 5) {
    motorLeft.setSpeed(180);
    motorRight.setSpeed(180);
    motorLeft.run(BACKWARD);
    motorRight.run(BACKWARD);
    delay(500);

    stopBot();       // ← FIXED
    delay(200);

    stuckCount = 0;
  }

  lastDist = d;
}

// -----------------------
// Setup runs onnce
// -----------------------
void setup() {
  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  scanServo.attach(10); 
  scanServo.write(90);  // cnter positon
}

// -----------------------
// Main loop runs forevr
// -----------------------
void loop() {

  long front = getDistance();
  checkStuck(front);

  // Path is clr → go forwrd
  if (front > 30) {
    forward();
  } 
  else {
    // Obstrcle → stop and scan
    stopBot();
    delay(200);

    // Look left (safer angle)
    scanServo.write(30);
    delay(400);
    long left = getDistance();

    // Look right (safer angle)
    scanServo.write(150);
    delay(400);
    long right = getDistance();

    // Back to cnter
    scanServo.write(90);
    delay(200);

    if (left > right) turnLeft();
    else turnRight();
  }

  delay(50);
}
