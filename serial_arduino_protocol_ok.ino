// HX711 define

#include "HX711.h"

#define DOUT 23
#define CLK  22
HX711 scale(DOUT, CLK);
float calibration_factor = 2125; //-7050 worked for my 440lb max scale setup
float units;
float ounces;
float pesoiniziale;
int outputValue = 0;        // value output to the PWM (analog out)

//Metal detector define
const int analogInPin1 = A0;  // Analog input pin that the potentiometer is attached to
const int analogInPin2 = A1;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 13; // Analog output pin that the LED is attached to
int sensorMetalValue1 = 0;        // value read from the pot
int sensorMetalValue2 = 0;       // value read from the pot

// Stepper
byte ledPin = 13;
byte enbPin = 25;//8;
byte dire1Pin = 27;//6;
byte step1Pin = 29;//2;
byte slee1Pin = 31;//4;
byte dire2Pin = 33;//7;
byte step2Pin = 35;//3;
byte slee2Pin = 37;//5;


#define OptoLimit1 26//10;
#define OptoLimit2 24//11;
byte Homing1 = 0; // rimettere a 0
byte Homing2 = 0;

int numberOfSteps = 5900; // mezzo giro 6300

int pulseWidthMicros = 450;  // microseconds
int millisbetweenSteps = 1; // milliseconds - or try 1000 for slower steps

// Servo

#include <Servo.h>
Servo braccioSpost;
Servo bloccoPorta;
#define pinBlocco 7  
#define pinBraccio 6
#define pinSporta 42
int val;    // variable to read the value from the analog pin

// LCD display
#include <LiquidCrystal.h>

#define Drs   40
#define Dena  38
#define Dd4   36
#define Dd5   34
#define Dd6   32
#define Dd7   30
#define Dled  28

LiquidCrystal lcd(Drs, Dena, Dd4, Dd5, Dd6, Dd7);

// messaggi lcd

#define MsgNum 10
#define MsgRow 2

const String MsgLcd[MsgNum][MsgRow]={
  { "Inizializzazione", "Cassonetto" },
  { "Cassonetto", "Intelligente" },
  { "Sportello", "Aperto" },
  { "Sportello", "Chiuso" },
  { "Tessera", "Riconosciuta" },
  { "Tessera", "Non Riconosciuta" },
  { "Inserire", "Rifiuto" },
  { "Rifiuto in", "Elabrazione" },

};


//ProtocolPlain

#include <PlainProtocol.h>
PlainProtocol plainSendRecive;

//Operazione 

#define StrokeOfArm "StAr" 
#define HomeArm "HmAr" //Parametri 1, numeri 0-255;
#define OpenDoor  "OpDo"
#define CloseDoor "ClDo"
#define PlateHoming "PlHom" 
#define PlateOnCam "PlCam"
#define PlatePos  "PlPo"   //Parametri 1, numeri 1-4;
#define LcdMessage "LcdMsg" // Parametri 2, numeri 0-100,0-100; codice del messaggio
#define LcdBacklight "LcdBl" //paramtri 1, numeri 0-1;
#define Metal "gtMe" 
#define UV "gtUv" 
#define Weight "gtWg" 

//Parametri di risposta
#define HardwareState "HwSt" // Risposta secca "Ok"
#define SwitchDoorStae "SwDoSt" //Risposta un parametro 0-1; dello stato dello sportello
#define WeightState "WeSt" // Risponde con il peso dell'oggetto 0-1000000; grammi
#define MetalState "MeSt" // Risponde con due Parametri 0-1024,0-1024;
#define IrResponse830 "IrRe0" // Risponde con 4 parametri 0-1024,0-1024,0-1024,0-1024; infrarosso 830nm
#define IrResponse850 "IrRe1" // Risponde con 4 parametri 0-1024,0-1024,0-1024,0-1024; infrarosso 850nm
#define IrResponse890 "IrRe2" // Risponde con 4 parametri 0-1024,0-1024,0-1024,0-1024; infrarosso 890nm
#define IrResponse940 "IrRe2" // Risponde con 4 parametri 0-1024,0-1024,0-1024,0-1024; infrarosso 940nm
#define UvResponse400 "UvRe1" // Risponde con 4 parametri 0-1024,0-1024,0-1024,0-1024; ultravioletto 400nm

// PROTOTIPI
//Plate
void PlateInit();
void PlateHomingF();
void PlateToCam();
void PlateToRecycle(int can);

//Door
void DoorInit(); //inizializza la porta
void DoorLock();   //blocca lo sportello 
void DoorUnlock();   //sblocca lo sportello 
bool DoorGetStat(); //legge lo stato della porta
//StrokeOfArm
void ArmInit();
void ArmStroke();
void ArmHoming();
void servoVaiA(int pos);
// Lcd 
void LcdInit();
void LcdBackLightON();
void LcdBackLightOFF();
void LcdMsg(byte Row0, byte Row1);
// Weight Scale
void WeightScaleInit();
float WeightScaleGet();
float WeightScaleGetZero();

// sensori
bool GetMetal(double M[]);
double GetWeight();
bool GetUV(double U[]);


void setup() {
  plainSendRecive.init();
  //plainSendRecive.sendFrame("StartingInit",0);
  
  LcdInit();
  DoorInit();
  PlateInit();
  WeightScaleInit();
  //plainSendRecive.sendFrame("StartingInitStep2",0);
  //Serial.begin(57600); // opens serial port, sets data rate to 57600 baud

  // Print a message to the LCD.
  LcdBackLightON();
  LcdMsg(1,1);
  delay(1500);
  LcdMsg(0,0);
  delay(1500);
  LcdBackLightOFF();
 

 // PlateHomingF();

  
  pinMode(LED_BUILTIN, OUTPUT);
  plainSendRecive.sendFrame("InitOk",0);


}

void loop() {
 
 while (true) {    
    if (plainSendRecive.available()){
      //Serial.println(plainSendRecive.receivedCommand);
      
      if (plainSendRecive.receivedCommand==PlateHoming) {
        PlateHomingF();
        plainSendRecive.sendFrame("Ok",0);
        
      
      }
      else if (plainSendRecive.receivedCommand==OpenDoor){
        DoorUnlock();
        plainSendRecive.sendFrame("Ok",0);
      }
      else if (plainSendRecive.receivedCommand==CloseDoor){
        DoorLock();
        plainSendRecive.sendFrame("Ok",0);
      }
      
      else if (plainSendRecive.receivedCommand==StrokeOfArm){
        ArmStroke();
        plainSendRecive.sendFrame("Ok",0);
      }
      else if (plainSendRecive.receivedCommand==HomeArm){
        ArmHoming();
        plainSendRecive.sendFrame("Ok",0);
      }
      else if (plainSendRecive.receivedCommand==PlateOnCam){
        PlateToCam();
        plainSendRecive.sendFrame("Ok",0);
      }
      else if (plainSendRecive.receivedCommand==PlatePos){
        
        PlateToRecycle(plainSendRecive.receivedContent[0]);
        plainSendRecive.sendFrame("Ok",0);
      }
        
      else if (plainSendRecive.receivedCommand==LcdMessage){
          
          LcdBackLightON();
          LcdMsg(plainSendRecive.receivedContent[0],plainSendRecive.receivedContent[1]);
          delay(1500);
          plainSendRecive.sendFrame("Ok",0);
      }
      else if (plainSendRecive.receivedCommand==Weight){
          
          double w = GetWeight();
          //Serial.println("peso");
          w*=100;
          int k=w;
          if(w!=-1) plainSendRecive.sendFrame("Ok",1,k);
          else plainSendRecive.sendFrame("Ko",0);
          
      }
      else if (plainSendRecive.receivedCommand==Metal){
          double M[2]={0,0};           
          bool ris = GetMetal(M);
          M[0]*=100;
          M[1]*=100;
          int m1 = M[0];
          int m2 = M[1];
            
          if(ris == true)
          {
            plainSendRecive.sendFrame("Ok",2,m1,m2);
          }
          else
          {
            plainSendRecive.sendFrame("Ko",0);
          }
          
      }
      else if (plainSendRecive.receivedCommand==UV){

          double risU[3]={0,0,0};
          bool ris = GetUV(risU);
          char t[100];
          int v1,v2,v3;
          risU[0]*=100;
          risU[1]*=100;
          risU[2]*=100;
          v1=risU[0];
          v2=risU[1];
          v3=risU[2];
                    
          if(ris == true)
          {
            plainSendRecive.sendFrame("Ok",3,v1,v2,v3);
          }
          else
          {
            plainSendRecive.sendFrame("Ko",0);
          }
      }

      
      
    }
    else{
      
    }
//    delay(1000);
    }

    
  }


