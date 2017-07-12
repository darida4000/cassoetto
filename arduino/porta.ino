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
int DoorUnlock() {
  // inizializzazione servo Blocco
  
  
  bloccoPorta.attach(pinBlocco);
  for (int pos = 60; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    bloccoPorta.write(pos);                // tell servo to go to position in variable 'pos'
    delay(15);                             // waits 15ms for the servo to reach the position
  }
  delay(700);
  bloccoPorta.detach();
  int attesa =0 ;
  while(DoorGetStat() != false) 
  { 
    delay(100);
    attesa++; 
    if(attesa > 600) // se rimane in attesa di apertura per più di un minuto
    {
      DoorLock(); 
      return 0;      
    }
  }

  while(DoorGetStat() == false) { delay(100); }; // se rimane aperto ?

  return 1;
  
}

bool DoorGetStat() {
  //#define pinSporta 42
  return (digitalRead(pinSporta) != 0);
}


