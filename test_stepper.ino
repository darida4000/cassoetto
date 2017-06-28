// testing a stepper motor with a Pololu A4988 driver board or equivalent
// on an Uno the onboard led will flash with each step
// this version uses delay() to manage timing

byte ledPin = 13;
byte enbPin = 8;
byte dire2Pin = 7;
byte dire1Pin = 6;
byte slee2Pin = 5;
byte slee1Pin = 4;
byte step2Pin = 3;
byte step1Pin = 2;
byte OptoLimit1=10;
byte OptoLimit2=11;
byte Homing1=0;
byte Homing2=0;

int numberOfSteps = 1000;

int pulseWidthMicros = 450;  // microseconds
int millisbetweenSteps = 1; // milliseconds - or try 1000 for slower steps


void setup() {

  Serial.begin(9600);
  Serial.println("Starting StepperTest");
  digitalWrite(ledPin, LOW);

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
  delay(1000);
  digitalWrite(dire1Pin, LOW);
  digitalWrite(dire2Pin, LOW);
  delay(1000);
  for (int n = 0; n < numberOfSteps; n++) {

   digitalWrite(step1Pin, HIGH);
   digitalWrite(step2Pin, HIGH);
    //delay(millisbetweenSteps);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
  digitalWrite(step1Pin, LOW);
  digitalWrite(step2Pin, LOW);
    delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary

    // delay(millisbetweenSteps);
  }
    digitalWrite(slee2Pin, LOW);
    digitalWrite(slee1Pin, LOW);
    digitalWrite(ledPin, !digitalRead(ledPin));
  
  
}
void loop()
{
delay(10000);
}
  


