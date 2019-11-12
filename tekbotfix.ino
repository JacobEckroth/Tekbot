//connect tools to the com that bluetooth is connected to on computer

#include <SoftwareSerial.h> //Allows setup of serial connection to Bluetooth Module
SoftwareSerial Eckroth(2, 4); //RX , TX
//Sets HC-05 RX to pin D2 and HC-05 TX to pin D3
//Creates a "virtual" Serial port/UART
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
//define the data transmit/receive pins on Arduino

//LED Info
#define LEDPIN 5 //Whatever pin we end up connecting to the Arduinos should be here
#define NUMPIXELS 2 //Popular Neopixel ring size
Adafruit_NeoPixel pixels(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);
//Motor declarations


#define PWM_Pin 3
#define PWM_Speed 130 //not sure what this does
#define MOTOR1 7 //digital pin
#define MOTOR2 8 //digital pin
#define MOTOR3 9 //digital pin 
#define MOTOR4 10 //digital pin
#define RIGHTPIN 12 //rightpinup
#define LEFTPIN 6//leftpinup



bool leftbump = false;
bool rightbump = false;
bool frontbump = false;
int currentred = 0;
int currentblue = 0;
int currentgreen = 0;
int redvel = 1;
int bluevel = 1;
int greenvel = 1;
bool idle = true;

char a = ' '; //Creates a changable character


void setup() {
  pinMode(PWM_Pin, OUTPUT);
  pinMode(MOTOR1, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(MOTOR3, OUTPUT);
  pinMode(MOTOR4, OUTPUT);
  pinMode(LEFTPIN, INPUT_PULLUP);
  pinMode(RIGHTPIN, INPUT_PULLUP);

  digitalWrite(PWM_Pin, PWM_Speed);
  digitalWrite(MOTOR1, LOW);
  digitalWrite(MOTOR2, LOW);
  digitalWrite(MOTOR3, LOW);
  digitalWrite(MOTOR4, LOW);



  Serial.begin(9600);  //Tells Arduino to talk to the computer
  Eckroth.begin(9600); //Tells HC-05 to talk to the computer
  Eckroth.println("Let's a go!"); //Creates a line in the Serial Monitor (Let's a go)
  pixels.clear();
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

}

void loop() {
  //Initializes variables that read from the bumpers.
  int leftvalue = digitalRead(LEFTPIN); 
  int rightvalue = digitalRead(RIGHTPIN);
  if (Eckroth.available()) { //If input is available from the serial
    a = Eckroth.read();
    if (a == 'w') { //If w is pressed
      tekbot_forward();
      Eckroth.println("Forward");
      idle = false;
    } else if (a == 'a') { //if a is pressed
      tekbot_left();
      idle = false;
      delay(100);
      tekbot_stop();
      Eckroth.println("Left");
    } else if (a == 'd') { //if d is pressed
      tekbot_right();
      idle = false;
      delay(100);
      tekbot_stop();
      Eckroth.println("Right");
    } else if (a == 's') { //if s is pressed
      tekbot_backwards();
      idle = false;
      Eckroth.println("Backwards");
    } else if (a == ' ') { //if space bar is pressed
      tekbot_stop();
      Eckroth.println("Stopping");
      idle = true;
    } else if (a == 'p') { //this is just for fun, but this activates if p is pressed
      idle = false;
      tekbot_left();
      Eckroth.println("we donuting");
    } else { //testing input for turning on LEDs
      for (int i = 0; i < NUMPIXELS; i++) {

        if (a == '0') {
          currentred = 255;
          currentgreen = 0;
          currentblue = 0;
          pixels.setPixelColor(i, pixels.Color(currentred, currentgreen, currentblue));
          pixels.show();
          Eckroth.println("Red");

        } else if (a == '1') {
          pixels.setPixelColor(i, pixels.Color(0, 255, 0));
          currentblue = 0;
          currentgreen = 255;
          currentred = 0;
          pixels.show();
          Eckroth.println("Green");
        } else if (a == '2') {
          pixels.setPixelColor(i, pixels.Color(255, 128, 0));
          currentblue = 0;
          currentgreen = 128;
          currentred = 255;
          pixels.show();
          Eckroth.println("Orange");
        }
      }
    }
  }
  
  if (leftvalue == LOW && rightvalue == HIGH) {
    Eckroth.println("Turning around after left bumper hit");
    tekbot_turnaroundleft();
  } else if (rightvalue == LOW && leftvalue == HIGH) {
    Eckroth.println("Turning around after right bumper hit");
    tekbot_turnaroundright();
  } else if (leftvalue == LOW && rightvalue == LOW) {
    Eckroth.println("Turning around after both bumpers hit");
    tekbot_turnaroundboth();
  }
  /*This tests if the robot is currently not moving.
    If it is not moving, then it fades the RGB leds through all the possible colors for a little light show. */
  if (idle == true) {
    currentred += 2 * redvel; //These are outside of the for loop, because otherwise it'll set the individual LED's to different colors
    currentblue += 1 * bluevel;
    currentgreen += 3 * greenvel;
    for (int i = 0; i < NUMPIXELS; i++) {
      if (currentred > 255) { //Testing if any of the colors exceed the bounds as accepted by the NEO
        redvel = -1;
        currentred = 255;
      } else if (currentred < 0) {
        redvel = 1;
        currentred = 0;
      }
      if (currentgreen > 255) {
        greenvel = -1;
        currentgreen = 255;
      } else if (currentgreen < 0) {
        greenvel = 1;
        currentgreen = 0;
      }
      if (currentblue > 255) {
        bluevel = -1;
        currentblue = 255;
      } else if (currentblue < 0) {
        bluevel = 1;
        currentblue = 0;
      }

      delay(10); //If this delay isn't here, things go absolutely bonkers, but if this is removed the bot will respond slightly faster. So you decide which you want
      pixels.setPixelColor(i, pixels.Color(currentred, currentgreen, currentblue));
      pixels.show();
    }
  }
}

//All bot commands here
void tekbot_stop() {
  idle = true;
  Eckroth.println("Idle");
  currentred = 0;
  currentgreen = 0;
  currentblue = 255;
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(currentred, currentgreen, currentblue));
    pixels.show();
  }
  digitalWrite(MOTOR1, LOW);
  digitalWrite(MOTOR2, LOW);
  digitalWrite(MOTOR3, LOW);
  digitalWrite(MOTOR4, LOW);
}

void tekbot_forward() {
  currentred = 0;
  currentgreen = 255;
  currentblue = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(currentred, currentgreen, currentblue));
    pixels.show();
  }
  digitalWrite(MOTOR1, LOW);
  digitalWrite(MOTOR2, HIGH);
  digitalWrite(MOTOR3, HIGH);
  digitalWrite(MOTOR4, LOW);
}

void tekbot_backwards() { //might be backwards
  currentred = 255;
  currentgreen = 0;
  currentblue = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(currentred, currentgreen, currentblue));
    pixels.show();
  }
  digitalWrite(MOTOR1, HIGH);
  digitalWrite(MOTOR2, LOW);
  digitalWrite(MOTOR3, LOW);
  digitalWrite(MOTOR4, HIGH);
}

void tekbot_left() {
  currentred = 255;
  currentgreen = 255;
  currentblue = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(currentred, currentgreen, currentblue));
    pixels.show();
  }
  digitalWrite(MOTOR1, LOW);
  digitalWrite(MOTOR2, HIGH);
  digitalWrite(MOTOR3, LOW);
  digitalWrite(MOTOR4, HIGH);
}

void tekbot_right() {
  currentred = 255;
  currentgreen = 255;
  currentblue = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(currentred, currentgreen, currentblue));
    pixels.show();
  }
  digitalWrite(MOTOR1, HIGH);
  digitalWrite(MOTOR2, LOW);
  digitalWrite(MOTOR3, HIGH);
  digitalWrite(MOTOR4, LOW);
}

void tekbot_turnaroundleft() {
  tekbot_stop();
  delay(500);
  tekbot_backwards();
  delay(200);
  tekbot_stop();
  delay(100);
  tekbot_right();
  delay(300); //adjust accordingly
  tekbot_stop();
}

void tekbot_turnaroundright() {
  tekbot_stop();
  delay(500);
  tekbot_backwards();
  delay(200);
  tekbot_stop();
  delay(100);
  tekbot_left();
  delay(300); //adjust accordingly
  tekbot_stop();
}

void tekbot_turnaroundboth() {
  tekbot_stop();
  delay(500);
  tekbot_backwards();
  delay(200);
  tekbot_stop();
  delay(100);
  tekbot_right();
  delay(600); //adjust accordingly
  tekbot_stop();
}
