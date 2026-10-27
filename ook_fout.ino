#include <Servo.h>  // bibliotheek voor de servos
#include <Wire.h> // I2C communicatie
#define IRpin 5
#define IRpin2 3
// blauw van de motor is plus
Servo flipper1;  // roept een blueprint aan uit de library (vertelt wat de servo kan en welke commands hij gebruikt)
Servo flipper2;  

const int servoPin1 = 11;  // const int: deze waardes kunnen niet meer veranderen
const int servoPin2 = 6;
const int rust = 110;  // flipper in rust
const int slag = 200; //flipper maakt slag
const int slag1 = 20; 
const int rust1 = 110;
int button = 12;
int led = 13;
int button2 = 4;
int led2 = 2;
const int MPUaddr = 0x68; // replace xx with address
int ones = 0;
int tens = 0;
int previous_state = 1;
int latchPin = 7; //grijs
int clockPin = A0; //paars
int dataPin = 8; // blauw

// vcc = 5v
// gnd = ground
// scl = A5
// sca = A4

byte digits[10] = {
  0b00111111,  // 0
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111   // 9
};

String naam = "sebastiaan";
long score = 0;
String machine = "Flipperkast_van_Sebastiaan";

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(IRpin, INPUT);
  pinMode(IRpin2, INPUT);

  Wire.beginTransmission(MPUaddr); // open connection
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true); // close connection

  flipper1.attach(servoPin1);
  flipper2.attach(servoPin2);
  flipper1.write(rust);
  flipper2.write(rust);

  pinMode(button, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(button2, INPUT_PULLUP);
  pinMode(led2, OUTPUT);

  test_display();

  Serial.begin(9600);
}


int16_t acX, acY, acZ;

void loop() {

  punten(); //zorgt ervoor dat je punten kan scoren en onthoud jouw score en stuurt het door naar de display
  tilt_sensor(); // kijkt of jij af bent door de IRsensor of of jij valsspeelt door de tiltsensor
  display(); // past de display aan op basis van de functie punten
  flippers(); // zorgt dat de flippers bewogen kunnen worden door middel van led_knopjes
}


// een funcie die de puntentelling regelt
void punten() {
  // zorgt ervoor dat de sensor niet constant punten blijft geven wanneer deze onderbroken word
    if (digitalRead(IRpin) != previous_state && digitalRead(IRpin) == 0) {
    ones++;

  }
  previous_state = digitalRead(IRpin);
  //wanneer ones bij 10 komt gaan de eenheden naar 0 en gaan de tientallen een omhoog
  if (ones >= 10) {
    ones = 0;
    tens++;
  }
  if (tens >= 10) {
    tens = 0;
  }
}

void server() {
  Serial.begin(9600);
  score = (tens * 10) + ones;
  Serial.print(naam);
  Serial.print(",");
  Serial.print(String(score));
  Serial.print(",");
  Serial.println(machine);
}

void tilt_sensor() {
  Wire.beginTransmission(MPUaddr); // open connection
  Wire.write(0x3B); // start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false); // keep connection open
  Wire.requestFrom(MPUaddr, 6, true); // request 6 registers

  int acXH = Wire.read(); // register 0x3B
  int acXL = Wire.read(); // register 0x3C
  int acYH = Wire.read(); // register 0x3D
  int acYL = Wire.read(); // register 0x3E
  int acZH = Wire.read(); // register 0x3F
  int acZL = Wire.read(); // register 0x40

  acX = ((acXH << 8 | acXL));
  acY = ((acYH << 8 | acYL));
  acZ = ((acZH << 8 | acZL));

  float gX = acX / 16384.0;
  float gY = acY / 16384.0;
  float gZ = acZ / 16384.0;

  //Serial.print("X: "); Serial.print(gX, 2); Serial.print("g | ");
  //Serial.print("Y: "); Serial.print(gY, 2); Serial.print("g | ");
  //Serial.print("Z: "); Serial.print(gZ, 2); Serial.println("g");
  delay(30);

  // Tilt detectie/balletje valt tussen flippers reset punten
  if (gX <= -0.15 || gX >= 0.15 || gY <= -0.19 || gY >= 0.20 || gZ <= 0.75 || gZ >= 1.10 || digitalRead(IRpin2) == LOW) {
  //  Serial.print("X: "); Serial.print(gX, 2); Serial.print("g | ");
 // Serial.print("Y: "); Serial.print(gY, 2); Serial.print("g | ");
 // Serial.print("Z: "); Serial.print(gZ, 2); Serial.println("g");
    server();
    tens = 0;
    ones = 0;
    delay(1000);

  }
}
xx
void display() {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digits[ones]);  // 2e display
  shiftOut(dataPin, clockPin, MSBFIRST, digits[tens]);  // 1e display
  digitalWrite(latchPin, HIGH);
}

void flippers() {

  if (digitalRead(button) == HIGH) {
    digitalWrite(led, LOW);
    flipper1.write(rust);

    // als knop ingedrukt dan flipper1 slag
  } else {
    digitalWrite(led2, HIGH);
    flipper1.write(slag);
  }

  if (digitalRead(button2) == HIGH) {
    digitalWrite(led2, LOW);
    flipper2.write(rust1);

  } else {
    digitalWrite(led, HIGH);
    flipper2.write(slag1);
  }
}
// telt af van 9 naar 0 soor van start aftellen
void test_display() {
  for (int i = 9; i>0; i--){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digits[i]);  // 2e display
  shiftOut(dataPin, clockPin, MSBFIRST, digits[i]);  // 1e display
  digitalWrite(latchPin, HIGH);
  delay(1000);
  }
}