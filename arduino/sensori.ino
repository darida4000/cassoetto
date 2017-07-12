


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

