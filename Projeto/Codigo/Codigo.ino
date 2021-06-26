/*
 * Projeto de PCI 2021 - PL2 - Grupo 2
 * Implementação de uma máquina lavar.
 *                          26/06/2021
 */

//bibliotecas usadas
#include <Stepper.h>
#include <LiquidCrystal_74HC595.h>





/*---------------------------------
 * --------Pins analógicos---------
 * --------------------------------
 */
int potentAlog = A0;
int tempAlog = A1;

/*---------------------------------
 * --------Pins digitais-----------
 * --------------------------------
 */
//Conexões com o motor de passo
int inOneM = 11;
int inTwoM = 10;
int inThreeM = 9;
int inFourM = 8;

//Pin do botão de input variado
int buttonPin;

//Pin do buzzer
int buzzPin;

//Pins de conexão com os LEDs
int greenLed;
//o power led é vermelho
int powerLed;
int redLed;
int blueLed;

/*
 * Pins de conexão com o LCD, que vai ser utilizado com o
 * shift register. A implicação disto é que deixamos usar
 * o LCD com um BUS de 8 bits paralelo, para um BUS em 
 * série de 1 bit. Daí ser necessário um pin para dar
 * informação ao shift register sobre o clock do 
 * microcontrolador. Irá ser utilizada uma biblioteca 
 * específica para controlar o LCD com o shift register.
 */
//Pinos do shift register para o arduino: ArduinoPin ---> registerPin
int clockPin = 2; // D2 ----> 11 SH_CP - shift register clock input
int dataPin = 1;  // D1 ----> 14 DS - serial data input
int latchPin = 0; // D0 ----> 12 ST_CP - storage register clock input

//Pinos do LCD para o shift register: LCDPin ---> registerPin
int registerSelect = 1; //Register Select ----> 1 - Q1 - parallel data output
int enableLCD = 3;      //Enable          ----> 3 - Q3 - parallel data output
int dataPinFour = 4;    //Data Pin 4      ----> 4 - Q4 - parallel data output
int dataPinFive = 5;    //Data Pin 5      ----> 5 - Q5 - parallel data output
int dataPinSix = 6;     //Data Pin 6      ----> 6 - Q6 - parallel data output
int dataPinSeven = 7;   //Data Pin 7      ----> 7 - Q7 - parallel data output

/*----------------------------------------------
 * --------Definições do motor de passo---------
 * ---------------------------------------------
 */

 
const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// for your motor

//IN1 ----> 11
//IN2 ----> 10
//IN3 ----> 9
//IN4 ----> 8
Stepper myStepper(stepsPerRevolution, inFourM, inThreeM, inTwoM, inOneM);


/*-----------------------------------
 * --------Definições do LCD---------
 * ----------------------------------
 */
//https://github.com/matmunk/LiquidCrystal_74HC595
//https://playground.arduino.cc/Main/LiquidCrystal/


LiquidCrystal_74HC595 lcd(dataPin, clockPin,latchPin, registerSelect, enablePin, dataPinFour,dataPinFive, dataPinSix, dataPinSeven);



/*--------------------------------------------
 * --------Inicialização do programma---------
 * -------------------------------------------
 */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(20, 4);

  //definição do pin digital para cada led
  pinMode(greenLed, OUTPUT);
  pinMode(powerLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  //definição do pin digital do butão de input variado
  pinMode(buttoPin, INPUT);
 //definição do pin digital do buzzer passivo
  pinMode(buzzPin, OUTPUT);
  
}


void loop() {
  // put your main code here, to run repeatedly:
  
}
