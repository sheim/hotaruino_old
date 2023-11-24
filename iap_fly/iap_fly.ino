
#define DECODE_NEC
// #define DEBUG               // Activate this for lots of lovely debug output from the decoders.

#include <Arduino.h>

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.
#include <IRremote.h>

// sender constants
uint16_t sAddress = 0x0102;
uint8_t sCommand = 0x34;
uint8_t sRepeats = 1;

// const int IR_SENSOR_PIN = 10;
// const int IR_LED_PIN    = 11;
#define IR_RECEIVE_PIN 10 // To be compatible with interrupt example, pin 2 is chosen here.
#define IR_SEND_PIN 11
const int GREEN_LED_PIN = 8;
const int RECEIVER_LED_PIN = 13;

// Coupling
const int POT_COUPL_PIN = A0; // black
// double POT_COUPL_LOW = 0;
// double POT_COUPL_HIGH = 0;
const double COUPL_UPPER = 4;
const double COUPL_LOWER = 0;

// Base frequency
const int POT_FREQ_PIN = A1;
// double POT_FREQ_LOW = 0;
// double POT_FREQ_HIGH = 0;
const double FREQ_UPPER = 3;
const double FREQ_LOWER = 0.5;

// NOTE: Arduino UNO (and most others) treat doubles as floats.
const double PI_HALF = 1.5; // chopped off just short of accurate, to avoid reaching a point where function is no longer concave-down.

bool flash_received = false;

double sensor_value = 0;
double x = 0;           // state "x", with x = f(phase), and belonging to [0;f(PI_HALF)], with resetting at f(PI_HALF).
double X_RESET = 0.997; // chopped off just short of f(PI_HALF)
double frequency = 0.5;
double phase = 0;
double omega = 0.01; // need to tune this, depends on loop speed
double coupling = 1.2; // excitation level
double noise = 0;

int iteration_counter = 1;

// Timing
unsigned long previous_millis = 0;
unsigned long TIME_STEP = 10; // in milliseconds
double DELTA_T = double(TIME_STEP) / 1000;

// Flash timing
long flash_start = 0;
long flash_time = 100;

double pot_reading = 0.;

// the setup function runs once when you press reset or power the board
void setup()
{
    delay(10); // wait for things to start up
    Serial.begin(115200);

    // start receiver
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));

    // start sender
    IrSender.begin();                     // Start with IR_SEND_PIN as send pin and if NO_LED_FEEDBACK_CODE is NOT defined, enable feedback LED at default feedback LED pin
    IrSender.begin(DISABLE_LED_FEEDBACK); // Start with IR_SEND_PIN as send pin and disable feedback LED at default feedback LED pin

    Serial.print(F("Send IR signals at pin "));
    Serial.println(IR_SEND_PIN);

    pinMode(RECEIVER_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);

    delay(10);

    // * omega is set
    omega = (frequency); // phase/T, T = 1/f

    // * Coupling set up

    // Serial.println("Turn coupling potentiometer (black) to min.");
    // digitalWrite(GREEN_LED_PIN, HIGH);
    // delay(3000);
    // POT_COUPL_LOW = analogRead(POT_COUPL_PIN);
    // Serial.print("Set coupling potentiometer to min: ");
    // Serial.println(POT_COUPL_LOW);

    // Serial.println("Turn coupling potentiometer to max.");
    // digitalWrite(GREEN_LED_PIN, LOW);
    // delay(3000);
    // POT_COUPL_HIGH = analogRead(POT_COUPL_PIN);
    // Serial.print("Set coupling potentiometer to max: ");
    // Serial.println(POT_COUPL_LOW);
    // trippleBlink(GREEN_LED_PIN);

    // * frequency set up

    // Serial.println("Turn frequency potentiometer to min.");
    // digitalWrite(GREEN_LED_PIN, HIGH);
    // delay(3000);
    // POT_FREQ_LOW = analogRead(POT_FREQ_PIN);
    // Serial.print("Set frequency potentiometer to min: ");
    // Serial.println(POT_FREQ_LOW);

    // Serial.println("Turn frequency potentiometer to max.");
    // digitalWrite(GREEN_LED_PIN, LOW);
    // delay(3000);
    // POT_FREQ_HIGH = analogRead(POT_FREQ_PIN);
    // Serial.print("Set frequency potentiometer to max: ");
    // Serial.println(POT_FREQ_HIGH);
    // trippleBlink(GREEN_LED_PIN);

    // * Finish set up
    Serial.println(" Set up finished.");
    delay(2000); // delay a second, so start-up values can be read from the console
}

// the loop function runs over and over again forever
void loop()
{
    pot_reading = analogRead(POT_FREQ_PIN);
    frequency = mapDouble(pot_reading, 0, 1023, FREQ_LOWER, FREQ_UPPER);
    omega = PI_HALF * frequency;

    // Check timing
    unsigned long current_millis = millis();
    // Toggle flash
    if (current_millis < (flash_start + flash_time))
    { // flash
        digitalWrite(GREEN_LED_PIN, HIGH);
        // analogWrite(IR_LED_PIN, 255);
    }
    else
    {
        digitalWrite(GREEN_LED_PIN, LOW); // turn off
                                          // analogWrite(IR_LED_PIN, 0);
    }
    // Check if sensor catches data
    if (IrReceiver.decode())
    {
        if (IrReceiver.decodedIRData.command == sCommand) {
            // do something
            flash_received = true;
            digitalWrite(RECEIVER_LED_PIN, HIGH);
            Serial.println("==== Received Flash. ====");
        } else {
            // do something else
            digitalWrite(RECEIVER_LED_PIN, LOW);
        }
      // Serial.println(IrReceiver.decodedIRData.protocol, HEX);
    }

    // increment dynamics

    // Check coupling weight here, so that each time-step delay is roughly the same
    pot_reading = analogRead(POT_COUPL_PIN);
    coupling = mapDouble(pot_reading, 0, 1023, COUPL_LOWER, COUPL_UPPER);

    noise = mapDouble(double(random(0, 500)), 0, 500, -1, 1);
    if (current_millis - previous_millis >= TIME_STEP)
    { // new timestep

        previous_millis = current_millis; // reset time

        if (flash_received)
        { // switch between active dynamics and passive
            if (phase < X_RESET / 2)
            {
                phase = phase + (omega + coupling + noise) * DELTA_T;
            }
            else
            {
                phase = phase + (omega + coupling * 2 + noise) * DELTA_T;
            }
        }
        else
        {
            phase = phase + (omega + noise) * DELTA_T;
        }
        // check if firing, if yes, reset.
        x = phase * phase;
        if (x > X_RESET)
        {
            // * flash
            IrSender.sendNEC(sAddress, sCommand, sRepeats);
            // send_ir_data();
            Serial.println("Sent Flash.");
            // * reset vars
            x = 0;
            phase = 0;
            flash_start = current_millis;
        }
        flash_received = false; // reset boolean for next time-step
    }

    // just some visiblity stuff
    iteration_counter = iteration_counter + 1;
    // if (iteration_counter % 503 == 0)
    // {
    //     Serial.println(phase);
    //     Serial.print("Frequency is: ");
    //     Serial.println(frequency);
    //     Serial.print(F("Coupling at: "));
    //     Serial.println(coupling);
    // }
    IrReceiver.resume();
}

// uint16_t sAddress = 0x0102;
// uint8_t sCommand = 0x34;
// uint8_t sRepeats = 1;

/*
 * Send NEC IR protocol
 */
void send_ir_data() {
    Serial.print(F("Sending: 0x"));
    Serial.print(sAddress, HEX);
    Serial.print(sCommand, HEX);
    Serial.println(sRepeats, HEX);

    // clip repeats at 4
    if (sRepeats > 4) {
        sRepeats = 4;
    }
    // Results for the first loop to: Protocol=NEC Address=0x102 Command=0x34 Raw-Data=0xCB340102 (32 bits)
    IrSender.sendNEC(sAddress, sCommand, sRepeats);
}

// void receive_ir_data() {
//     if (IrReceiver.decode()) {
//         Serial.print(F("Decoded protocol: "));
//         Serial.print(getProtocolString(IrReceiver.decodedIRData.protocol));
//         Serial.print(F("Decoded raw data: "));
// #if (__INT_WIDTH__ < 32)
//         Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
// #else
//         PrintULL::print(&Serial, IrReceiver.decodedIRData.decodedRawData, HEX);
// #endif
//         Serial.print(F(", decoded address: "));
//         Serial.print(IrReceiver.decodedIRData.address, HEX);
//         Serial.print(F(", decoded command: "));
//         Serial.println(IrReceiver.decodedIRData.command, HEX);
//         IrReceiver.resume();
//     }
// }

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
    double temp = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    // temp = (int) (4*temp + .5);
    return temp;
}

void trippleBlink(int LED_PIN)
{
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
}
