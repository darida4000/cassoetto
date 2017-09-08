

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

  Homing1=0;
  Homing2=0;
  delay(1000);

  digitalWrite(slee1Pin, HIGH);
  digitalWrite(slee2Pin, HIGH);
  digitalWrite(enbPin, LOW);
  delay(500);
  digitalWrite(dire1Pin, HIGH);
  digitalWrite(dire2Pin, LOW);
  delay(500);
 
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
  WeightScaleInit();
}
void PlateToCam()
{
  LedCamLightON();
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
  LedCamLightOFF();
  // a seconda del cestino:
  switch(can)
  {
    case 0:
        LcdBackLightON();
        LcdMsgOk(8);
        delay(1500);
        LcdBackLightOFF();

      digitalWrite(slee1Pin, HIGH);
      digitalWrite(slee2Pin, HIGH);
      digitalWrite(enbPin, LOW);
      delay(200);
      digitalWrite(dire1Pin, HIGH); // disco sotto
      digitalWrite(dire2Pin, LOW);
      delay(200);

      for (int n = 0; n < numberOfSteps; n++) {
        digitalWrite(step2Pin, HIGH);
        if(n<numberOfSteps/2) digitalWrite(step1Pin, HIGH);
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
        digitalWrite(step2Pin, LOW);
        if(n<numberOfSteps/2) digitalWrite(step1Pin, LOW);
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
      }
       
      digitalWrite(slee2Pin, LOW);
      digitalWrite(slee1Pin, LOW);


      break;

    case 1:
            LcdBackLightON();
        LcdMsgOk(9);
        delay(1500);
        LcdBackLightOFF();

      digitalWrite(slee1Pin, HIGH);
      digitalWrite(slee2Pin, HIGH);
      digitalWrite(enbPin, LOW);
      delay(200);
      digitalWrite(dire1Pin, HIGH); // disco sotto
      digitalWrite(dire2Pin, LOW);
      delay(200);

      for (int n = 0; n < 2950; n++) {
        digitalWrite(step2Pin, HIGH);
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
        digitalWrite(step2Pin, LOW);
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
      }
       
      digitalWrite(slee2Pin, LOW);
      digitalWrite(slee1Pin, LOW);


      break;


    
    case 2:
         LcdBackLightON();
        LcdMsgOk(10);
        delay(1500);
        LcdBackLightOFF();

 
      digitalWrite(slee1Pin, HIGH);
      digitalWrite(slee2Pin, HIGH);
      digitalWrite(enbPin, LOW);
      delay(200);
      digitalWrite(dire1Pin, LOW); // disco sotto
      digitalWrite(dire2Pin, HIGH);
      delay(200);

      for (int n = 0; n < numberOfSteps/2; n++) {
        digitalWrite(step2Pin, HIGH);
        digitalWrite(step1Pin, HIGH);
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
        digitalWrite(step2Pin, LOW);
        digitalWrite(step1Pin, LOW);
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
      }

      delay(200);
      digitalWrite(dire1Pin, HIGH); // disco sotto
      digitalWrite(dire2Pin, LOW);
      delay(200);

      for (int n = 0; n < numberOfSteps/2; n++) {
        digitalWrite(step2Pin, HIGH);
        
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
        digitalWrite(step2Pin, LOW);
        
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
      }

      digitalWrite(slee2Pin, LOW);
      digitalWrite(slee1Pin, LOW);


      break;

    
    case 3:
        LcdBackLightON();
        LcdMsgOk(11);
        delay(1500);
        LcdBackLightOFF();

    
      digitalWrite(slee1Pin, HIGH);
      digitalWrite(slee2Pin, HIGH);
      digitalWrite(enbPin, LOW);
      delay(200);
      digitalWrite(dire1Pin, LOW); // disco sotto
      digitalWrite(dire2Pin, HIGH);
      delay(200);

      for (int n = 0; n < numberOfSteps; n++) {
       if(n<numberOfSteps/2) digitalWrite(step2Pin, HIGH);
        digitalWrite(step1Pin, HIGH);
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
        if(n<numberOfSteps/2)digitalWrite(step2Pin, LOW);
        digitalWrite(step1Pin, LOW);
        delayMicroseconds(pulseWidthMicros); // this line is probably unnecessary
      }

     

      digitalWrite(slee2Pin, LOW);
      digitalWrite(slee1Pin, LOW);


      break;

    default:
    break;


    
  }



  
}

