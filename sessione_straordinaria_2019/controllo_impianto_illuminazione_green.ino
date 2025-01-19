const int startImpianto = 3; //Pulsante di Start

//Pin di acquisizione dei segnali d'ingresso provenienti dai sensori

const int pinAnemometro = AN0;
const int pinSensoreCrepuscolare = AN1;
const int pinConcentrazionePM10 = AN2;
const int pinConcentrazioneMonossido = AN3;

const int pinEncoderTurbina = 2;


//Pin di controllo delle interfacce di potenza


const int pin_controllo_Lampada = 9;          //Analogico tipo PWM
const int pin_controllo_pinza_frenatura = 4;  //Digitale tipo ON/OFF


//contatori impulsi encoder

volatile int conta_impulsi_Turbina= 0;

//Variabile di segnalazione frenatura attiva 

bool frenaturaTurbina = true;


//Strutture dati per la memorizzazione e la gestione dei dati acquisiti
int i = 0; 
float PM10[96]= {0};
float   CO[96]= {0};
float sommaPM10 = 0;
float   sommaCO = 0;
float mediaPM10 = 0;
float   mediaCO = 0;

long int previousTime = 0;

long int timeout = 5*60*1000; // una rilevazione ogni 5 minuti espressa in millesimi di secondo

/**************************************************************************/

//Ad ogni impulso dell'encoder incremento il contatore degli impulsi 
//FUnzione di CallBack
void incrementa_impulsi_encoder_turbina()
{

     conta_impulsi_Turbina++;
}



void setup() {

Serial.begin(9600);
Serial.println("Imposto il microcontrollore...");

//Impostazione dei piedini d'ingresso
pinMode(startButtonPin, INPUT);
pinMode(pinAnemometro, INPUT);
pinMode(pinSensoreCrepuscolare, INPUT);
pinMode(pinConcentrazionePM10, INPUT);
pinMode(pinConcentrazioneMonossido, INPUT);

//Impostazione dei piedini di uscita
pinMode(pin_controllo_Lampada, OUTPUT);
pinMode(pin_controllo_pinza_frenatura, OUTPUT);


//Supponendo l'uscita dell' encoder di tipo open Collector attivo il resistore di PULLUP del piedino

pinMode(pinEncoderTurbina, INPUT_PULLUP);

//Registro il piedino diinterrupt, la ISR Interrupt Service Routine e il trigger dell'interrupt
attachInterrupt(digitalPinToInterrupt(pinEncoderTurbina ), incrementa_impulsi_encoder_turbina, RISING);


Serial.println("Inizio monitoraggio ambientale e impianto...");
(//Imposto la variabile per la gestione dei tempi di campionamento dei dati ambientali
previousTime= millis();
}

void loop() {

///suppongo un pulsante con pull-up resistor -> se non premuto il livello di tensione sul piedino è HIGH e resto bloccato nel ciclo
  
while(!digitalRead(startImpianto)) {
bool frenaturaTurbina = true;
Serial.println("Impianto fermo...");
}


//=================================================================================================================
bool frenaturaTurbina = false;
Serial.println("Impianto in funzione...");
//Sblocco il freno della turbina eolica
digitalWrite(pin_controllo_pinza_frenatura, LOW);


if ( millis() - previousTime> timeout) {
    int PM10 = analogRead(pinConcentrazionePM10);  
    int   CO = analogRead(pinConcentrazioneMonossido);  
     
    PM10[i] = (PM10/1023.0) * 100; //Supponendo che al 100% di concentrazione la tensione in ingresso al piedino sia massima
      CO[i] = (  CO/1023.0) * 100; //Supponendo che al 100% di concentrazione la tensione in ingresso al piedino sia massima

    i++; //aggiorno l'indice dei dati raccolti
    previousTime = millis(); //Aggiorno il riferimento temporale dell'ultima rilevazione
    
}
 

 if (i == 96) //Sono passate 8 ore (5 minuti per 96 rilevazioni)
 {
  for (int k = 0; k<96 ; k++){
    sommaPM10 = sommaPM10 + PM10[k];
      sommaCO = sommaCO + CO[k];
  }

  mediaPM10 =  sommaPM10/96; 
  mediaCO   =    sommaCO/96;
  Serial.print ("Concentrazione PM10 ultime 8 ore: ");
  Serial.print (mediaPM10);
  Serial.print ("Concentrazione CO ultime 8 ore: ");
  Serial.print (mediaCO);
  i = 0; //inizia un nuovo ciclo di 8 ore di rilevazioni dei dati ambientali, una ogni 5 minuti
  }
  


}
