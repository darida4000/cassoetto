
/////////////////////////////////////////////////////////////////////

  // SENSORI

 /////////////////////////////////////////////////////////////////////


bool GetMetal(int M[]) //COMPLETARE
{

      M[0]= analogRead(analogInPin1);
      M[1] = analogRead(analogInPin2);
      
      return true;
      

}


int GetUV() //COMPLETARE
{
    // Deve restituire un valore tra 0 e 100 dove 0 è opaco. In caso di errore restituire -1
    return 0;

}


float GetWeight() //COMPLETARE
{
  //Serial.println(pesoiniziale);
   // scale.set_scale(calibration_factor); //Adjust to this calibration factor
  
    int attesa = 0;
    do {
      units = GetWeightQuick(); 
      delay(1000);
      attesa++; // aspetto max un minuto
    } while ((abs(pesoiniziale - units) < 0.00000001) && (attesa < 60));
    //Serial.println(units);
    if (pesoiniziale == units) return -1; 
    return units;
}

float GetWeightQuick() 
{
 
   scale.set_scale(calibration_factor); //Adjust to this calibration factor
   float f;
   f= scale.get_units(), 10; // peso

   if (f  < 0) {
    f  = 0.00;
  }
   
   return f;
   
   
}
