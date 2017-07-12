
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
