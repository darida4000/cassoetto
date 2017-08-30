/////////////////////////////////////////////////////////////////////

// RICICLO PROGETTO DIFFERENZIATA
//
//

/////////////////////////////////////////////////////////////////////



// HX711 define

#include "HX711.h"

#define DOUT 23
#define CLK  22
HX711 scale(DOUT, CLK);
float calibration_factor = 1010; //-7050 worked for my 440lb max scale setup
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

int numberOfSteps = 5900;


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


//Sesnori IR-UV-LuceCam

#define UV400Led  39
#define IR940Led  41
#define IR890Led  43
#define IR850Led  45
#define IR830Led  47
#define CamLed    49
#define IRLedSens 51
//Sensori

#define UVSense1 A2
#define IRSense1 A3
#define IRSense2 A4
#define UVSense2 A5
#define UVSense0 A6
#define IRSense0 A7

int NirUvData[6][3];//Dati di trasparenza NIR UV
/*  {{UV0-400nm,UV1-400nm,UV2-400nm},
     {IR0-Sens,IR1-Sens,IR2-Sens},
     {IR0-940nm,IR1-940nm,IR2-940nm},
     {IR0-890nm,IR1-890nm,IR2-890nm},
     {IR0-850nm,IR1-850nm,IR2-850nm},
     {IR0-830nm,IR1-830nm,IR2-830nm}}
*/
int NirUvIniV[6][3];//Dati iniziali NIR UV

const int NirUvThreshold[6][3] = {{10, 10, 10},
  {40, 40, 40},
  {20, 20, 20},
  {50, 50, 50},
  {30, 30, 30},
  {20, 20, 20}
};
int NirUvAnResult[6][3];//Risultato di analaisi NIR UV


const String MsgLcd[MsgNum][MsgRow] = {
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
#define HomeArm "HmAr"
#define OpenDoor  "OpDo"
#define CloseDoor "ClDo"
#define PlateHoming "PlHom"
#define PlateOnCam "PlCam"
#define PlatePos  "PlPo"   //Parametri 1, numeri 0-3;
#define LcdMessage "LcdMsg" // Parametri 2, numeri 0-100,0-100; codice del messaggio
#define LcdBacklight "LcdBl" //paramtri 1, numeri 0-1;
#define Metal "gtMe"
#define UV "gtUv"
#define Weight "gtWg"
#define QWeight "qgtWg"
#define Reset "reset"

//Parametri di risposta
/*

   OPERAZIONE ANDATA A BUON FINE:
   RISPOSTA:
   <OK>;
   OPERAZIONE ANDATA A BUON FINE CON  VALORI:
   RISPOSTA:
   <OK>5,6,7;
   OPERAZIONE NON RIUSCITA:
   <KO>;

*/
// PROTOTIPI
//Plate
void PlateInit();
void PlateHomingF();
void PlateToCam();
void PlateToRecycle(int can);

//Door
void DoorInit(); //inizializza la porta
void DoorLock();   //blocca lo sportello
int DoorUnlock();   //sblocca lo sportello e rimane in attesa di apertura / chiusura
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
bool GetMetal(int M[]);
float GetWeight();
int GetUvNir();

////Sesnori IR-UV-LuceCam
void IRUVLCInit();
void UV400LightON();
void UV400LightOFF();
void IR940LightON();
void IR940LightOFF();
void IR890LightON();
void IR890LightOFF();
void IR850LightON();
void IR850LightOFF();
void IR830LightON();
void IR830LightOFF();
void LedCamLightON();
void LedCamLightOFF();

void IRSensLightON();
void IRSensLightOFF();

/////////////////////////////////////////////////////////////////////

// SETUP

/////////////////////////////////////////////////////////////////////


void setup() {
  plainSendRecive.init();
  //  plainSendRecive.sendFrame("StartingInit",0);

  LcdInit();
  DoorInit();
  // plainSendRecive.sendFrame("StartingInitStep1",0);
  PlateInit();
  // plainSendRecive.sendFrame("StartingInitStep1b",0);
  WeightScaleInit();
  IRUVLCInit();
  //plainSendRecive.sendFrame("StartingInitStep2",0);
  //  //Serial.begin(57600);  // opens serial port, sets data rate to 57600 baud

  // Print a message to the LCD.
  LcdBackLightON();
  LcdMsg(1, 1);
  delay(1500);
  LcdMsg(0, 0);
  delay(1500);
  LcdBackLightOFF();


  PlateHomingF();
  DoorLock();

  pinMode(LED_BUILTIN, OUTPUT);
  plainSendRecive.sendFrame("InitOk", 0);
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
}

/////////////////////////////////////////////////////////////////////

// LOOP

/////////////////////////////////////////////////////////////////////

void loop() {

  while (true) {
    if (plainSendRecive.available()) {

      if (plainSendRecive.receivedCommand == PlateHoming) {
        PlateHomingF();
        plainSendRecive.sendFrame("Ok", 0);


      }
      else if (plainSendRecive.receivedCommand == OpenDoor) {
        int r = DoorUnlock();
        if (r == 1) plainSendRecive.sendFrame("Ok", 0);
        else plainSendRecive.sendFrame("Ko", 0);
      }
      else if (plainSendRecive.receivedCommand == CloseDoor) {
        DoorLock();
        plainSendRecive.sendFrame("Ok", 0);
      }

      else if (plainSendRecive.receivedCommand == StrokeOfArm) {
        ArmStroke();
        plainSendRecive.sendFrame("Ok", 0);
      }
      else if (plainSendRecive.receivedCommand == HomeArm) {
        ArmHoming();
        plainSendRecive.sendFrame("Ok", 0);
      }
      else if (plainSendRecive.receivedCommand == PlateOnCam) {
        PlateToCam();
        plainSendRecive.sendFrame("Ok", 0);
      }
      else if (plainSendRecive.receivedCommand == PlatePos) {

        PlateToRecycle(plainSendRecive.receivedContent[0]);
        plainSendRecive.sendFrame("Ok", 0);
      }
      else if (plainSendRecive.receivedCommand == Reset) {


        plainSendRecive.sendFrame("InitOk", 0);
      }
      else if (plainSendRecive.receivedCommand == LcdMessage) {

        LcdBackLightON();
        LcdMsg(plainSendRecive.receivedContent[0], plainSendRecive.receivedContent[1]);
        delay(1500);
        plainSendRecive.sendFrame("Ok", 0);
      }
      else if (plainSendRecive.receivedCommand == Weight) {

        float w = GetWeight();
        //Serial.println("peso");
        w *= 100;
        int k = w;
        if (w != -1) plainSendRecive.sendFrame("Ok", 1, k);
        else plainSendRecive.sendFrame("Ko", 0);

      }
      else if (plainSendRecive.receivedCommand == QWeight) {

        float w = GetWeightQuick();
        w *= 100;

        int k = w;

        if (w != -1) plainSendRecive.sendFrame("Ok", 1, k);
        else plainSendRecive.sendFrame("Ko", 0);

      }

      else if (plainSendRecive.receivedCommand == Metal) {
        int m[2] = {0, 0};

        bool ris = GetMetal(m);



        if (ris == true)
        {
          plainSendRecive.sendFrame("Ok", 2, m[0], m[1]);
        }
        else
        {
          plainSendRecive.sendFrame("Ko", 0);
        }

      }
      else if (plainSendRecive.receivedCommand == UV) {


        int ris = GetUvNir();

        if (plainSendRecive.receivedContentLenth == 1) {
          if(plainSendRecive.receivedContent[0] == 1 ){
            /* Serial.println("NirUvIniV");
             for (int i = 0; i < 6; i++) {
              for (int j = 0; j < 3; j++) {
                if(j!=0)Serial.print("-");
                Serial.print( NirUvIniV[i][j]);
                }
                Serial.println();
            }
             Serial.println("NirUvData");
            for (int i = 0; i < 6; i++) {
              for (int j = 0; j < 3; j++) {
                if(j!=0)Serial.print("-");
                Serial.print( NirUvData[i][j]);
                }
                Serial.println();
            }
             Serial.println("NirUvAnResult");
            for (int i = 0; i < 6; i++) {
              for (int j = 0; j < 3; j++) {
                if(j!=0)Serial.print("-");
                Serial.print( NirUvAnResult[i][j]);
                }
                Serial.println();
            }
*/

            // restituisco il valore

            if((NirUvData[1][1] == 0) && (NirUvData[2][1] < 5)) 
            { // vetro
                  //Serial.println("vetro");
                plainSendRecive.sendFrame("Ok", 1, 0);
            }
            else if(((NirUvData[1][2] == 0) && (NirUvData[4][2] > 200)) || ((NirUvData[1][1] == 0) && (NirUvData[4][1] > 500)))
            { // plastica
             // Serial.println("plastica");
                plainSendRecive.sendFrame("Ok", 1, 1);
                
            }
            else plainSendRecive.sendFrame("Ok", 1, 2);
            

            
            

            
          }
        }
        else {
          if (ris >= 0)
          {
            plainSendRecive.sendFrame("Ok", 1, ris);
          }
          else
          {
            plainSendRecive.sendFrame("Ko", 0);
          }
        }
      }



    }

  }
}


