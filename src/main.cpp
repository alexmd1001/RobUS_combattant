#include <Arduino.h>
#include <LibRobus.h>

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

int valSuiveurLigne[8];
bool son = false;
bool tuer_quille = false;

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

  digitalWrite(47, LOW);
  digitalWrite(48, LOW);
  digitalWrite(49,LOW);
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
  delay(100);
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
    if (getRangeSonar()< 20.0){
      tuer_quille=true;
    }
    if (tuer_quille==true){
      tourner(45, LEFT);
      tourner(40, LEFT);
      avancer();
      tuer_quille = false;
    }
}

void avancer(){
  MOTOR_SetSpeed(LEFT,0.25);
  MOTOR_SetSpeed(RIGHT, 0.25);
 }