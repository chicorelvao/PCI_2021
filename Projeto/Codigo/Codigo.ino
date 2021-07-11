/*
 * Projeto de PCI 2021 - PL2 - Grupo 2
 * Implementação de uma máquina lavar.
 *                          26/06/2021
 */

//Bibliotecas usadas

#include <Stepper.h>
#include <LiquidCrystal_74HC595.h>
#include <IRremote.h>
#include <NewTone.h>






/*---------------------------------
 * --------Pins analógicos---------
 * --------------------------------
 */


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

//Pin do buzzer
int buzzPin = 7;

//Pins de conexão com os LEDs
int greenLed = 1;
int redLed = 0;
int blueLed = 2;

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
int clockPin = 5; // D2 ----> 11 SH_CP - shift register clock input
int dataPin = 4;  // D1 ----> 14 DS - serial data input
int latchPin = 3; // D0 ----> 12 ST_CP - storage register clock input

//Pinos do LCD para o shift register: LCDPin ---> registerPin
int registerSelect = 1; //Register Select ----> 1 - Q1 - parallel data output
int enableLCD = 3;      //Enable          ----> 3 - Q3 - parallel data output
int dataPinFour = 4;    //Data Pin 4      ----> 4 - Q4 - parallel data output
int dataPinFive = 5;    //Data Pin 5      ----> 5 - Q5 - parallel data output
int dataPinSix = 6;     //Data Pin 6      ----> 6 - Q6 - parallel data output
int dataPinSeven = 7;   //Data Pin 7      ----> 7 - Q7 - parallel data output

// Pin do sensor de IR
int IRPin = 6;







/*----------------------------------------------
 * ------------Variáveis adicionais-------------
 * ---------------------------------------------
 */

//-------------Variaveis int-------------------


//Armazena o tempo de um tipo de lavagem
int cycleDuration;

//Tecla do comando selecionada
int num;

//Variavel que contem a temperatura maxima de um programa
int temperature;



//-------------Variaveis long------------------

/*A explicação da variavel cycleTimeEnd requer alguma clareza. 
* Objetivo desta variavel é armazenar o milisegundo em que um programa de 
* lavagem vai terminar.
* O controlador do arduino, o ATmega contem 3 Timers ou "relógios internos".
* Os timers contam desde o ínicio do funcionamento do Arduino. O registadores
* designados para os timers são atualizados pelo o clock. O valor guardado na 
* memória é um long atualizado a cada milisegundo.
* Dois destes são de curta duração, que após umas horas dão reset e começam a
* contar do zero. O terceiro timer tem um período de aproximadamente 56 dias.
* A variavel é obtida somando o cycleDuration ao tempo atual (função millis).
* Ora, para contar o tempo que falta num programa é necessário saber a que 
* milisegundo este vai terminar. Subtraindo o tempo atual armazenado no 
* registador do timer ao milisegundo em que o programa vai terminar, é 
* possível obter o tempo que falta para o programa terminar.
*/
long cycleTimeEnd;

/*Cada programa tem 4 fases: lavagem, exaguamento, centrifugação e descarga.
* Cada fase tem uma fração de tempo atribuida para respeitar o tempo esolhido
* pelo o utilizador. A variavel runTime à semelhança da var cycleTimeEnd
* define o milisegundo em uma fase irá terminar.
*/
long runTime;


//-------------Variaveis boolean-------------------

//Booleana para controlos de fluxo de voltar a atrás
boolean goBack;
//Booleana para verificar se uma opcão selecionada é válida.
boolean invalidOption;

//Boolean que indica se o produto(s) para a lavagem já foram introduzidos
boolean product;

/*----------------------------------------------
 * --------Definições do motor de passo---------
 * ---------------------------------------------
 */

 
const int stepsPerRevolution = 2048; 

//Ligação do driver do motor de passo ao arduino
//IN1 ----> 11
//IN2 ----> 10
//IN3 ----> 9
//IN4 ----> 8

//Objeto stepper que controla o motor de passo
Stepper myStepper(stepsPerRevolution, inFourM, inTwoM, inThreeM, inOneM);

// -------Variáveis do motor de passo ------------
/*Conjuto de passos mais pequeno a ser usado neste programa. 
* Para manter o programa uniforme, além disso 100 passos é
* número suficientemente baixo para evitar erros nos cálculos
* de tempo restante, dado que o método .step() ocupa todo o
* tempo de cpu enquanto os passos não forem terminados.
*/
int deltaStep = 100;
//Velocidade do motor
int motorSpeed = 0;






/*-----------------------------------
 * --------Definições do LCD---------
 * ----------------------------------
 */


//https://github.com/matmunk/LiquidCrystal_74HC595
//https://playground.arduino.cc/Main/LiquidCrystal/

//Objeto que controla o LCD
LiquidCrystal_74HC595 lcd(dataPin, clockPin,latchPin, registerSelect, enableLCD, dataPinFour,dataPinFive, dataPinSix, dataPinSeven);


/*---------------------------------
 * --------Simbolos do LCD---------
 * --------------------------------
 */

/*Array que representa o simbolo do cadeado para informar o utilizador que 
  que a maquina de lavar está a funcionar e nao é possível alterar o programa.
*/
byte lock[8] = {0b00100 ,
                0b01010 ,
                0b01010 ,
                0b11111 ,
                0b11111 ,
                0b11111 ,
                0b11111 ,
                0b00000};






/*----------------------------------------
 * --------Definições do Infrared---------
 * ---------------------------------------
 */

//Objeto que permite a interação com o sensor de infra-vermelhos
IRrecv irrecv(IRPin);
decode_results results;

//https://www.pjrc.com/teensy/td_libs_IRremote.html









/*--------------------------------------------
 * --------Inicialização do programma---------
 * -------------------------------------------
 */
void setup() {

  //Definição do pin digital para cada led
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  //Definição do pin digital do sensor de Infra-vermelhos
  pinMode(IRPin, INPUT);

 //Definição do pin digital do buzzer passivo
  pinMode(buzzPin, OUTPUT);

  // Inicialização do LCD
  lcd.begin(16, 2);
  /*O simbolos são associados a um char com index para ser
  * mais facéis de imprimir estes no LCD
  */
  lcd.createChar(0,lock);

  //Inicialização do sensor de infra-vermelhos
  irrecv.enableIRIn();

 

/*--------------------------------------------
 * ----------------Apresentação-----------------
 * ---------------------------------------------
 */

   // Mensagem de inicio da máquina
  messageLCD("Bem Vindo!",3,0);
  delay(100);
  lcd.clear();
  // https://eletronicaparatodos.com/entendendo-e-controlando-motores-de-passo-com-driver-uln2003-e-arduino-roduino-board/

  
}










/*--------------------------------------------
 * ------------Fluxo do programma-------------
 * -------------------------------------------
 */


void loop() {

  
  lcd.clear();
  
  // Apresentação do menu principal e respetivas opções
  messageLCD("Menu",5,0); 
  delay(100);
  lcd.clear();

  goBack = true;
  while(goBack)
  {
    messageLCD("  Programas   Programas  ", 0, 0);
    messageLCD(" 1-Rapidos    2-Delicados", 0, 1);
    // REMOV - Delay no início da apresentação - 
    delay(100);
  
    //Deslocação do texto no ecrã, para o utilizador conseguir ver
    moveDisplay(9, 100);
    // delay na transição entre a apresentação dos programas
    delay(100);
    
    messageLCD("  Programas   Programas  ", 0, 0);
    messageLCD(" 3-Algodoes  4-Sinteticos", 0, 1); 
    // delay no início da apresentação
    delay(100);
  
    moveDisplay(9, 100);
  
    messageLCD("Selecione o programa desejado:", 0, 0);
      
    num = IRrequest();
  
    switch (num){ 
      // Opção- Rápidos  

      case 1:  
        invalidOption = true;
        while(invalidOption)
        {
          lcd.clear();
          messageLCD("      Rapidos         Rapidos      ", 0, 0);
          messageLCD(" 1-Rapido (Pre.def) 2-Rapido (20 C)", 0, 1);
          // REMOV - Delay no início da apresentação - 
          delay(10);
        
          //Deslocação do texto no ecrã, para o utilizador conseguir ver
          moveDisplay(19, 10);
          // delay na transição entre a apresentação dos programas
          delay(10);
          
          messageLCD("      Rapidos         Rapidos      ", 0, 0);
          messageLCD(" 3-Rapido (40 C)    4-Rapido (60 C)", 0, 1);
          // delay no início da apresentação
          delay(10);
        
          moveDisplay(19, 10);

          num = IRrequest();

          if(goBack)
          {
            invalidOption = false;
          }
          else
          {
            switch (num){
              // Rápido (30 min) - tecla 1
              case 1:
              
                /*
                rpm (máquina)          rpm (stepper)
                1200           ----->  18
                800            ----->  12
                */
                motorSpeed = 12;
                cycleDuration = 10;
                temperature = 30;
                cicloDeLavagem(motorSpeed, cycleDuration, temperature);
                invalidOption = false;      
                break;
        
              // Rápido (T = 20ºC) - tecla 2
              case 2: 
      
                progMaquina(12, 20, 15, 35);
                invalidOption = false;
                break;
      
              // Rápido (T = 40ºC) - tecla 3
              case 3: 
      
                progMaquina(18, 40, 15, 35);
                invalidOption = false;
                break;
      
              // Rápido (T = 60ºC) - tecla 4
              case 4: 
              
                /*
                rpm (máquina)          rpm (stepper)
                1200           ----->  18
                1000           ----->  15
                */
                progMaquina(15, 60, 15, 35);
                invalidOption = false;
                break;
    
              default:
           
                lcd.clear();
                messageLCD("Opcao invalida.", 0, 0);
                delay(1000);
            }

          }
        }
        
        break;
        
      // Opção- Delicados 
      
      case 2: 
        /*
        rpm (máquina)          rpm (stepper)
        1200           ----->  18
        600            ----->  9
        */
        progMaquina(9, 30, 45, 75);// 45 min = 3/4 h e 75 min = 1 1/4 h
        
        break;
        
      // Opção- Algodões
      
      case 3: 
        invalidOption = true;
        while(invalidOption)
        {
          lcd.clear();
          messageLCD(" Algodoes     Algodoes      Algodoes",0,0);
          messageLCD(" 1-Algodao diario 2-Algodao (225min)",0,1);
          moveDisplay(20, 500);
          num = IRrequest();
          
          if(goBack)
          {
            invalidOption = false;
          }
          else
          {
            switch (num){
              // Algodão diário - tecla 1
              case 1:
      
                progMaquina(18, 30, 165, 225);// 165 min = 2 3/4 h e 225 min = 3 3/4 h
                invalidOption = false;
                break;
      
              // Algodão (225 min) - tecla 2
              case 2:
      
                motorSpeed = 18;
                cycleDuration = 450; // 225 min na datasheet correspondem a 450s no stepper
                temperature = 40;
                cicloDeLavagem(motorSpeed, cycleDuration, temperature);
                invalidOption = false;
                break;
    
              default:
              
                lcd.clear();
                messageLCD("Opcao invalida.", 0, 0);
                delay(1000);
            }
          }
        }
          
        break;
          
      // Opção- Sintéticos
      
      case 4:
        invalidOption = true;
        while(invalidOption)
        {
          lcd.clear();
          messageLCD(" Sinteticos     Sinteticos     Sinteticos",0,0);
          messageLCD(" 1-Sintetico diario 2-Sintetico (200 min)",0,1);
          moveDisplay(25, 500);
          num = IRrequest();
          
          if(goBack)
          {
            invalidOption = false;
          }
          else
          {
            switch (num){
              // Sintético diário - tecla 1
              case 1:
      
                progMaquina(18, 30, 105, 198);// 105 min = 1 3/4 h e 225 min = 2 1/2 h
                invalidOption = false;
                break;
      
              // Sintético (200 min) - tecla 2
              case 2:
      
                motorSpeed = 18;
                cycleDuration = 360; // 200 min na datasheet correspondem a 360s no stepper
                temperature = 40;
                cicloDeLavagem(motorSpeed, cycleDuration, temperature);
                invalidOption = false;
                break;
    
              default:
              
                lcd.clear();
                messageLCD("Opcao invalida.", 0, 0);
                delay(1000);
            }
          }
        }
        break;
      
      default:
        lcd.clear();
        messageLCD("Opcao invalida.", 0, 0);
        delay(1000);
        
    }
  }
}




/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------
 *--------------------------------- Funções ----------------------------------
 *----------------------------------------------------------------------------
 *----------------------------------------------------------------------------
 */



/*--------------------------------------------------
 * ---------------- Funções do LCD------------------
 * -------------------------------------------------
 */

 /* Função para imprimir uma mensagem no LCD
  * Os argumentos são a messagem e o pixel onde começa a mensagem
  */

void messageLCD (String message,  int colsLCD, int rowLCD){
  lcd.setCursor(colsLCD, rowLCD);
  lcd.print(message);
}


/* Função que move o texto no display, para que o texto demasiado
* para o ecrã fique visível. O LCD guarda o texto que não se vê 
* na sua memória. Ou seja, o lcd real tem 16 por 2 pixeis, mas o
* o virtual tem o dobro.
* Os argumentos são até que pixeis se quer ver o LCD e o tempo 
* de espera entre cada deslocação de píxel.
*/
void moveDisplay(int maxPixel, int waitTime){

  for (int positionCounter = 0; positionCounter < maxPixel; positionCounter++) {
    lcd.scrollDisplayLeft();

    delay(waitTime);
  }

  lcd.clear();

}

void printTimeLeft(){

  long timeLeft = cycleTimeEnd - millis();
  String printTime;
  String minuteStr;
  
  long hourLeft = timeLeft/60000;

  if(hourLeft < 0) {
    hourLeft = 0;
  }

  long minuteLeft = (timeLeft - hourLeft*60000)/1000;
  
  if(minuteLeft < 0 && hourLeft == 0) {
    minuteLeft = 0 ;
  }

  if(minuteLeft < 10) {
    minuteStr = "0" + String(minuteLeft);
  } else {
    minuteStr = String(minuteLeft);
  }



  printTime = String(hourLeft) + ":" + minuteStr;
  messageLCD(printTime, 9,1);

 }




/*---------------------------------------------------------------
 * ----------------Controlo do ciclo de lavagem------------------
 * --------------------------------------------------------------
 */



void progMaquina(int motorSpeed, int temperature, int infLim, int supLim)     
{
  lcd.clear();
  int cont1 = 0;
  num = infLim;

  while(cont1 == 0 || (num < infLim || num > supLim))
  {
    cont1++;
    
    if (cont1 == 1)
    {
      messageLCD(" Insira a duracao desejada: ",0,0);
      moveDisplay(12, 500);
    }

    else
    {
      messageLCD(" Duracao introduzida invalida!",0,0);
      moveDisplay(14, 500);
      messageLCD(" Insira a duracao desejada: ",0,0);
      moveDisplay(12, 500);
    }
      
    num = IRrequest();
  }

  
  
  cicloDeLavagem(motorSpeed, num, temperature);            
}

void cicloDeLavagem(int motorSpeed, int  cycleDuration, int temperature)
{ 
  lcd.clear();
  lcd.setCursor(15,1);
  lcd.write(byte(0));
  messageLCD("A lavar...",0,0);
  digitalWrite(greenLed, HIGH);


  cycleTimeEnd = millis() + ( (long) cycleDuration)*1000;
  // as frações do tempo de duração correspondentes a cada fase do ciclo precisam de ser ajustadas com valores que façam mais sentido
  int washDuration = 0.25 * cycleDuration;
  int rinseDuration = 0.25 * cycleDuration;
  int spinDuration = 0.25 * cycleDuration;
  int drainDuration = 0.25 * cycleDuration;
  lavagem(washDuration, motorSpeed);
  lavagem(rinseDuration, (motorSpeed-5));
  
  centrifugacao(spinDuration, motorSpeed);
  descarga(drainDuration, motorSpeed);

  digitalWrite(greenLed, LOW);
  
  
    NewTone(buzzPin, 1000); // Send 1KHz sound signal...
    delay(1000);    
    noNewTone(buzzPin);     // Stop sound...   
     /*// ...for 1 sec
     tone(buzzPin, 700); // Send 1KHz sound signal...
    delay(1000);        // ...for 1 sec
     tone(buzzPin, 500); // Send 1KHz sound signal...
    delay(1000);        // ...for 1 sec
    noTone(buzzPin);     // Stop sound...
    delay(1000); */

  

}
    
// Indica se os produtos (detergente e amaciador) foram colocados
void productIn (){

  product = false;

  while(product){

    messageLCD(" Coloque o detergente e prima CH", 0, 0);
    moveDisplay(16, 500);
    num = IRrequest();
  
  // Associa-se esta função à tecla CH do telecomando
    if(num = 0xFF629D){
      product = true;      
    }

  }


}

/*
 * Métodos responsáveis pela execução do ciclo de lavagem,
 * na ordem: lavagem, enxaguamento, centrifugação e descarga.
 * 
 * A lavagem e o enxaguamento consistem na rotação do motor
 * no sentido dos ponteiros do relógio, com o motor a rodar mais
 * rapidamente na lavagem do que no enxaguamento. 
 * 
 * O método correspondente à centrifugação acelera a rotação 
 * do motor clockwise até atingir a velocidade máxima, desacelera-o
 * até parar, inverte o sentido e acelera-o counterclockwise 
 * até atingir a velocidade máxima. 
 * O método correspondente  à descarga apenas desacelera o 
 * motor até parar, dando-se depois a descarga.
 * 
 */
 

/*Função que calcula o tempo que falta para acabar uma lavagem e 
* apresentar no ecrã
*/


// Lavagem 
void  lavagem (int timeMax, int speedMov){
  
  myStepper.setSpeed(speedMov);
  deltaStep = 100;

  runTime = ((long) timeMax)*1000 + millis();

  while(runTime > millis()){
    myStepper.step(deltaStep);
    printTimeLeft();
    
  }
  
  
}


void rotationDirection(int speedMov, boolean speedUp, boolean toRight){
  
  if(toRight) {
    deltaStep = 100;
  } else {
    deltaStep = -100;
  }


  if(speedUp){

    for (int i = 3; i <= speedMov; i = i + (speedMov/3))
    {
      
    
      myStepper.setSpeed(i);
      for(int j = 0; j<= 4; j++){
        myStepper.step(deltaStep);
        printTimeLeft();
      }
      
    }
    
  } else {

    for (int i = speedMov; i > 3; i = i - (speedMov/3))
      {
        myStepper.setSpeed(i);
        
      for(int j = 0; j<= 4; j++){
        myStepper.step(deltaStep);
        printTimeLeft();
      }
        
      }

  }

}


void centrifugacao (int timeMax, int speedMov){

  runTime = ((long) timeMax)*1000 + millis();
  

  while(runTime > millis()) {

    for(int rotationType = 0; rotationType < 4; rotationType++){

      switch (rotationType){

        case 0:
          /* code */
          rotationDirection(speedMov, false, true);
          break;

        case 1:
        /* code */
          rotationDirection(speedMov, true, false);
          break;

        case 2:
          /* code */
          rotationDirection(speedMov, false, false);
          break;

        case 3:
        /* code */
          rotationDirection(speedMov, true, true);
          break;
        
        default:
          break;
      }

      if(millis() > runTime) {
        break;
      }

    }

    

  }

}


void descarga (int timeMax, int speedMov){
  messageLCD("A terminar...",0,0);
  long revSteps = 2048L;
  
  long steps;
  // o motor desacelera até parar e dá-se a descarga
  for (int i = speedMov; i >= 0; i = i - (speedMov/3))
  {
    myStepper.setSpeed(i);
    steps = revSteps * i * timeMax / (3 * 60);
    
    for(int i = 0; i < steps; i += 100){
      myStepper.step(-100);
      printTimeLeft();
    }
  }
}





/*---------------------------------------------------------------
 * ----------------Interface com Infra-Vermelhos-----------------
 * --------------------------------------------------------------
 */
int IRrequest (){
    //número que vai ser introduzido pelo o utilizador
    int number = 0; 

    results.value = 0xFF6897; // garante que entra no while, se a última tecla que o utilizador premiu foi o play
    //se formos utilizar as variáveis receiveIR e comandOption em baixo, temos de atualizá-las à medida que irrecv.decode(&results) e results.value mudam


    while(results.value != 0xFFC23D){
      if (irrecv.decode(&results)){   
       
       //Para evitar somar número negativos
       
        switch(results.value){ 
          case 0xFF22DD:
          //Tecla Rewind
            number = -3;
            messageLCD("Voltar atras?", 0, 1);

            break;

          case 0xFF629D:  
          //Tecla CH
            number = -1;
            messageLCD("Detergente colocado? Confirme.", 0, 1);
            break;
            
          case 0xFF6897:
            number = number *10 + 0;
            messageLCD(String(number), 0, 1);
            delay(1000);
            
            break;
        
          case 0xFF30CF:  
            number = number *10 + 1;
            
           
            messageLCD(String(number), 0, 1);
            delay(1000);
            break;
        
          case 0xFF18E7: 
            number = number *10 + 2;
            
            messageLCD(String(number), 0, 1);
            delay(1000);
            break;
        
          case 0xFF7A85: 
            number = number *10 + 3;
            
            messageLCD(String(number), 0, 1);
            break;
            
          case 0xFF10EF:
            number = number *10 + 4;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF38C7:  
            number = number *10 + 5;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF5AA5: 
            number = number *10 + 6;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF42BD:
            number = number *10 + 7;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF4AB5: 
            number = number *10 + 8;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF52AD: 
            number = number *10 + 9;
            messageLCD(String(number), 0, 1);
            break;
        
          default:
            Serial.print(" unknown button   ");
            Serial.println(results.value, HEX);
        }
        
        if(number > 900){
          number = 0;
          messageLCD("0     ", 0, 1);
        } 


        irrecv.resume();

      }

    }

    lcd.clear();

    /*
     * Os valores -1  e -3 devolvidos por IRrequest() foram arbitrariamente definidos para 
     * servirem de flag à colocação de detergente e à opção de voltar atrás, respetivamente.
     * Se num = -1 o detergente foi colocado e o programa segue, caso contrário o 
     * detergente não foi colocado e o programa volta a solicitar a sua inserção.
     * Se num = -3 a opção de voltar atrás foi escolhida e o programa volta à 
     * apresentação dos programas, caso contrário o programa segue normalmente
     */
     
    if(number != -1 && number != -3){
      messageLCD(String(number), 0, 1);
      delay(1000);
    }

    if(number == -1){
      product = true;
    }

    if(number == -3){
      goBack = true;
    }
    else{
      goBack = false;
    }
    
    return number;
}


              


            
