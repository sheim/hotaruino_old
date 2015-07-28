/*

*/

const int IR_SENSOR_PIN = A0;
const int GREEN_LED_PIN = 10;
const int IR_LED_PIN    = 9;
const int RECEIVER_LED_PIN = 13;

// NOTE: Arduino UNO (and most others) treat doubles as floats.
const double PI_HALF = 1.5; // chopped off just short of accurate, to avoid reaching a point where function is no longer concave-down.

bool flash_received = false;

double sensor_value = 0;
double x = 0; // state "x", with x = f(phase), and belonging to [0;f(PI_HALF)], with resetting at f(PI_HALF).
double X_RESET = 0.997; // chopped off just short of f(PI_HALF)
double phase = 0;
double omega = 0.01; // need to tune this, depends on loop speed
double eps   = 0.05;
double A = 1.2; //excitation level
double threshold = 0;
int iteration_counter = 1;

// Timing
unsigned long previous_millis = 0;
unsigned long TIME_STEP = 10;
// Flash timing
long flash_start = 0;
long flash_time = 200;


// the setup function runs once when you press reset or power the board
void setup() {
  delay(10); // wait for things to start up
  Serial.begin(9600);
  
  pinMode(RECEIVER_LED_PIN, OUTPUT);

  analogWrite(IR_LED_PIN, 0); // make sure things are off
  delay(10);
  for (int i = 1; i < 10; i++) { // get threshold value
    threshold = threshold + analogRead(IR_SENSOR_PIN);
    delay(50);
  }
  threshold = threshold / 10; // Sample ambient light 100 times and average out to find a decent threshold.
  threshold = threshold - 50; // TODO: add some safeguard, in case 50 is too much (i.e. room is too bright)
  Serial.print("Threshold set at: ");
  Serial.println(threshold);
}

// the loop function runs over and over again forever
void loop() {

  // Check timing
  unsigned long current_millis = millis();
  // Toggle flash
  if (current_millis < (flash_start + flash_time)) { // flash
    analogWrite(GREEN_LED_PIN, 255);
    analogWrite(IR_LED_PIN, 255);
  }
  else {
    analogWrite(GREEN_LED_PIN, 0); // turn off
    analogWrite(IR_LED_PIN, 0);
  }

  // Check if sensors catches data
  sensor_value = analogRead(IR_SENSOR_PIN);
  if (sensor_value < threshold) {
    flash_received = true;       // NOTE: could make this incrementable, so it the number of flashes seen is also important.
    digitalWrite(RECEIVER_LED_PIN, HIGH);
  }
  else {
    digitalWrite(RECEIVER_LED_PIN, LOW);
  }
  // increment dynamics
  if (current_millis - previous_millis >= TIME_STEP) { // new timestep
    previous_millis = current_millis; // reset time
    if (flash_received) { // switch between active dynamics and passive
      phase = phase + omega + A * eps; // Increment phase by eps.
    }
    else {
      phase = phase + omega;
    }
    // check if firing, if yes, reset.
    x = sin(phase);
    if (x > X_RESET) {
      x = 0;
      phase = 0;
      flash_start = current_millis;
    }
    flash_received = false; // reset boolean for next time-step
  }

  // just some visiblity stuff
  iteration_counter = iteration_counter + 1;
  if (iteration_counter % 12 == 0) {
    Serial.println(sensor_value);
  }
}
