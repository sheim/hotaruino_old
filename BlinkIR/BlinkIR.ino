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

const int IR_SENSOR_PIN = A0;
const int GREEN_LED_PIN = 9;
const int IR_LED_PIN    = 10;

const int pi =  314;
const int pi2 =  628;

int sensor_value = 0;
int phase = 15;
int phase_next = 0;
int omega = pi/100;
int stim = 0;

// the setup function runs once when you press reset or power the board
void setup() {
Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {

//      Serial.println(phase);
      sensor_value = analogRead(IR_SENSOR_PIN);
      if(sensor_value<700) {
        stim = 1;
      }
      else {
        stim = 0;
      }
      phase_next = phase_next + omega - stim*sin(phase_next);
      if ((phase_next % pi2) < 5) {
        analogWrite(GREEN_LED_PIN, 255);
        analogWrite(IR_LED_PIN, 255);
      }
      else {
        analogWrite(GREEN_LED_PIN, 0);
        analogWrite(IR_LED_PIN, 0);
      }

      if(sensor_value<700) {
      Serial.println(sensor_value);
      }
      delay(10);

}
