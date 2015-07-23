/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */

int IR_SENSOR_PIN = A0;
int GREEN_LED_PIN = 9; // LED connected to digital pin 9
int IR_LED_PIN = 10;

int sensor_value = 0;
double phase = 0;
double phase_next = 0;

// the setup function runs once when you press reset or power the board
void setup() {
Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {

      phase_
      analogWrite(GREEN_LED_PIN, 255);
      analogWrite(IR_LED_PIN, 255);
      delay(50);
      sensorValue = analogRead(IR_SENSOR_PIN);
      Serial.println(sensorValue);
      delay(1000);

      analogWrite(GREEN_LED_PIN, 0);
      analogWrite(IR_LED_PIN, 0);
      delay(50);
      sensorValue = analogRead(IR_SENSOR_PIN);
      Serial.println(sensorValue);
      delay(1000);

}
