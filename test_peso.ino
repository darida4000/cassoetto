
// HX711 define
#include "HX711.h"
#define DOUT 23
#define CLK  22
HX711 scale(DOUT, CLK);
float calibration_factor = 2125; //-7050 worked for my 440lb max scale setup
float units;
float ounces;
int outputValue = 0;        // value output to the PWM (analog out)



void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  

  // HX711 init
  scale.set_scale(calibration_factor);
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);

  delay(1000);
}

void loop() {

  
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  Serial.print("Reading: ");
  units = scale.get_units(), 10;
  if (units < 0) {
    units = 0.00;
  }
  ounces = units * 0.035274;
  Serial.print(units);
  Serial.print(" grams"); 
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();

  delay(1000);
}
