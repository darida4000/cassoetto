 /////////////////////////////////////////////////////////////////////

  // RICICLO PROGETTO DIFFERENZIATA
  //
  // FUNZIONI APPOGGIO

  /////////////////////////////////////////////////////////////////////


 /////////////////////////////////////////////////////////////////////

  // BILANCIA

 /////////////////////////////////////////////////////////////////////


// bilancia pesa
void WeightScaleInit(){
    scale.set_scale();
  scale.tare();  //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  pesoiniziale = scale.get_units(), 10;
}
float WeightScaleGet(){
  
}
float WeightScaleGetZero(){

  return pesoiniziale;
}

/////////////////////////////////////////////////////////////////////

  // DISCHI

 /////////////////////////////////////////////////////////////////////


void PlateInit(){
  pinMode(OptoLimit1, INPUT_PULLUP);
  pinMode(OptoLimit2, INPUT_PULLUP);
    pinMode(dire1Pin, OUTPUT);
  pinMode(dire2Pin, OUTPUT);
  pinMode(step1Pin, OUTPUT);
  pinMode(step2Pin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(enbPin, OUTPUT);
  pinMode(slee1Pin, OUTPUT);
  pinMode(slee2Pin, OUTPUT);
  //Motori liberi 
  digitalWrite(slee2Pin, LOW);
  digitalWrite(slee1Pin, LOW);

}
void PlateHomingF()
{


  delay(1000);

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
  ArmHoming();
}
void PlateToCam()
{
  digitalWrite(slee1Pin, HIGH);
  digitalWrite(slee2Pin, HIGH);
  digitalWrite(enbPin, LOW);
  delay(200);
  digitalWrite(dire1Pin, LOW); // disco sotto
  digitalWrite(dire2Pin, HIGH);
  delay(200);

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
void PlateToRecycle(int can)
{

  // devo muovere i piatti di modo da far cadere  il rifiuto nel contenitore 'can'



  
}
/////////////////////////////////////////////////////////////////////

  // BRACCETTO INTERNO

 /////////////////////////////////////////////////////////////////////

void ArmHoming()
{
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
    delay(500);  
}   
// Braccio interno funzioni
void servoVaiA(int pos)
{
  for (int i = 90; i < pos; i += 5)
  {
    braccioSpost.write(i);
    delay(50);
  }
}
void ArmStroke()
{
  
  // inizializzazione servo Braccio
  braccioSpost.attach(pinBraccio);
  
  for (int i = 90; i < 160; i += 5)
  {
    braccioSpost.write(i);
    delay(50);
  }
  
  braccioSpost.detach();
  
  delay(500);  
  
}
/////////////////////////////////////////////////////////////////////

  // PORTA

 /////////////////////////////////////////////////////////////////////


//Porta Funzioni 
void DoorInit(){
    pinMode(pinSporta, INPUT_PULLUP);
      DoorLock();
    delay(1000);
      DoorUnlock();
    delay(1000);
}

void DoorLock() {
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
void DoorUnlock() {
  // inizializzazione servo Blocco
  bloccoPorta.attach(pinBlocco);
  for (int pos = 60; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    bloccoPorta.write(pos);                // tell servo to go to position in variable 'pos'
    delay(15);                             // waits 15ms for the servo to reach the position
  }
  delay(700);
  bloccoPorta.detach();
}

bool DoorGetStat() {
  //#define pinSporta 42
  return (digitalRead(pinSporta) != 0);
}


/////////////////////////////////////////////////////////////////////

  // LCD

 /////////////////////////////////////////////////////////////////////

void LcdInit(){
  lcd.begin(16, 2);
  pinMode(Dled, OUTPUT);
  lcd.clear();
  digitalWrite(Dled, LOW);
}
void LcdBackLightON(){
    digitalWrite(Dled, HIGH);
}
void LcdBackLightOFF(){
    digitalWrite(Dled, LOW);
}
void LcdMsg(byte Row0, byte Row1){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(MsgLcd[0][Row0]);
    lcd.setCursor(0, 1);
    lcd.print(MsgLcd[1][Row1]);
  
}


/////////////////////////////////////////////////////////////////////

  // SENSORI

 /////////////////////////////////////////////////////////////////////


bool GetMetal(double M[]) //COMPLETARE
{

      M[0]= analogRead(analogInPin1);
      M[1] = analogRead(analogInPin2);
      
      return true;
      

}


bool GetUV(double U[]) //COMPLETARE
{
    U[0] = 1;
    U[1] = 1;
    U[2] = 1;
    return true;

}


double GetWeight() //COMPLETARE
{
    scale.set_scale(calibration_factor); //Adjust to this calibration factor
  
    int attesa = 0;
    do {
      units = 12.898;//scale.get_units(), 10; // peso
      delay(1000);
      attesa++; // aspetto max un minuto
    } while ((abs(pesoiniziale - units) < 0.5) && (attesa < 60));

    if (pesoiniziale == units) return -1; 
    return units;
}

