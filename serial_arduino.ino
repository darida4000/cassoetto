// HX711 define
#include "HX711.h"

#define DOUT 23
#define CLK  22
HX711 scale(DOUT, CLK);
float calibration_factor = 2125; //-7050 worked for my 440lb max scale setup
float units;
float ounces;
float pesoiniziale;
int outputValue = 0;        // value output to the PWM (analog out)

//Metal detector define
const int analogInPin1 = A0;  // Analog input pin that the potentiometer is attached to
const int analogInPin2 = A1;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 13; // Analog output pin that the LED is attached to
int sensorMetalValue1 = 0;        // value read from the pot
int sensorMetalValue2 = 0;       // value read from the pot

// Stepper
byte ledPin = 13;
byte enbPin = 25;//8;
byte dire1Pin = 27;//6;
byte step1Pin = 29;//2;
byte slee1Pin = 31;//4;
byte dire2Pin = 33;//7;
byte step2Pin = 35;//3;
byte slee2Pin = 37;//5;


#define OptoLimit1 26//10;
#define OptoLimit2 24//11;
byte Homing1 = 0; // rimettere a 0
byte Homing2 = 0;

int numberOfSteps = 6100; // mezzo giro 6300

int pulseWidthMicros = 450;  // microseconds
int millisbetweenSteps = 1; // milliseconds - or try 1000 for slower steps

// Servo

#include <Servo.h>
Servo braccioSpost;//myservo;  // oggetto servo
Servo bloccoPorta;
#define pinBlocco 7  // analog pin used to connect the potentiometer
#define pinBraccio 6
#define pinSporta 42
int val;    // variable to read the value from the analog pin

// LCD display
#include <LiquidCrystal.h>

#define Drs   40
#define Dena  38
#define Dd4   36
#define Dd5   34
#define Dd6   32
#define Dd7   30
#define Dled  28

LiquidCrystal lcd(Drs, Dena, Dd4, Dd5, Dd6, Dd7);

// PROTOTIPI

void homing();
void muoviPosizioneCam();
void BlPorta();
void SbPorta();
bool GetSporta();
void servoVaiA(int pos)
{

  for (int i = 90; i < pos; i += 5)
  {
    braccioSpost.write(i);
    delay(50);
  }

}

void setup() {
  pinMode(Dled, OUTPUT);
  pinMode(OptoLimit1, INPUT_PULLUP);
  pinMode(OptoLimit2, INPUT_PULLUP);
  pinMode(pinSporta, INPUT_PULLUP);
  Serial.begin(57600); // opens serial port, sets data rate to 57600 baud
  lcd.begin(16, 2);
  Serial.println("ar");

  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("hello, world!");
  lcd.setCursor(0, 1);
  lcd.print("Cassonetto intelligente");
  digitalWrite(Dled, HIGH);
  // inizializzazione stepper e homing + inizializza servo
  homing();

  // HX711
  scale.set_scale();
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  pesoiniziale = scale.get_units(), 10;
  //pesoiniziale = 10;
  //Serial.println(pesoiniziale);

  pinMode(LED_BUILTIN, OUTPUT);


}

void loop() {






  while (true) {
    ///// version

    //

    while (Serial.available() <= 0) {

      delay(1000);
    }
    while (Serial.available() > 0) {
      char incomingByte = Serial.read();  // consumo
    }

    // arrivato qualcosa,

    // DA IMPLEMENTARE: SBLOCCO SPORTELLO

    // procedo ad aspettare che il peso sia diverso da quello iniziale

    scale.set_scale(calibration_factor); //Adjust to this calibration factor

    // CICLO FINCHE' non arriva qualcosa. A u certo punto dovrò, se non arriva nulla, fermarmi
    int attesa = 0;
    do {
      units = scale.get_units(), 10; // peso
      delay(1000);
      attesa++; // aspetto max un minuto
    } while ((abs(pesoiniziale - units) < 0.5) && (attesa < 60));

    if (pesoiniziale == units) continue; // se non è arrivato nulla e è passato un minuto

    // DA IMPLEMENTARE: BLOCCO SPORTELLO

    muoviPosizioneCam(); // muovo in posizione per camera e altri sensori

    String r = "pe";
    r = units +  r;
    Serial.println(r); // restituisce al raspy il peso

    // ATTENDO SULLA SERIALE
    while (Serial.available() <= 0) {
      delay(1000);
    }
    while (Serial.available() > 0) {
      char incomingByte = Serial.read();  // consumo
    }

    // DOPO CHE RASPY HA COMUNICATO DI AVER FATTO LA FOTO
    // MUOVO SERVO BRACCIO

    servoVaiA(160);

    // SESORE METALLI E UV

    sensorMetalValue1 = analogRead(analogInPin1);
    sensorMetalValue2 = analogRead(analogInPin2);
    int totalmetal = sensorMetalValue2 + sensorMetalValue1;
    String res = "";
    res = res + totalmetal + "|" + 21;

    // COMUNICO VALORI AL RASP
    Serial.println(res);

    // servo in posizione iniziale

    int pos = 150;
    while (pos > 90)
    {
      braccioSpost.write(pos);                  // valori tra 0 e 180
      delay(150);
      pos -= 5;
    }

    // RIMANGO IN ATTESA DEL RESPONSO DEL RASPBERRY
    while (Serial.available() <= 0) {
      delay(1000);
    }
    while (Serial.available() > 0) {
      char incomingByte = Serial.read();  // consumo
    }

    // DA IMPLEMETARE:  A SECONDA DEL CARATTERE RICEVUTO MI MUOVO E GETTO RIFIUTO

    // DA IMPLEMETARE: FACCIO HOMING
    // COMUICO AL RASPY DI AVER FINITO
    Serial.println("T");


    delay(1000);
  }
}


void muoviPosizioneCam()
{


  digitalWrite(slee1Pin, HIGH);
  digitalWrite(slee2Pin, HIGH);
  digitalWrite(enbPin, LOW);
  delay(1000);


  digitalWrite(dire1Pin, LOW); // disco sotto
  digitalWrite(dire2Pin, HIGH);
  delay(1000);

  for (int n = 0; n < numberOfSteps; n++) {

    digitalWrite(step1Pin, HIGH);
    digitalWrite(step2Pin, HIGH);
    //delay(millisbetweenSteps);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    digitalWrite(step1Pin, LOW);
    digitalWrite(step2Pin, LOW);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary


  }
  digitalWrite(slee2Pin, LOW);
  digitalWrite(slee1Pin, LOW);

}


void homing()
{


  delay(1000);

  pinMode(dire1Pin, OUTPUT);
  pinMode(dire2Pin, OUTPUT);
  pinMode(step1Pin, OUTPUT);
  pinMode(step2Pin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(enbPin, OUTPUT);
  pinMode(slee1Pin, OUTPUT);
  pinMode(slee2Pin, OUTPUT);
  pinMode(OptoLimit1, INPUT);
  pinMode(OptoLimit2, INPUT);

  digitalWrite(slee1Pin, HIGH);
  digitalWrite(slee2Pin, HIGH);
  digitalWrite(enbPin, LOW);
  delay(500);
  digitalWrite(dire1Pin, HIGH);
  digitalWrite(dire2Pin, LOW);
  delay(500);
  //for (int n = 0; n < numberOfSteps; n++) {
  while (Homing1 != 3 || Homing2 != 3) {
    if (Homing1 == 0)digitalWrite(step1Pin, HIGH);
    if (Homing2 == 0) digitalWrite(step2Pin, HIGH);

    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    if (Homing1 == 0)digitalWrite(step1Pin, LOW);
    if (Homing2 == 0)digitalWrite(step2Pin, LOW);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
    if (digitalRead(OptoLimit1) == 0)Homing1 = 3;
    if (digitalRead(OptoLimit2) == 0)Homing2 = 3;

  }
  delay(1000);
  digitalWrite(slee2Pin, LOW);
  digitalWrite(slee1Pin, LOW);

  // inizializzazione servo Braccio
  braccioSpost.attach(pinBraccio);
  int pos = 150;
  while (pos > 90)
  {
    braccioSpost.write(pos);                  // valori tra 0 e 180
    delay(150);
    pos -= 5;
  }
  braccioSpost.detach();
  BlPorta();
  delay(1000);
  SbPorta();
  delay(1000);

  for (int cont = 0; cont <= 20; cont += 1) { //lettura stato dello switch porta per 10 sec
    lcd.setCursor(0, 1);
    if (GetSporta()) {
      lcd.print("SwitchPort open       ");
    } else {
      lcd.print("SwitchPort close      ");
    }

    delay(500);
  }

}
void BlPorta() {
  lcd.setCursor(0, 1);
  lcd.print("Porta Chiusa       ");
  // inizializzazione servo Blocco
  bloccoPorta.attach(pinBlocco);

  for (int pos = 0; pos <= 60; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    bloccoPorta.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                            // waits 15ms for the servo to reach the position
  }

  delay(700);
  bloccoPorta.detach();
}
void SbPorta() {
  lcd.setCursor(0, 1);
  lcd.print("Porta Aperta       ");
  // inizializzazione servo Blocco
  bloccoPorta.attach(pinBlocco);
  for (int pos = 60; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    bloccoPorta.write(pos);                // tell servo to go to position in variable 'pos'
    delay(15);                             // waits 15ms for the servo to reach the position
  }
  delay(700);
  bloccoPorta.detach();
}
bool GetSporta() {
  //#define pinSporta 42
  return (digitalRead(pinSporta) != 0);
}
