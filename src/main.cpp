#include <Arduino.h>
#include <LibRobus.h>
#include <arduino-timer.h>

void suiveurLigne();
void arreter();
void sonar();
double getRangeSonar();
double convertAnalog(double valeurBrute);
void IRcote();
void tourner(double angle, uint8_t moteur);
double distance_to_pulses(double distance);
void renverserQuille();
void avancer();
void suiveurLigne2();
void retourner_ligne();

int valSuiveurLigne[8];
bool son = false;
bool tuer_quille = false;
#define KP 0.001
#define KI 0.000001

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  BoardInit();

  for (int i = 37; i <= 45; i++)
  {
    pinMode(i, INPUT);
  }

  pinMode(46, OUTPUT); //sonar
  pinMode(47, OUTPUT); //jaune
  pinMode(48, OUTPUT); //bleu
  pinMode(49, OUTPUT); //rouge
  pinMode(53, OUTPUT); //vert

  digitalWrite(47, HIGH);
  digitalWrite(48, HIGH);
  digitalWrite(49,HIGH);
  digitalWrite(53,HIGH);

  delay(1000);

  
}

void loop() {
  for (int i = 37; i <= 44; i++)
  {
    valSuiveurLigne[i-37] = digitalRead(i);
  }
  for (int i = 0; i < 8; i++)
  {
    //Serial.print(valSuiveurLigne[i]);
  }
 // Serial.println();
  //Serial.println(analogRead(A11));
  suiveurLigne();

  if (analogRead(A11)>500){
    digitalWrite(53, LOW);
    son = true;
  }
  if (son == true){
    digitalWrite(47, LOW);
    digitalWrite(48, LOW);
    digitalWrite(49,LOW);
    renverserQuille();
  }


}

void suiveurLigne() {
  MOTOR_SetSpeed(LEFT,0.2);
  MOTOR_SetSpeed(RIGHT,0.2);

  if (valSuiveurLigne[3] == 0 && valSuiveurLigne[4] == 0 )
  {
    MOTOR_SetSpeed(RIGHT,0.2);
    MOTOR_SetSpeed(LEFT,0.2);
  }
  
  else if(valSuiveurLigne[0] == 0 || valSuiveurLigne[1] == 0 || valSuiveurLigne[2] == 0 ) {
    MOTOR_SetSpeed(RIGHT, 0.2);
    MOTOR_SetSpeed(LEFT, 0);
  }
  else if(valSuiveurLigne[6] == 0 ||valSuiveurLigne[7] == 0 || valSuiveurLigne[5] == 0) { 
    MOTOR_SetSpeed(RIGHT, 0);
    MOTOR_SetSpeed(LEFT, 0.2);
  }
    else if(valSuiveurLigne[0] == 1 && valSuiveurLigne[1] ==1 && valSuiveurLigne[2] ==1 && valSuiveurLigne[3] ==1 && valSuiveurLigne[4] ==1 && valSuiveurLigne[5] ==1 && valSuiveurLigne[6]==1 && valSuiveurLigne[7]==1 ){
     MOTOR_SetSpeed(LEFT, 0.2);
    } 
  delayMicroseconds(50);
  }

  void arreter()
  {
  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT, 0);
  }


/*void sonar(){ //ce qu'on fait avec le sonar
  double distanceSonar = getRangeSonar();
}*/

double getRangeSonar(){
  digitalWrite(46, LOW);    // Set the trigger pin to low for 2uS
  delay(10);
  digitalWrite(46, HIGH);   // Send a 10uS high to trigger ranging
  delayMicroseconds(10);
  digitalWrite(46, LOW);    // Send pin low again
  double range = pulseIn(45, HIGH)*0.17/10.0;

  return range;
}

double convertAnalog(double valeurBrute){
  return valeurBrute*5/1024;
}

/*void IRcote(){
  int lectureIR0 = analogRead(A0);
  double voltageIR0 = convertAnalog(lectureIR0);
  if(voltageIR0 > 1.2 ){ //environ 25cm
    Serial.println("Doit s'arreter!!!!! <25cm ");
    //arreter();
  }
}*/

void tourner(double angle, uint8_t moteur){
  MOTOR_SetSpeed(LEFT, 0);
  MOTOR_SetSpeed(RIGHT,0);
  double distance = (angle*119.380521)/360.0;
  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);
  double pulses_voulus = distance_to_pulses(distance);
  int nb_pulses_tot = 0;
  MOTOR_SetSpeed(moteur, 0.3);

  while(nb_pulses_tot < pulses_voulus){
    delay(50);
    nb_pulses_tot += ENCODER_ReadReset(moteur);
  }
  arreter();
}

double distance_to_pulses(double distance)
{
  double pulses = (distance*3200.0)/23.938936;
  return pulses;
}

void renverserQuille(){
  Serial.println(getRangeSonar());
    if (getRangeSonar()< 40.0){
      tuer_quille=true;
    }
    if (tuer_quille==true){
      tourner(45, LEFT);
      tourner(40, LEFT);
      tuer_quille = false;
      son = false;
      delay(1000);
      retourner_ligne();
      suiveurLigne2();
    }
}

void avancer(){
  int16_t erreur = 0;
	int32_t sommeErreur = 0;
	//uint32_t moyenneEnco = 0;
	float consigneVitesse = 0.05;
	//uint32_t pourcPulse = 0;

  ENCODER_Reset(LEFT);
  ENCODER_Reset(RIGHT);

	MOTOR_SetSpeed(LEFT, consigneVitesse);
	MOTOR_SetSpeed(RIGHT, consigneVitesse);
	
		//lecture des encos
		int32_t encoGauche = ENCODER_Read(LEFT);
		int32_t encoDroite = ENCODER_Read(RIGHT);

		erreur = encoDroite - encoGauche;
		sommeErreur += erreur;
		float ajustement = erreur * KP + sommeErreur * KI;
		float consigneVitesseAdj = consigneVitesse + ajustement;
		MOTOR_SetSpeed(LEFT,consigneVitesseAdj);
		MOTOR_SetSpeed(RIGHT,consigneVitesse);
	  //moyenneEnco = (ENCODER_Read(LEFT)+ENCODER_Read(RIGHT))/2;

 }

void retourner_ligne()
{
   
   if (valSuiveurLigne[0]==0 || valSuiveurLigne[1]==0 || valSuiveurLigne[2]==0 || valSuiveurLigne[3]==0 || valSuiveurLigne[4]==0 || valSuiveurLigne[5]==0 || valSuiveurLigne[6]==0 || valSuiveurLigne[7]==0) 
   {
      arreter();
      tourner(45, LEFT);
      tourner(40,LEFT);
   }
   else
   {
      avancer();
      delay(100);
      retourner_ligne();
   }
}

void suiveurLigne2() {
  MOTOR_SetSpeed(LEFT,0.2);
  MOTOR_SetSpeed(RIGHT,0.2);

  if (valSuiveurLigne[3] == 0 && valSuiveurLigne[4] == 0 )
  {
    MOTOR_SetSpeed(RIGHT,0.2);
    MOTOR_SetSpeed(LEFT,0.2);
  }
  
  else if(valSuiveurLigne[0] == 0 || valSuiveurLigne[1] == 0 || valSuiveurLigne[2] == 0 ) {
    MOTOR_SetSpeed(RIGHT, 0.2);
    MOTOR_SetSpeed(LEFT, 0);
  }
  else if(valSuiveurLigne[6] == 0 ||valSuiveurLigne[7] == 0 || valSuiveurLigne[5] == 0) { 
    MOTOR_SetSpeed(RIGHT, 0);
    MOTOR_SetSpeed(LEFT, 0.2);
  }
  else if(valSuiveurLigne[0] == 1 && valSuiveurLigne[1] ==1 && valSuiveurLigne[2] ==1 && valSuiveurLigne[3] ==1 && valSuiveurLigne[4] ==1 && valSuiveurLigne[5] ==1 && valSuiveurLigne[6]==1 && valSuiveurLigne[7]==1 ){
     MOTOR_SetSpeed(LEFT, 0.2);
  }
  else if(valSuiveurLigne[0] == 0 && valSuiveurLigne[1] == 0 && valSuiveurLigne[2] == 0 && valSuiveurLigne[3] == 0 && valSuiveurLigne[4] == 0){
    tourner(45, RIGHT);
    tourner(45, RIGHT);
  }
  delayMicroseconds(50);
  }