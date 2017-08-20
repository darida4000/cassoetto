
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

  UvNirRead(NirUvIniV);


}

void UV400LightON() {
  digitalWrite(UV400Led, HIGH);
}
void UV400LightOFF() {
  digitalWrite(UV400Led, LOW);
}
void IR940LightON() {
  digitalWrite(IR940Led, HIGH);
}
void IR940LightOFF() {
  digitalWrite(IR890Led, LOW);
}
void IR890LightON() {
  digitalWrite(IR890Led, HIGH);
}
void IR890LightOFF() {
  digitalWrite(IR890Led, LOW);
}
void IR850LightON() {
  digitalWrite(IR850Led, HIGH);
}
void IR850LightOFF() {
  digitalWrite(IR850Led, LOW);
}
void IR830LightON() {
  digitalWrite(IR830Led, HIGH);
}
void IR830LightOFF() {
  digitalWrite(IR830Led, LOW);
}
void LedCamLightON() {
  digitalWrite(CamLed, HIGH);
}
void LedCamLightOFF() {
  digitalWrite(CamLed, LOW);
}
void IRSensLightON() {
  digitalWrite(IRLedSens, HIGH);
}
void IRSensLightOFF() {
  digitalWrite(IRLedSens, LOW);
}


bool UV400TraspRead(int UvData[6][3]) {
  UV400LightON();
  delay(2);

  UvData[0][0] = analogRead(UVSense0);
  UvData[1][0] = analogRead(UVSense1);
  UvData[2][0] = analogRead(UVSense2);

  delay(2);
  UV400LightOFF();

  return true;
}

bool IRSensReflRead(int NirData[6][3]) {
  IRSensLightON();
  delay(2);

  NirData[0][1] = analogRead(IRSense0);
  NirData[1][1] = analogRead(IRSense1);
  NirData[2][1] = analogRead(IRSense2);

  delay(2);
  IRSensLightOFF();

  return true;
}

bool IR940TraspRead(int NirData[6][3]) {
  IR940LightON();
  delay(2);

  NirData[0][2] = analogRead(IRSense0);
  NirData[1][2] = analogRead(IRSense1);
  NirData[2][2] = analogRead(IRSense2);

  delay(2);
  IR940LightOFF();

  return true;
}

bool IR890TraspRead(int NirData[6][3]) {
  IR890LightON();
  delay(2);

  NirData[0][3] = analogRead(IRSense0);
  NirData[1][3] = analogRead(IRSense1);
  NirData[2][3] = analogRead(IRSense2);

  delay(2);
  IR890LightOFF();

  return true;
}

bool IR850TraspRead(int NirData[6][3]) {
  IR850LightON();
  delay(2);

  NirData[0][4] = analogRead(IRSense0);
  NirData[1][4] = analogRead(IRSense1);
  NirData[2][4] = analogRead(IRSense2);

  delay(2);
  IR850LightOFF();

  return true;
}

bool IR830TraspRead(int NirData[6][3]) {
  IR830LightON();
  delay(2);

  NirData[0][5] = analogRead(IRSense0);
  NirData[1][5] = analogRead(IRSense1);
  NirData[2][5] = analogRead(IRSense2);

  delay(2);
  IR830LightOFF();

  return true;
}

bool UvNirRead(int UvNirMatrix[6][3]) {
  UV400TraspRead(UvNirMatrix);
  IRSensReflRead(UvNirMatrix);
  IR940TraspRead(UvNirMatrix);
  IR890TraspRead(UvNirMatrix);
  IR850TraspRead(UvNirMatrix);
  IR830TraspRead(UvNirMatrix);
  return true;
}
int UvNirDataAnalizer(int UvNirRead[6][3], int UvNirIntRead[6][3], int UvNirAnalResult[6][3]) {
  int UvNirDiffData[6][3];
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 3; j++) {
      UvNirDiffData[i][j]=UvNirIntRead[i][j]- UvNirRead[i][j];
      if((UvNirDiffData[i][j] > NirUvThreshold[i][j] ))                                              {UvNirAnalResult[i][j]=0;} //oggetto o parte non trasparente
      if((UvNirDiffData[i][j] > NirUvThreshold[i][j]/10)&&(UvNirDiffData[i][j]<NirUvThreshold[i][j])){UvNirAnalResult[i][j]=1;} //oggetto o parte trasparente attenuazione
      if((UvNirDiffData[i][j] < NirUvThreshold[i][j]/10))                                            {UvNirAnalResult[i][j]=2;} //oggetto o parte trasparente senza attenuazione 
    }

  }
  

}



/*
  bool verifica;
  while(true){
  if (verifica==true){
    IR890LightON();
  }else{
    IR890LightOFF();
  }
  verifica =!verifica;
  Serial.print("IR0:");
  Serial.print(analogRead(IRSense0));
  Serial.print("IR1:");
  Serial.print(analogRead(IRSense1));
  Serial.print("IR2:");
  Serial.print(analogRead(IRSense2));
  Serial.print("UV0:");
  Serial.print(analogRead(UVSense0));
  Serial.print("UV1:");
  Serial.print(analogRead(UVSense1));
  Serial.print("UV2:");
  Serial.print(analogRead(UVSense2));
  Serial.println();
  delay(700);
  }
*/

