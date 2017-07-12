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

