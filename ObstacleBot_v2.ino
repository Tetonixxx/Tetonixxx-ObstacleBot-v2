#include <AFMotor.h>        // load motor shield library so we can use the motors
#include <Servo.h>          // load servo library so we can control the servo motor

// Motors on M1 and M2 (these match the ports on the shield)
AF_DCMotor motorLeft(1);    // left motor is plugged into M1
AF_DCMotor motorRight(2);   // right motor is plugged into M2

// Servo used to rotate ultrasonic sensor
Servo scanServo;            // create a servo object

// Ultrasonic pins
#define TRIG A1             // TRIG pin of sensor connected to analog pin A1
#define ECHO A2             // ECHO pin of sensor connected to analog pin A2

// Stuck detection variables
int stuckCount = 0;         // counts how many times distance didn’t change
long lastDist = 0;          // stores previous distance reading

// ----------------------------------------------------
// Function: measure distance in front of the robot
// ----------------------------------------------------
long getDistance() {

  digitalWrite(TRIG, LOW);  // make TRIG low to reset pulse
  delayMicroseconds(2);     // wait for 2 microseconds
  digitalWrite(TRIG, HIGH); // send a short HIGH pulse to trigger the sensor
  delayMicroseconds(10);    // keep TRIG HIGH for 10 microseconds
  digitalWrite(TRIG, LOW);  // stop the pulse

  // measure how long the ECHO pin stays HIGH (sound travel time)
  long duration = pulseIn(ECHO, HIGH, 60000); // timeout 60ms so code doesn’t freeze

  // convert time into distance (speed of sound formula)
  long dist = duration * 0.034 / 2;  // distance in cm

  // if distance is invalid or too far, cap it at 400 cm
  if (dist == 0 || dist > 400)
    dist = 400;

  return dist;              // return the final distance value
}

// ----------------------------------------------------
// Motor control functions
// ----------------------------------------------------
void forward() {
  Serial.println("Action: FORWARD");  // print what the bot is doing

  motorLeft.setSpeed(180);            // set speed of left motor
  motorRight.setSpeed(180);           // set speed of right motor
  motorLeft.run(FORWARD);             // left motor moves forward
  motorRight.run(FORWARD);            // right motor moves forward
}

void stopBot() {
  Serial.println("Action: STOP");     // tell monitor bot is stopping

  motorLeft.run(RELEASE);             // stop left motor
  motorRight.run(RELEASE);            // stop right motor
}

void turnLeft() {
  Serial.println("Action: TURN LEFT");  // print action

  motorLeft.setSpeed(180);              // set both speeds
  motorRight.setSpeed(180);
  motorLeft.run(BACKWARD);              // left wheel goes backward
  motorRight.run(FORWARD);              // right wheel goes forward
  delay(400);                           // turning time
  stopBot();                            // stop after turning
  delay(150);                           // small delay before moving again
}

void turnRight() {
  Serial.println("Action: TURN RIGHT"); // print action

  motorLeft.setSpeed(180);              // set both speeds
  motorRight.setSpeed(180);
  motorLeft.run(FORWARD);               // left wheel forward
  motorRight.run(BACKWARD);             // right wheel backward
  delay(400);                           // turning time
  stopBot();                            // stop after turning
  delay(150);                           // small delay
}

// ----------------------------------------------------
// Stuck detection system
// ----------------------------------------------------
void checkStuck(long d) {              // receives current distance value

  // check if distance barely changed from last reading
  if (abs(d - lastDist) < 5) {
    stuckCount++;                      // increase stuck counter
  } else {
    stuckCount = 0;                    // reset if robot is moving properly
  }

  // if stuck too many times in a row
  if (stuckCount >= 5) {
    Serial.println("⚠ STUCK DETECTED! Reversing..."); // print warning

    motorLeft.setSpeed(180);           // set motor speed
    motorRight.setSpeed(180);
    motorLeft.run(BACKWARD);           // reverse both wheels
    motorRight.run(BACKWARD);
    delay(500);                        // reverse for 0.5 seconds

    stopBot();                         // stop movement
    delay(200);                        // short wait

    stuckCount = 0;                    // reset stuck counter
  }

  lastDist = d;                        // save current distance for next check
}

// ----------------------------------------------------
// SETUP: runs once at start
// ----------------------------------------------------
void setup() {
  Serial.begin(9600);                  // start serial monitor
  Serial.println("=== BOT STARTED ==="); // print startup message

  pinMode(TRIG, OUTPUT);               // TRIG pin is output
  pinMode(ECHO, INPUT);                // ECHO pin is input

  scanServo.attach(10);                // attach servo to pin 10
  scanServo.write(90);                 // set servo to middle position
}

// ----------------------------------------------------
// MAIN LOOP: runs forever
// ----------------------------------------------------
void loop() {

  long front = getDistance();          // get distance ahead of robot
  Serial.print("Front: ");             // print label
  Serial.print(front);                 // print distance
  Serial.print(" cm | StuckCount: ");  // print stuck counter label
  Serial.println(stuckCount);          // print stuck counter value

  checkStuck(front);                   // check if robot seems stuck

  // if nothing close in front
  if (front > 30) {
    forward();                         // drive forward
  } 
  else {                               // obstacle detected
    Serial.println("Obstacle ahead → scanning..."); // show message

    stopBot();                         // stop
    delay(200);                        // brief delay

    scanServo.write(30);               // turn sensor to left side
    delay(400);                        // wait for servo
    long left = getDistance();         // measure left distance
    Serial.print("Left: ");
    Serial.print(left);
    Serial.println(" cm");

    scanServo.write(150);              // turn sensor to right
    delay(400);                        // wait
    long right = getDistance();        // measure right distance
    Serial.print("Right: ");
    Serial.print(right);
    Serial.println(" cm");

    scanServo.write(90);               // center servo again
    delay(200);                        // wait

    // choose better direction
    if (left > right)
      turnLeft();                      // more space on left
    else
      turnRight();                     // more space on right
  }

  delay(50);                           // small delay for stability
}
