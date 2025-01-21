const int stopImpianto = 3; //Pulsante di Start

//Pin di acquisizione dei segnali d'ingresso provenienti dai sensori
const int              pinAnemometro = A0;
const int     pinSensoreCrepuscolare = A1;
const int      pinConcentrazionePM10 = A2;
const int pinConcentrazioneMonossido = A3;
const int          pinEncoderTurbina = 2;
//Pin di controllo delle interfacce di potenza
const int pin_controllo_Lampada = 9;          //Analogico tipo PWM
const int pin_controllo_pinza_frenatura = 4;  //Digitale tipo ON/OFF

//contatori impulsi encoder
volatile int conta_impulsi_Turbina= 0;
//Variabile di segnalazione frenatura attiva 
bool frenaturaTurbina = true;


//Strutture dati per la memorizzazione e la gestione dei dati acquisiti
int i = 0; 
int PM10[96];
int CO[96];

float sommaPM10 = 0;
float   sommaCO = 0;
float mediaPM10 = 0;
float   mediaCO = 0;

long int ultimoCampionamentoAmbientale = 0;
long int timeout_sensori_ambientali = 5*60*1000; // una rilevazione ogni 5 minuti espressa in millesimi di secondo
long int ultimoCampionamentoAnemometro = 0;
long int timeout_anemometro = 5000; // una rilevazione ogni 5 secondi espressa in millesimi di secondo

void setup() {

Serial.begin(9600);
Serial.println("Imposto il microcontrollore...");

//Impostazione dei piedini d'ingresso
pinMode(stopImpianto, INPUT);
pinMode(pinAnemometro, INPUT);
pinMode(pinSensoreCrepuscolare, INPUT);
pinMode(pinConcentrazionePM10, INPUT);
pinMode(pinConcentrazioneMonossido, INPUT);

//Impostazione dei piedini di uscita
pinMode(pin_controllo_Lampada, OUTPUT);
pinMode(pin_controllo_pinza_frenatura, OUTPUT);

Serial.println("Inizio monitoraggio ambientale e impianto...");
//Imposto la variabile per la gestione dei tempi di campionamento dei dati ambientali (nel caso di sblocco della turbina prima di fine setup)
ultimoCampionamentoAmbientale= millis();
ultimoCampionamentoAnemometro = millis();
}

void loop() {

//================================================================================================================================
//suppongo un pulsante con pull-up resistor -> se non premuto il livello di tensione sul piedino è HIGH e resto bloccato nel ciclo
//================================================================================================================================

while(digitalRead(stopImpianto)) {
bool frenaturaTurbina = true;
Serial.println("Impianto fermo...");

//alla partenza contiene i riferimenti temporali corretti di inizio monitoraggio
ultimoCampionamentoAmbientale= millis();
ultimoCampionamentoAnemometro = millis();

}

//====================================
//Inizio il monitoraggio dell'impianto
//====================================

bool frenaturaTurbina = false;
Serial.println("Impianto in funzione...");

//Sblocco il freno della turbina eolica
digitalWrite(pin_controllo_pinza_frenatura, LOW);


//========================================
//Controllo della luminosità della lampada 
//========================================


//controllo lo stato della luminosità ambiente 
int lum_numerica = analogRead(pinSensoreCrepuscolare); 

//Se lum_numerica = 0 ho il massimo di luminosità (la tensione del sensore ha legge inversa, tensione maggiore per luminosità minore)
float lux = 20 - (lum_numerica /1023.0)*20; 

//Controllo PWM (pulse width modulation) della lampada

if (lux > 10) analogWrite(pin_controllo_Lampada, 0); //giorno -> lampada spenta
if ( lux<10 && lux>0.1 ) 
  analogWrite(pin_controllo_Lampada, 128); //Crepuscolo -> mezza potenza
  else
  analogWrite(pin_controllo_Lampada, 256); //Notte -> potenza massima



//=====================================================
//Monitoraggio ambientale e raccolta dati ogni 5 minuti
//=====================================================


if ( millis() - ultimoCampionamentoAmbientale> timeout_sensori_ambientali) {
    int PM10_numerico = analogRead(pinConcentrazionePM10);  
    int   CO_numerico = analogRead(pinConcentrazioneMonossido);  
     
    PM10[i] = (PM10_numerico/1023.0) * 100; //Supponendo che al 100% di concentrazione la tensione in ingresso al piedino sia massima
    CO[i] = (CO_numerico/1023.0) * 100; //Supponendo che al 100% di concentrazione la tensione in ingresso al piedino sia massima

    i++; //aggiorno l'indice dei dati raccolti
    ultimoCampionamentoAmbientale = millis(); //Aggiorno il riferimento temporale dell'ultima rilevazione
    
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

  //======================================================
  //Controllo della velocità della turbina ogni 5 secondi
  //======================================================

  if ( millis() - ultimoCampionamentoAnemometro>timeout_anemometro) {
    int   windSpeed_numerica =  analogRead(pinAnemometro); 
    int            windSpeed =  (windSpeed_numerica/1023.0) * 60;
    
    //frenatura digitale con isteresi

    if ( windSpeed > 30 ) digitalWrite(pin_controllo_pinza_frenatura, HIGH);
    else
      if ( windSpeed < 28 ) digitalWrite(pin_controllo_pinza_frenatura, LOW);
  
    ultimoCampionamentoAnemometro = millis();
  }

}
