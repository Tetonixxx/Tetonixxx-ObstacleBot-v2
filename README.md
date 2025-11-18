# ObstacleBot – Ultrasonic Obstacle Avoiding Robot
This project controls a simple obstacle-avoiding robot using:
- Adafruit Motor Shield v1
- 2 DC motors (M1 and M2)
- 1 ultrasonic sensor (Trigger on A1, Echo on A2)
- 1 servo for scanning (SERVO_1 / pin 10)

## Features
- Automatically drives forward
- Detects obstacles using ultrasonic sensor
- Scans left and right using servo
- Chooses the side with more space
- Turns automatically to avoid crashes
- Fully compatible with Arduino UNO

## Wiring
### Motors
- Motor A → M1
- Motor B → M2

### Ultrasonic Sensor
- Trigger → A1
- Echo → A2
- VCC → 5V
- GND → GND

### Servo
- Signal → SERVO_1 (Pin 10 on shield)
- VCC → 5V
- GND → GND

Upload the `ObstacleBot.ino` file to your Arduino and you're ready to go.
