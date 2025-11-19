#include <AFMotor.h>        // include motor shield library so we can control motors
#include <Servo.h>          // include servo library so we can control the servo

// Moters on M1 and M2
AF_DCMotor motorLeft(1);    // create left motor object on port M1
AF_DCMotor motorRight(2);   // create right motor object on port M2

// Servvo for scannig left/right
Servo scanServo;            // create servo object for turning ultrasonic

// Ultrasonic pns
#define TRIG A1             // TRIG pin of ultrasonic connected to A1
#define ECHO A2             // ECHO pin of ultrasonic connected to A2

// Stuck detction helpres
int stuckCount = 0;         // count how many loops bot seems “stuck”
long lastDist = 0;          // stores previous distance to compare movement

// -----------------------
// Get distnce in cm
// -----------------------
long getDistance() {        // function that measures distance and returns it

  digitalWrite(TRIG, LOW);           // reset TRIG low
  delayMicroseconds(2);              // wait 2 microseconds
  digitalWrite(TRIG, HIGH);          // send trigger pulse high
  delayMicroseconds(10);             // keep pulse high for 10 microseconds
  digitalWrite(TRIG, LOW);           // end pulse

  long duration = pulseIn(ECHO, HIGH, 60000);  // measure time echo stays high (max 60ms)

  long dist = duration * 0.034 / 2;  // convert time → distance in cm

  if (dist == 0 || dist > 400)       // if reading is invalid or too far
    dist = 400;                      // set to 400 cm max

  return dist;                       // give back distance value
}

// -----------------------
// Moter helper functons
// -----------------------
void forward() {                     // function to make bot go forward
  motorLeft.setSpeed(180);           // set left motor speed
  motorRight.setSpeed(180);          // set right motor speed
  motorLeft.run(FORWARD);            // left motor moves forward
  motorRight.run(FORWARD);           // right motor moves forward
}

void stopBot() {                     // function to stop the bot
  motorLeft.run(RELEASE);            // stop left motor
  motorRight.run(RELEASE);           // stop right motor
}

void turnLeft() {                    // function to turn left
  motorLeft.setSpeed(180);           // set left motor speed
  motorRight.setSpeed(180);          // set right motor speed
  motorLeft.run(BACKWARD);           // left motor goes backwards
  motorRight.run(FORWARD);           // right motor goes forward
  delay(400);                        // delay to complete turning
  stopBot();                         // stop motors after turn
  delay(150);                        // small rest after turning
}

void turnRight() {                   // function to turn right
  motorLeft.setSpeed(180);           // set left motor speed
  motorRight.setSpeed(180);          // set right motor speed
  motorLeft.run(FORWARD);            // left motor goes forward
  motorRight.run(BACKWARD);          // right motor goes backward
  delay(400);                        // wait to finish turning
  stopBot();                         // stop motors
  delay(150);                        // small break
}

// -----------------------
// Detect if bot is stck
// -----------------------
void checkStuck(long d) {            // function to detect if bot is not moving

  if (abs(d - lastDist) < 5) {       // if distance barely changed
    stuckCount++;                    // bot might be stuck → increase counter
  } else {
    stuckCount = 0;                  // distance changed → bot moving normally
  }

  if (stuckCount >= 5) {             // if bot appears stuck for 5 loops

    motorLeft.setSpeed(180);         // set speed for escaping
    motorRight.setSpeed(180);
    motorLeft.run(BACKWARD);         // reverse left motor
    motorRight.run(BACKWARD);        // reverse right motor
    delay(500);                      // reverse for 0.5 seconds

    stopBot();                       // stop motors
    delay(200);                      // small delay

    stuckCount = 0;                  // reset stuck counter
  }

  lastDist = d;                      // save current distance for next loop
}

// -----------------------
// Setup runs onnce
// -----------------------
void setup() {
  Serial.begin(9600);                // start serial monitor at 9600 baud

  pinMode(TRIG, OUTPUT);             // TRIG pin is output
  pinMode(ECHO, INPUT);              // ECHO pin is input

  scanServo.attach(10);              // attach servo to pin 10
  scanServo.write(90);               // move servo to center (90°)
}

// -----------------------
// Main loop runs forevr
// -----------------------
void loop() {

  long front = getDistance();        // measure distance in front
  checkStuck(front);                 // check if bot is stuck

  if (front > 30) {                  // if obstacle is more than 30 cm away
    forward();                       // go forward
  } 
  else {                             // if something is in front

    stopBot();                       // stop the bot
    delay(200);                      // wait

    scanServo.write(30);             // turn servo left
    delay(400);                      // wait for servo to reach position
    long left = getDistance();       // read left distance

    scanServo.write(150);            // turn servo right
    delay(400);                      // wait
    long right = getDistance();      // read right distance

    scanServo.write(90);             // center the servo
    delay(200);                      // small pause

    if (left > right)                // if left side has more space
      turnLeft();                    // turn left
    else
      turnRight();                   // otherwise turn right
  }

  delay(50);                         // small delay between loops
}
