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
int threshold = 0;
int iteration_counter = 1;

// the setup function runs once when you press reset or power the board
void setup() {
Serial.begin(9600);
for (int i=1;i<10;i++) {
  threshold = threshold + analogRead(IR_SENSOR_PIN);
  Serial.println(threshold);
  delay(50);
}
threshold = threshold/10; // Sample ambient light 100 times and average out to find a decent threshold.
Serial.println(threshold);
threshold = threshold - 100; // add some safeguard, in case 100 is too much (i.e. room is too bright)
Serial.print("Threshold set at: ");
Serial.println(threshold);
}

// the loop function runs over and over again forever
void loop() {

        analogWrite(GREEN_LED_PIN, 255);
        analogWrite(IR_LED_PIN, 255);
        Serial.println(analogRead(IR_SENSOR_PIN));
        delay(20);

}
