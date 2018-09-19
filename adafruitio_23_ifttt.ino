// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

// Import Servo Libraries
#if defined(ARDUINO_ARCH_ESP32)
  // ESP32Servo Library (https://github.com/madhephaestus/ESP32Servo)
  // installation: library manager -> search -> "ESP32Servo"
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif

/************************ Example Starts Here *******************************/

// pin used to control the servo
#define SERVO_PIN 14

// Flag's up position, in degrees
#define FLAG_UP 105


// Flag's down position, in degrees
#define FLAG_DOWN 15

// digital pin 5
#define BUTTON_PIN 5

#define AIO_DATA_LENGTH 45

// last received email date string
char _oldVal[AIO_DATA_LENGTH];

bool _raised = false;

// create an instance of the servo class
Servo servo;

// set up the 'servo' feed
AdafruitIO_Feed *gmail_feed = io.feed("gmail");

void setup() {

  // set button pin as an input
  pinMode(BUTTON_PIN, INPUT);
  
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("IFTTT Gmailbox");

  // tell the servo class which pin we are using
  servo.attach(SERVO_PIN);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'servo' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  gmail_feed->onMessage(handleMessage);
 
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  gmail_feed->get();

  // write flag to down position
  servo.write(FLAG_DOWN);

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  // aknowledge
  if(_raised && digitalRead(BUTTON_PIN) != LOW) {
    Serial.print("aknowledging");
    servo.write(FLAG_DOWN);
    _raised = false;
  }
}

// this function is called whenever a 'gmail' message
// is received from Adafruit IO. it was attached to
// the gmail feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {
  char *newVal = data->value();
  if(_oldVal) {
    Serial.println("new value");
    Serial.println(newVal);
    Serial.println("old value");
    Serial.println(_oldVal);
    if(strcmp(newVal, _oldVal) == 0) {
      Serial.println("They are equal");
      return;
    }
    Serial.println("They are NOT equal");
  }
  memset(_oldVal, 0, AIO_DATA_LENGTH);
  strcpy(_oldVal, newVal);
  Serial.println("You've got mail!------------------------------------");
  Serial.println(data->value());
  Serial.println("================------------------------------------");
  servo.write(FLAG_UP);
  _raised = true;
}
