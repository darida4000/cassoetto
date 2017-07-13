
void IRUVLCInit() {

  pinMode(UV400Led, OUTPUT);
  pinMode(IR940Led, OUTPUT);
  pinMode(IR890Led, OUTPUT);
  pinMode(IR850Led, OUTPUT);
  pinMode(IR830Led, OUTPUT);
  pinMode(CamLed, OUTPUT);
  pinMode(IRLedSens, OUTPUT);


  digitalWrite(UV400Led, LOW);
  digitalWrite(IR940Led, LOW);
  digitalWrite(IR890Led, LOW);
  digitalWrite(IR850Led, LOW);
  digitalWrite(IR830Led, LOW);
  digitalWrite(CamLed, LOW);
  digitalWrite(IRLedSens, LOW);


  digitalWrite(UV400Led, HIGH);
  delay(500);
  digitalWrite(UV400Led, LOW);
  digitalWrite(IR940Led, HIGH);
  delay(500);
  digitalWrite(IR940Led, LOW);
  digitalWrite(IR890Led, HIGH);
  delay(500);
  digitalWrite(IR890Led, LOW);
  digitalWrite(IR850Led, HIGH);
  delay(500);
  digitalWrite(IR850Led, LOW);
  digitalWrite(IR830Led, HIGH);
  delay(500);
  digitalWrite(IR830Led, LOW);
  digitalWrite(CamLed, HIGH);
  delay(500);
  digitalWrite(CamLed, LOW);
  digitalWrite(IRLedSens, HIGH);
  delay(500);
  digitalWrite(IRLedSens, LOW);

}

void UV400LightON(){  digitalWrite(UV400Led, HIGH);}
void UV400LightOFF(){  digitalWrite(UV400Led, LOW);}
void IR940LightON(){  digitalWrite(IR940Led, HIGH);}
void IR940LightOFF(){  digitalWrite(IR890Led, LOW);}
void IR890LightON(){digitalWrite(IR890Led, HIGH);}
void IR890LightOFF(){  digitalWrite(IR890Led, LOW);}
void IR850LightON(){  digitalWrite(IR850Led, HIGH);}
void IR850LightOFF(){  digitalWrite(IR850Led, LOW);}
void IR830LightON(){  digitalWrite(IR830Led, HIGH);}
void IR830LightOFF(){ digitalWrite(IR830Led, LOW);}
void LedCamLightON(){ digitalWrite(CamLed, HIGH);}
void LedCamLightOFF(){  digitalWrite(CamLed, LOW);}
void IRSensLightON(){  digitalWrite(IRLedSens, HIGH);}
void IRSensLightOFF(){ digitalWrite(IRLedSens, LOW);}


