/*
 * Projeto de PCI 2021 - PL2 - Grupo 2
 * Implementação de uma máquina lavar.
 *                          26/06/2021
 */

//bibliotecas usadas
#include <Stepper.h>
#include <LiquidCrystal_74HC595.h>
#include <IRremote.h>
#include "SevSeg.h"
SevSeg sevseg; 



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

// Pin do sensor de IR
int pin = 7;
IRrecv irrecv(pin);
decode_results results;

//Boolean que indica se um código IR foi recebido (true) ou não (false)
//https://www.pjrc.com/teensy/td_libs_IRremote.html
boolean reciveIR = irrecv.decode(&results);

//Tecla do comando selecionada
int comandOption = results.value;

//Boolean que indica se o produto(s) para a lavagem já foram introduzidos
boolean product = false;


//Pin do botão de input variado
int buttonPin;

//Pin do buzzer
int buzzPin;

//Pins de conexão com os LEDs
int greenLed = 4;
//o power led é vermelho
int powerLed = 5;
int redLed = 6;
int blueLed = 7;

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
 * ------------Variáveis adicionais-------------
 * ---------------------------------------------
 */
 //Guarda o estado do butão para ligar e desligar a máquina
boolean buttonOnState = false;

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

// Variáveis do motor de passo 

int stepCount = 0;  // number of steps the motor has taken
int motorSpeed = 0;
int halfPotentiometer = 511; //ponto médio do potenciometrio
int sensorReading = 0;

//Velocidades minimas e máximas obtidas empiricamente
//O motor nao roda para outras velocidade em 64 passos por rotação completa
int minSpeed;
int maxSpeed;

/*-----------------------------------
 * --------Definições do LCD---------
 * ----------------------------------
 */
//https://github.com/matmunk/LiquidCrystal_74HC595
//https://playground.arduino.cc/Main/LiquidCrystal/


LiquidCrystal_74HC595 lcd(dataPin, clockPin,latchPin, registerSelect, enableLCD, dataPinFour,dataPinFive, dataPinSix, dataPinSeven);



/*--------------------------------------------
 * --------Inicialização do programma---------
 * -------------------------------------------
 */
void setup() {
  // put your setup code here, to run once:


  //definição do pin digital para cada led
  pinMode(greenLed, OUTPUT);
  pinMode(powerLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  //definição do pin digital do butão de input variado
  pinMode(buttonPin, INPUT);
 //definição do pin digital do buzzer passivo
  pinMode(buzzPin, OUTPUT);

  // Inicialização do LCD
  lcd.begin(16, 2);
  lcd.print("Bem Vindo!");
  delay(1000);
  lcd.clear();
  
}

void messageLCD (string message, colsLCD, rowLCD){
  lcd.setCursor(colsLCD, rowLCD);
  lcd.print(message);
}

// Indica se os produtos (detergente e amaciador) foram colocados
boolean productIn (int comandOption, boolean product){
  
  // Associa-se esta função à tecla CH do telecomando
  if(comandOption = 0xFF629D){
    product = true;      
  }
  return product;
}

void motorStep (){



}

void loop() {
  // put your main code here, to run repeatedly:
  //buttonOnState = checkButton();
  while (buttonOnState == true){
    
    // Mensagem de inicio da máquina
    messageLCD("Bem Vindo!",0,1);
    delay(500);
    lcd.clear();

    // Apresentação do menu principal e respetivas opções
    messageLCD("Menu       Menu     Menu     Menu  Menu ",0,0); 
    messageLCD("Programas: 1-Rápidos  2-Delicados  3-Algodões  4-Sintéticos",0,1);
    delay(500);
    lcd.scrollDisplayLeft();
    delay(100);
   
    if (reciveIR)
    {
      
      switch (comandOption){
        
           // Opção - Rápidos    
           case 0xFF30CF:  
              lcd.clear();
              messageLCD("Rápidos       Rápidos     Rápidos     Rápidos  Rápidos ",0,0);
              messageLCD("1-Rápido (P.def) 2-Rápido ()",0,0);
              
              
              switch (comandOption){
                
                // Rápido (30 min)
                case 0xFF30CF:
                product = productIn (comandOption, product);
                if(product == true){
                  
                }
              
              }
           break;
        
          case 0xFF18E7: 
            cont1++;  
            Serial.println(" 2              ");
            num = updateNum(2);
            Serial.println(num);
            break;
        
          case 0xFF7A85: 
            cont1++; 
            Serial.println(" 3              ");
            num = updateNum(3);
            Serial.println(num);
            break;
            
          case 0xFF10EF:
            cont1++;   
            Serial.println(" 4              ");
            num = updateNum(4);
            Serial.println(num);
            break;
        
          case 0xFF38C7:  
            cont1++; 
            Serial.println(" 5              ");
            num = updateNum(5);
            Serial.println(num);
            break;
        
          case 0xFF5AA5: 
            cont1++; 
            Serial.println(" 6              ");
            num = updateNum(6);
            Serial.println(num);
            break;
        
          case 0xFF42BD:
            cont1++;    
            Serial.println(" 7              ");
            num = updateNum(7);
            Serial.println(num);
            break;
        
          case 0xFF4AB5: 
            cont1++;   
            Serial.println(" 8              ");
            num = updateNum(8);
            Serial.println(num);
            break;
        
          case 0xFF52AD: 
            cont1++;   
            Serial.println(" 9              ");
            num = updateNum(9);
            Serial.println(num);
            break;
        
          default:
            Serial.print(" unknown button   ");
            Serial.println(results.value, HEX);
        }
      }
      
    }

  }




  lcd.setCursor(0, 0);
  lcd.print("Menu       Menu     Menu     Menu  Menu ");
  lcd.print("1-Lavagens  2-Cenfriguar 3-Torcer   4-EN");
  delay(500);
  lcd.scrollDisplayLeft();
  delay(100);


  
  /*
  lcd.print("Menu:");
  lcd.setCursor(0, 1);
  lcd.print("2-Lavagens");
  delay(1000);
    lcd.clear();
*/
  
}
