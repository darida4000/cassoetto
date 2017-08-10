
  /////////////////////////////////////////////////////////////////////

  // RICICLO PROGETTO DIFFERENZIATA
  //
  //

  /////////////////////////////////////////////////////////////////////



  /////////////////////////////////////////////////////////////////////

  // INCLUDES

  /////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <utility>


#include "rs232.h"
#include <curl/curl.h>

#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>





// COSTANTI
#define BUF_SIZE 128
#define IMG_OBJECT 8 // immagini nel database
#define IMG_SCENE 3 // numero fotogrammi
#define ORB_PRECISION 5000 // precisione orb
#define SOGLIA_SFONDO 20 // soglia per eliminazione sfondo
#define SOGLIA_CARTA 1.5

using namespace cv;
using namespace std;

// PROTOTIPI

bool niceHomography(Mat* H);
double homographyRating(Mat* H);
void readImages();
int findMaxIndex();
void captureImages(int cam);
string calcolaEmd();
void  salva_dati_thingspeack(string codice_tessera, string mat, float peso);
std::vector<std::string> explode(std::string const & s, char delim);
void readConfig();
int checkAll();
string imageDetection();
string colorQuickWin();
float shapeDetection(Mat image1);

  /////////////////////////////////////////////////////////////////////

  // DICHIARAZIONI

  /////////////////////////////////////////////////////////////////////

// OpenCV
int maxImages=IMG_OBJECT; // togliere
int objectIndex=0;
Mat img_object;
Mat img_scene[IMG_SCENE];
VideoCapture cap;
bool found=false;
Mat img_object_data[IMG_OBJECT];
Mat img_matches[IMG_OBJECT];
double determinanti[IMG_OBJECT][IMG_SCENE];
Ptr<Feature2D> f2d;
Ptr<FeatureDetector> detector;
Ptr<DescriptorExtractor> extractor;
Mat descriptors_object[IMG_OBJECT], descriptors_scene[IMG_SCENE];
std::vector<KeyPoint> keypoints_object[IMG_OBJECT], keypoints_scene[IMG_SCENE];
String nomiRifiuti[IMG_OBJECT];
int colindex=0;
double maxFound=0;
Mat bg; Mat carta[3]; Mat shape;
BFMatcher matcher(NORM_HAMMING);
  
  // RFID
  int i=0;
  int k=0;
  int cport_nr_arduino=25;
  int bdrate_arduino=57600;
  int cport_nr=24;
  int bdrate=19200;
  char mode[]={'8','N','1',0}; // 8 data bits, no parity, 1 stop bit
  char str_send[BUF_SIZE]; // send data buffer
  unsigned char str_recv[BUF_SIZE]; // recv data buffer
  char codice_tessera_hex[6];
bool ack;
bool waitArduinoAck()
{
	int attesa = 0;
		while(1)
		{
			int n = RS232_PollComport(cport_nr_arduino, str_recv, (int)BUF_SIZE);
			if(n > 0){
				str_recv[n] = 0;
				std::string str;
				str.append(reinterpret_cast<const char*>(str_recv));
				if(str == "<Ok>;") 
				{
					
					return true;				
				}
				
			}
			attesa++;
			usleep(1000000);  
			if(attesa > 60) return false; 
		}
}

int main()
{
	cout << "Accensione" << endl << flush;

     readConfig(); // lettura file testuale di configurazione - probing

	
  /////////////////////////////////////////////////////////////////////

  // CONTROLLI

  /////////////////////////////////////////////////////////////////////

   if(checkAll()==-1) return 0;


  /////////////////////////////////////////////////////////////////////

  // INIZIALIZZAZIONI

  /////////////////////////////////////////////////////////////////////
	// open cv

	f2d= ORB::create();
    int minHessian = 400;
    detector = ORB::create(ORB_PRECISION);
     
	

	// legge tutte le immagini dal disco

     readImages();

	usleep(1000000);  /* ASPETTA un secondo */

  /////////////////////////////////////////////////////////////////////

  // INZIO LOOP PRINIPALE

  /////////////////////////////////////////////////////////////////////
	
	cout << "Ready to go!" << endl << flush;

	while(true)
	{

  // INIZIALIZZO VARIABILI PER I VALORI OTTENUTI DA ARDUINO:

		float peso = 0.0;
		float metallo= 0.0;
		float uv= 0.0;

  /////////////////////////////////////////////////////////////////////

  // ATTESA RFID

  /////////////////////////////////////////////////////////////////////

		while(1)
		{
			cout << "Pool rfid" << endl << flush;

			int n = RS232_PollComport(cport_nr, str_recv, (int)BUF_SIZE);
			if(n > 0){
				str_recv[n] = 0;
				sprintf(codice_tessera_hex,"%02X%02X%02X",str_recv[6],str_recv[7],str_recv[8]);
				//printf("CODICE TESSERA PER DATABASE:%s \n",codice_tessera_hex);
				break; // esco dal ciclo se tessera strisciata
			}
			usleep(1000000);  // 1 secondo di pausa
		} // fine ciclo lettura su tessera



  /////////////////////////////////////////////////////////////////////

  // CICLO COMUNICAZIONE ARDUINO

  /////////////////////////////////////////////////////////////////////

  // STEP 1)  DICO A ARDUINO DI APRIRE LO SPORTELLO E RIMANERE IN ATTESA DELLA BILANCIA

		printf("Inserire il rifiuto \n");
		RS232_cputs(cport_nr_arduino, "<OpDo>;"); // apertura lock e attesa di apertura chiusura sportello
		
		ack  = waitArduinoAck();
		if (ack == false) 
		{
			cout << "Errore nella comunicazione o sportello non chiuso-> apertura porta"<< endl;
			continue;
		}
		 
		
		//usleep(1000000);  /* aspetto un secondo */
		printf("Lo sto pesando... \n");
		
		// STEP 2) CHIEDO  IL PESO E RIMANGO IN ATTESA CHE ARDUINO MI RESTITUISCA IL PESO
				
		RS232_cputs(cport_nr_arduino, "<gtWg>;");
		usleep(2000000);  /* pausa */

		bool timeout = false;
		while(1)
		{
			int n = RS232_PollComport(cport_nr_arduino, str_recv, (int)BUF_SIZE);
			if(n > 0){
				str_recv[n] = 0;
				std::string str,str1;
				str.append(reinterpret_cast<const char*>(str_recv));
				
				if (str[1] == 'K') // è arrivato <Ko>, il peso non è arrivato in 60 secondi;
				{
					timeout = true;
					break;
					
				}
				
				cout << "Atterrato un oggetto ..." << str << endl << flush;
				str1=str.substr(4);
				
				
				peso = atof(str1.c_str());
				peso/=100;
				break;
			}
		usleep(1000000);  /* pausa */
		}
		
		// STEP 2 bis CHIUSURA PORTA
		
		RS232_cputs(cport_nr_arduino, "<ClDo>;"); // chiusura porta
		
		ack  = waitArduinoAck();
		if (ack == false) 
		{
			cout << "Errore nella comunicazione -> chiusura porta"<< endl;
			continue;
		}
		///// ripeso
		
		RS232_cputs(cport_nr_arduino, "<qgtWg>;");
		usleep(2000000);  /* pausa */		
		while(1)
		{
			int n = RS232_PollComport(cport_nr_arduino, str_recv, (int)BUF_SIZE);
			if(n > 0){
				str_recv[n] = 0;
				std::string str,str1;
				str.append(reinterpret_cast<const char*>(str_recv));		
				cout << "Ricevuto: " << str << endl << flush;
				str1=str.substr(4);				
				peso = atof(str1.c_str());
				peso/=100;
				break;
			}
		usleep(1000000);  /* pausa */
		}
		cout << "Ricevuto peso: " << peso << endl << flush;
		// STEP 2 bis CHIUSURA PORTA
		
		////ripeso
		if (timeout == true)  continue; // non è attrerrato nulla sulla bilancia !
	
		// STEP 3) CHIEDO  A ARDUINO DI POSIZIONARSI SOTTO CAM
		 
		RS232_cputs(cport_nr_arduino, "<PlCam>;"); // posizione webcam		
		
		ack  = waitArduinoAck();

		if (ack == false) 
		{
			cout << "Errore nella comunicazione -> posizionamento webcam"<< endl;
			continue;
		}
		
		usleep(3000000);  /* aspetto un secondo */

    // STEP 4) FACCIO LE FOTO

		captureImages(0);

    // STEP 5) CHIEDO AD ARDUINO DI  MUOVERE IL BRACCIO INTERNO

		RS232_cputs(cport_nr_arduino, "<StAr>;"); // sposto braccio	
		
		ack  = waitArduinoAck();

		if (ack == false) 
		{
			cout << "Errore nella comunicazione -> spostamento braccio"<< endl;
			continue;
		}
		
	// STEP 6) RIMANGO I ATTESA CHE ARDUINO MI COMUICHI I VALORI DEI SENSORI METALLI E UV	
		
		RS232_cputs(cport_nr_arduino, "<gtMe>;"); // richiedo sensore metalli
    
		while(1)
		{		
			int n = RS232_PollComport(cport_nr_arduino, str_recv, (int)BUF_SIZE);
			if(n > 0){
				str_recv[n] = 0;
				//printf("Ricevuto: %s",str_recv);
				std::string str,str1;
				str.append(reinterpret_cast<const char*>(str_recv));
				
				str1=str.substr(4);
				cout << "Ricevuto: " << str1 << endl << flush;
				
				std::vector<std::string> v = explode(str1, ',');
				
				metallo = atof(v[0].c_str()) + atof(v[1].c_str());   // metallo
				//metallo /=100;
				
				break; // esce
			}
			usleep(1000000);  // pausa
		}
cout << "Metallo:" <<metallo << endl << flush;
		RS232_cputs(cport_nr_arduino, "<gtUv>;"); // richiedo sensore UV
		

    // STEP 6) RIMANGO I ATTESA CHE ARDUINO MI COMUICHI I VALORI DEI SENSORI METALLI E UV
		while(1)
		{		
			int n = RS232_PollComport(cport_nr_arduino, str_recv, (int)BUF_SIZE);
			if(n > 0){
				str_recv[n] = 0;
				//printf("Ricevuto: %s",str_recv);
				std::string str,str1;
				str.append(reinterpret_cast<const char*>(str_recv));
				
				str1=str.substr(4);
				cout << "Ricevuto: " << str1 << endl << flush;
				
				//std::vector<std::string> v = explode(str1, ',');
				
				//uv = atof(v[0].c_str()) + atof(v[1].c_str())+ atof(v[2].c_str());   // uv
				uv = atof(str1.c_str());
				//uv /=100;
				
				break; // esce
			}
			usleep(1000000);  // pausa
		}

	// STEP 6 BIS) RIMETTO A POSTO IL BRACCETTO

		RS232_cputs(cport_nr_arduino, "<HmAr>;"); // sposto braccio	
		
		ack  = waitArduinoAck();

		if (ack == false) 
		{
			cout << "Errore nella comunicazione -> spostamento braccio"<< endl;
			continue;
		}
		




		cout <<  "DATI OTTENUTI: " << metallo << "  " << uv << endl ;

  // STEP 7)  ELABORAZIONI, ARDUINO RIMANE IN ATTESA DI RESPONSO
		bool trovato = false;
		char risultato[100];
		string r;
		sprintf(risultato,"I - Indifferenziato");
    
		if((metallo > 0)) // && (peso > 40) && (peso < 50)) // per il metallo non interessa il peso
		{
			sprintf(risultato,"M - Lattina");
			trovato = true;
		}
	
		if((uv > 0.00) && (peso > 40) && (peso < 50) && (trovato ==false))
		{
			sprintf(risultato,"P - Bottiglia di plastica");
			trovato = true;
		}
	/////////////////////////////////////////
		if(trovato == false)
		{
			r = colorQuickWin();
			if(r != "")
			{
				sprintf(risultato,"%s",r.c_str());
				trovato = true;
			}
		}
		
		// controllo bicchiere
		int y = shapeDetection(img_scene[2]);
		if(y==1)
		{
			sprintf(risultato,"%s","P - Bicchiere di plastica");
			trovato = true;
		}
		
		// controllo con istogramma
		if(trovato == false) 
		{
			r = calcolaEmd();
			if((r!="" )) 
			{
				
				sprintf(risultato,"%s",r.c_str());
				trovato = true;
			}
		}
	
	// metodo orb
		if(trovato == false) // se non è escuso, passo al video
		{
			r = imageDetection();
			if((r != "") && (peso < 8));
			{
				sprintf(risultato,"%s",r.c_str());
				trovato = true;
			}
		}	
	
        
		//RS232_cputs(cport_nr_arduino, risultato);
		
		cout <<  "Il risultato è:" << risultato << endl << flush;
		
		int can=-1;
		string message ="<PlPo>0;";
		if (risultato[0]  == 'I') message ="<PlPo>0;";
		else if (risultato[0]  == 'C') message ="<PlPo>1;";
		else if (risultato[0]  == 'P') message ="<PlPo>2;";
		else if (risultato[0]  == 'M') message ="<PlPo>3;";
		
		cout << "Messaggio " << message;

   // STEP 8) ASPETTO CHE ARDUINO MI DICA DI AVER  GETTATO RIFIUTO 


		RS232_cputs(cport_nr_arduino, message.c_str()); 
		
		
		
		ack  = waitArduinoAck();

		if (ack == false) 
		{
			cout << "Errore nella comunicazione -> posizionamento piatto per cassonetto"<< endl;
			continue;
		}
		
		RS232_cputs(cport_nr_arduino, "<PlHom>;"); // posizione home		
		
		ack  = waitArduinoAck();

		if (ack == false) 
		{
			cout << "Errore nella comunicazione -> posizionamento home"<< endl;
			continue;
		}

  // STEP 9) SALVO I DATI NEL CLOUD

		salva_dati_thingspeack(codice_tessera_hex,risultato,peso);

		cout << "FINE" << endl;

		//break; // togliere
		
} // fine loop principale



   cap.release();

  return(0);
}



  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO

  /////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO

  /////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO

  /////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Legge le immagini dal disco

  /////////////////////////////////////////////////////////////////////




void readImages()
{
	// matching
shape = imread( "img/bicchiere.jpg",1);
        // per EMD
    bg=imread("img/sfondo.jpg",CV_LOAD_IMAGE_GRAYSCALE);

    carta[0] = imread( "img/cartabiancaok.jpg",1);
    carta[1] = imread( "img/cartamarroneok.jpg",1); // sostituire con carta bianca
    carta[2] = imread( "img/retrobueno.jpg",1); // sostituire con carta bianca
    // per ORB
    
  img_object_data[0] = imread("img/bueno.jpg" , CV_LOAD_IMAGE_GRAYSCALE );
  nomiRifiuti[0] = "Kinder bueno";
  img_object_data[1] = imread("img/croc.jpg" , CV_LOAD_IMAGE_GRAYSCALE );
  nomiRifiuti[1] = "Croccantelle";
  img_object_data[2] = imread("img/sc1.jpg" , CV_LOAD_IMAGE_GRAYSCALE );
  nomiRifiuti[2] = "Schiacciatelle al rosmarino";
  img_object_data[3] = imread("img/scretro.jpg" , CV_LOAD_IMAGE_GRAYSCALE );
  nomiRifiuti[3] = "Schiacciatelle - retro ";
  img_object_data[4] = imread("img/fz1.jpg" , CV_LOAD_IMAGE_GRAYSCALE );
  nomiRifiuti[4] = "Fonzie";
  img_object_data[5] = imread("img/ps2.jpg" , CV_LOAD_IMAGE_GRAYSCALE );
  nomiRifiuti[5] = "pizzottelle 2";
  img_object_data[6] = imread("img/retrobueno.jpg" , CV_LOAD_IMAGE_GRAYSCALE );
  nomiRifiuti[6] = "Retro bueno";
  img_object_data[7] = imread("img/milka.jpg" , CV_LOAD_IMAGE_GRAYSCALE );
  nomiRifiuti[7] = "Milka";

  for(int i =0;i< maxImages; i++) for(int ii =0;ii< 3; ii++) determinanti[i][ii] = 0;

	for (int i=0;i< maxImages;i++)
	{
	
		detector->detectAndCompute(img_object_data[i], noArray(), keypoints_object[i], descriptors_object[i]);
		/*if(descriptors_object[i].type()!=CV_32F) {
          descriptors_object[i].convertTo(descriptors_object[i], CV_32F);
        }*/


	}
}
  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Shape detection

  /////////////////////////////////////////////////////////////////////
float shapeDetection(Mat image1)
{ 
	RNG rng(12345);
    Mat maschera,mm;
    Mat backupFrame;
   // Mat image1=imread("img/bicchiere.jpg",1); // shape base
    // theresold !!!!
           /* backupFrame = image1.clone();
		cvtColor( image1, image1, cv::COLOR_BGR2GRAY  );
		absdiff(bg,image1,maschera);
		threshold(maschera,maschera,SOGLIA_SFONDO,255,THRESH_BINARY);
		backupFrame.copyTo(image1, maschera);
		*/
		
    Mat imagegray1,imageresult1;
    int thresh=150;
    double ans=0, result=0;;
    cvtColor(image1, imageresult1,CV_BGR2GRAY);
    
    
	threshold(imageresult1,imageresult1,80,255,THRESH_BINARY);
	
    vector<vector<Point> >contours1;
    vector<Vec4i> hierarchy1;

    findContours(imageresult1,contours1,hierarchy1,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE,cvPoint(0,0));
    for(int i=0;i<contours1.size();i++)
    {
        Scalar color=Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
        //approxPolyDP( Mat(contours1[i]), contours1[i], 6, true );
        drawContours(imageresult1,contours1,i,color,1,8,hierarchy1,0,Point());

    }

    
    cout << "qui"<< endl <<flush;
    double maxC1=0;
    int maxC1In = 0;
    for(int i=0;i<contours1.size();i++)
    {
		double cLen = arcLength(contours1[i],false);
		if (cLen > maxC1)
		{
			maxC1 = cLen;
			maxC1In = i; 
		}
		
	}
	
    Scalar color=Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
        
     drawContours(imageresult1,contours1,maxC1In,color,3,8,hierarchy1,0,Point());

	Rect boundRect = boundingRect( contours1[maxC1In] );
	double a1= boundRect.area();
   
  
	cout  << "Area elemanto: " << a1;
       
    cout << endl;

	Moments mom = cv::moments(contours1[maxC1In]); 
	double hu[7];
	HuMoments(mom, hu); // now in hu are your 7 Hu-Moments

	for (int i=0;i<7;i++)
	{
		cout << hu[i] << endl;
		
	}


	
	hu[0] = round(hu[0] * 100) /100;     // 0.18
	
	hu[2] = round(hu[2] * 1000) /1000;   // 0.001
	
	cout << "Momenti di hu" << hu[0] << " --> " << hu[2] << endl;
	
	if(((hu[0]==0.18) || (hu[0]==0.19)) && ((hu[2]==0.001) || (hu[2]==0.002)) )
	return 1;
	else
	return 0;
	
	
	 	
    
    

  }

  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Acquisisce immagini da webcam

  /////////////////////////////////////////////////////////////////////

void captureImages(int cam)
{

	cout << "Inizio cattura img" << endl << flush;

    try
    {
      for(int i=0;i<1;i++) cap >> img_scene[0];
      usleep(1000000); 
      for(int i=0;i<20;i++) cap >> img_scene[1];
      usleep(1000000); 
      for(int i=0;i<40;i++) cap >> img_scene[2];
    }
          catch(cv::Exception& e)
    {


    }
    
    imwrite("img/lastimg.jpg",img_scene[2]);
    
	Mat maschera;
    Mat backupFrame;
	for(int i=0;i<IMG_SCENE ;i++)
	{
		backupFrame = img_scene[i].clone();
		cvtColor( img_scene[i], img_scene[i], cv::COLOR_BGR2GRAY  );
		absdiff(bg,img_scene[i],maschera);
		threshold(maschera,maschera,SOGLIA_SFONDO,255,THRESH_BINARY);
		backupFrame.copyTo(img_scene[i], maschera);

	}
	/*float f = shapeDetection();
	cout << f;*/
	imshow("stato cam 0",img_scene[0]);
	imshow("stato cam 1 ",img_scene[1]);
	imshow("stato cam 2",img_scene[2]);
    waitKey(0);
	cout << "immagini catturate" << endl << flush;
	
	for (int i=0;i< 3;i++)
	{

		detector->detectAndCompute(img_scene[i], noArray(), keypoints_scene[i], descriptors_scene[i]);

        /*if(descriptors_scene[i].type()!=CV_32F) {
          descriptors_scene[i].convertTo(descriptors_scene[i], CV_32F);
        }*/

	}
	
}

  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Cotrollo omografia per orb

  /////////////////////////////////////////////////////////////////////
  
bool niceHomography(Mat* H)
{
	const double det = H->at<double>(0,0)* H->at<double>(1,1)-H->at<double>(1,0)* H->at<double>(0,1);
	if (det<0) return false;
	const double N1 = sqrt(H->at<double>(0,0)* H->at<double>(0,0)+H->at<double>(1,0)* H->at<double>(0,1));
	if (N1>4 || N1 < 0.1) return false;
	const double N2 = sqrt(H->at<double>(0,1)* H->at<double>(0,1)+H->at<double>(1,1)* H->at<double>(1,1));
	if (N2>4 || N2 < 0.1) return false;
	const double N3 = sqrt(H->at<double>(2,0)* H->at<double>(2,0)+H->at<double>(2,1)* H->at<double>(2,1));
	if (N3>0.002) return false;

	return true;


}

double homographyRating(Mat* H)
{

    const double det = H->at<double>(0,0)* H->at<double>(1,1)-H->at<double>(1,0)* H->at<double>(0,1);
    return det;

}

  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Trova il massimo valore nella matrice dei determinanti per ORB

  /////////////////////////////////////////////////////////////////////
  
int findMaxIndex()
{

    cout << "determinanti:" << endl;
    cout << "--------------" << endl;

    double max=0;
    int indice = -1;
    int k;
    for( k=0;k< maxImages;k++)
    {

      for(int h=0;h<3;h++)
      {
        cout << " " << determinanti[k][h];
        if (determinanti[k][h] > 1.5) continue; // modificata  soglia esclusione
        if(determinanti[k][h] > max)
        {

          max = determinanti[k][h];
          indice = k;
          colindex=h;
        }
      }
      cout << endl;
    }
    maxFound=max;
    return indice;

}

  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // calcola EMD

  /////////////////////////////////////////////////////////////////////
  
string calcolaEmd()
{

     string ris="";
    Mat src_base, hsv_base;
    Mat src_test1, hsv_test1;
    Mat src_test2, hsv_test2;
    Mat hsv_half_down;


    int imgCarta=3;
   
    Mat scene[3];
    Mat hsv_carta[imgCarta];
    Mat hsv_scene[3];
    Mat mask_carta[imgCarta];
    Mat mask_scene[3];

   
	Mat maschera,mm;
    Mat backupFrame;
    
    string s="";
    string t="";
	

    	// elimino lo sfondo da  immagini di carta
    for(int i=0;i<imgCarta;i++)
      {
        backupFrame = carta[i].clone();
		cvtColor( carta[i], carta[i], cv::COLOR_BGR2GRAY  );
		absdiff(bg,carta[i],maschera);
		threshold(maschera,maschera,SOGLIA_SFONDO,255,THRESH_BINARY);
		backupFrame.copyTo(carta[i], maschera);

      /// Converto in HSV
        cvtColor( carta[i], hsv_carta[i], COLOR_BGR2HSV );



    inRange(hsv_carta[i], Scalar(0, 15, 50), Scalar(180, 255, 255), mask_carta[i]);

      }
      /*imshow("carta originaria",carta[1]);
       waitKey(0);*/

    for(int i=0;i<3;i++)
      {
        scene[i] = img_scene[i].clone();
        //scene[i] = scene[i] - bg;
        cvtColor( scene[i], hsv_scene[i], COLOR_BGR2HSV );
        inRange(hsv_scene[i], Scalar(0, 15, 50), Scalar(180, 255, 255), mask_scene[i]);
      }


   // setup
    int h_bins = 20; int s_bins = 20; int v_bins =50;
    int histSize[] = { h_bins, s_bins };

    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    float v_ranges[] = { 100, 256 };

    const float* ranges[] = { h_ranges,  s_ranges };


    int channels[] = { 0,1 };

    MatND hist_base[imgCarta];
    MatND hist_test[3];

    for (int i=0;i<imgCarta;i++)
    {
      calcHist( &hsv_carta[i], 1, channels, mask_carta[i], hist_base[i], 2, histSize, ranges, true, false );
      normalize( hist_base[i], hist_base[i], 0, 1, NORM_MINMAX, -1, Mat() );
    }
    for (int i=0;i<3;i++)
    {
      calcHist( &hsv_scene[i], 1, channels, mask_scene[i], hist_test[i], 2, histSize, ranges, true, false );
      normalize( hist_test[i], hist_test[i], 0, 1, NORM_MINMAX, -1, Mat() );
    }


  // calcolo EMD
  for(int i=0;i<imgCarta;i++)   /// per ogni img carta
  {
	  
  for (int j=1;j<3;j++) // per ogni fotogramma
  {
	
  vector<cv::Mat> sig(3);
  MatND hist[2];

  hist[0] = hist_base[i].clone();
  hist[1] = hist_test[j].clone();

  for(int ii = 0;ii<2;ii++)
  {
    vector <cv::Vec3f> sigv;
    normalize( hist[ii], hist[ii], 1, 0, NORM_L1 );

    for(int h=0;h<h_bins;h++)
    {
      for(int s=0;s< s_bins;s++)
       {

        float bin_val=hist[ii].at<float>(h,s);
        if(bin_val!=0) sigv.push_back(cv::Vec3f(bin_val,(float)h,(float)s));

      }
      sig[ii] =cv::Mat(sigv).clone().reshape(1);


    }
    if(ii>0){

        float emdResult = EMD(sig[0],sig[ii],cv::DIST_L1);
        cout << "EMD:" << emdResult << endl;
        
        switch(i)
        {
			case 0:	
			if (emdResult< SOGLIA_CARTA-0.2) return "C - Carta";
			break;
			case 1:	
			if (emdResult< 4) return "C - Carta marrone";
			break;
			case 2:	
			if (emdResult< 2.4) return "P - Retro bueno";
			break;
		}
     }
    }
   }
 }


return ris;
}
  /////////////////////////////////////////////////////////////////////

  // CHIAMATA CURL PER THINGSPEACK

  /////////////////////////////////////////////////////////////////////
  
void salva_dati_thingspeack(string codice_tessera, string mat, float peso)
{


  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if(curl) {
	  std::ostringstream p;
      p << peso;
	   std::string pesoS = p.str();
	  string invio;
	  invio = "https://api.thingspeak.com/update?api_key=";
	  // https://api.thingspeak.com/update?api_key=1BDO257EXYZC2O51&field1=5
	  // https://api.thingspeak.com/update?api_key=GNB4H517VAVEHAYU&field1=5
	  if(codice_tessera=="3BF827")
	  {
		  invio += "1BDO257EXYZC2O51"; // api key
		
	  }
	  else
	  {
		   invio +="GNB4H517VAVEHAYU"; // api key
		
	  }
	  if(mat[0] == 'C')
	  {
		  invio =invio + "&field1=" + pesoS;
	  }
	  
	  if(mat[0] == 'M')
	  {
		invio =invio + "&field2=" + pesoS;
	  }
	  	  if(mat[0] == 'P')
	  {
		  invio =invio + "&field3=" + pesoS;
	  }
	  	  if(mat[0] == 'I')
	  {
		  invio =invio + "&field4=" + pesoS;
	  }  
	  cout << "URL inviata a thigspeack:" << invio<< endl << flush;
    
    curl_easy_setopt(curl, CURLOPT_URL, invio.c_str());
	  
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    res = curl_easy_perform(curl);
    // controllo errori chiamata
   /* if(res != CURLE_OK) cout << "Errore";
      fprintf(stderr, "errore: %s\n",
              curl_easy_strerror(res));*/

    curl_easy_cleanup(curl);
  }
}
  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Esplode una stringa

  /////////////////////////////////////////////////////////////////////
  
std::vector<std::string> explode(std::string const & s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); )
    {
        result.push_back(token);
    }

    return result;
}

  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Legge file configurazione

  /////////////////////////////////////////////////////////////////////
  
void readConfig()
{/*
	ifstream fp;
	char line[100];
	std::vector<std::string> v ;
	String s;
	
	fp.open("config.riciclo");
	fp >> line;
	
	s=line;
	v = explode(s, '=');
    cport_nr_arduino = atof(v[1].c_str());  
	fp >> line;
	
	s=line;
	v = explode(line, '=');
    cport_nr = atof(v[1].c_str()); 	
    
    fp.close();
    
   int p=24;
    
   while(1)
   {
	cout << "Cerco arduino in porta " << p << endl << flush;

	int n = RS232_PollComport(p, str_recv, (int)BUF_SIZE);
    if(n > 0){
        str_recv[n] = 0;
		cout << "Arduino è sulla "<< p  << endl << flush;
		
        break; 
    }
    p++;
    if(p==26) p = 24;
	usleep(1000000);  // 1 secondo di pausa
   } // fine ciclo lettura su tessera

    if(p==24)
    {
		cport_nr_arduino=24;
		cport_nr=25;
	}
	else
	{
		cport_nr_arduino=25;
		cport_nr=24;
		
	}
	*/
	cport_nr_arduino=RS232_GetPortnr("arduino");
	//cout << "porta arduino:" << cport_nr_arduino;
	cport_nr=RS232_GetPortnr("rfid");
	 
   
	
}


  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Controlli iniziali

  /////////////////////////////////////////////////////////////////////
  
  
int checkAll()
{
  if(RS232_OpenComport(cport_nr, bdrate, mode))
  {
    printf("Problema apertura porta scheda rfid\n");
    return -1;
  }

  if(RS232_OpenComport(cport_nr_arduino, bdrate_arduino, mode))
  {
    printf("Problema apertura comunicazione con arduino\n");
    return -1;
  }
  
  // attendo che arduino mi confermi la corretta inizializzazione
	while(1)
	{
		cout << "Pool arduino" << endl << flush;
		int n = RS232_PollComport(cport_nr_arduino, str_recv, (int)BUF_SIZE);
		if(n > 0){
				str_recv[n] = 0;
				std::string str;
				str.append(reinterpret_cast<const char*>(str_recv));
				cout << "Arduino pronto! " << str << endl << flush;
				if(str=="<InitOk>;") break; // esco dal ciclo se tessera strisciata
		}
		usleep(1000000);  // 1 secondo di pausa
	} 
  
  
  

  try
	{
		if(!cap.open(0))
		{
			cout << "Errore apertura cam " << 0;
        }
    }
    catch(cv::Exception& e)
    {
		sleep(3000);
		if(!cap.open(0))
		{
			cout << "Errore apertura cam " << 0;
			return -1;
        }
	}

}
  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Applica quickwins basati sul colore prevalente  dell'oggetto

  /////////////////////////////////////////////////////////////////////
string colorQuickWin()
{
    
    string ris="";
    double pixel; 
    double perc;
    Mat test2_mask,hsv_test2;
    
    // controllo sul terzo fotogramma
    cvtColor( img_scene[2], hsv_test2, COLOR_BGR2HSV );

	// da ripetere per ogni prodotto
	
    inRange(hsv_test2, Scalar(25, 15, 50), Scalar(33, 255, 255), test2_mask); // valore per fonzie con percentuali intorno al 15
    pixel = countNonZero(test2_mask); //  307200 pixel totali per immagini 640 x 480
    perc = pixel*100/307200;
    cout << "percentuale" << perc << endl << flush;
    if (perc > 4 ) 
    {
		ris = "P - Fonzies o TUC"; 
		return ris;
	}
	cout << perc << endl << flush;

	// fine da ripetere per ogni prodotto


	// da ripetere per ogni prodotto
	
    inRange(hsv_test2, Scalar(115, 100, 70), Scalar(125, 200, 200), test2_mask); // valore per milka con percentuali intorno al 15
    pixel = countNonZero(test2_mask); //  307200 pixel totali per immagini 640 x 480
    perc = pixel*100/307200;
    cout << "percentuale M" << perc << endl << flush;
    if (perc > 7) 
    {
		ris = "P - Milka"; 
		return ris;
	}
	cout << perc << endl << flush;

	// fine da ripetere per ogni prodotto
	

    return ris;
 
}

  /////////////////////////////////////////////////////////////////////

  // FUNZIONI APPOGGIO
  // Applica metodo ORB

  /////////////////////////////////////////////////////////////////////
  
string imageDetection()
{
	string ris="";
 // confronto ogni immagine del database con i tre fotogrammi acquisiti

    for (int imgDb=0; imgDb<maxImages;imgDb++)
    {
		for (int imgIndex=1;imgIndex<3;imgIndex++)
		{
		// dichiaro e alloco alcuni oggetti che mi serviranno
			std::vector< DMatch > matches;
			double max_dist = 0; double min_dist = 100;
   
            matcher.match( descriptors_object[imgDb], descriptors_scene[imgIndex], matches );

			for( int i = 0; i < descriptors_object[imgDb].rows; i++ )
			{ double dist = matches[i].distance;
				if( dist < min_dist ) min_dist = dist;
				if( dist > max_dist ) max_dist = dist;
			}

			std::vector< DMatch > good_matches;

			for( int i = 0; i < descriptors_object[imgDb].rows; i++ )
			{ if( matches[i].distance <= 3*min_dist )
            { good_matches.push_back( matches[i]); }
			}

			std::vector<Point2f> obj;
			std::vector<Point2f> scene;

			for( int i = 0; i < good_matches.size(); i++ )
			{
          
				obj.push_back( keypoints_object[imgDb][ good_matches[i].queryIdx ].pt );
				scene.push_back( keypoints_scene[imgIndex][ good_matches[i].trainIdx ].pt );
			}
        
			if(good_matches.size()>4)
			{
				Mat H = findHomography( obj, scene, CV_RANSAC );
	            if((niceHomography(&H) == true) || (min_dist<100)) 
				{	
					determinanti[imgDb][imgIndex] = homographyRating(&H);
				}
			}
        
		} 
	}

	int index = findMaxIndex();
	if((index == -1) || (maxFound < 0.1))
	{
		ris="";
	}
	else
	{

		ris = "P - " + nomiRifiuti[index];
    }
  return ris;
}
