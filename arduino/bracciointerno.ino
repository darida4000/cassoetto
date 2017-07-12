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

