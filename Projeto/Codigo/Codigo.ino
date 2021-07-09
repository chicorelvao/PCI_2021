

/*
 * Projeto de PCI 2021 - PL2 - Grupo 2
 * Implementação de uma máquina lavar.
 *                          26/06/2021
 */

//bibliotecas usadas

#include <Stepper.h>
#include <LiquidCrystal_74HC595.h>
#include <IRremote.h>
#include <NewTone.h>



/*---------------------------------
 * --------Simbolos do LCD---------
 * --------------------------------
 */
byte lock[8] = {0b00100 ,
                0b01010 ,
                0b01010 ,
                0b11111 ,
                0b11111 ,
                0b11111 ,
                0b11111 ,
                0b00000};













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
//o power led é vermelho
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


//Tecla do comando selecionada

int num; // numero final comando
int temperature;
int rotations;




//Boolean que indica se o produto(s) para a lavagem já foram introduzidos
boolean product;










/*----------------------------------------------
 * ------------Variáveis adicionais-------------
 * ---------------------------------------------
 */

// Tempo do ciclo de lavagem
int timeCounter;
int timeMax;
boolean timeCheck = false;
int cycleDuration;

 //Conta o número de dígitos inseridos pelo utilizador no LCD
int cont1;
 //Variáveis envolvidas na atualização do número final no comando
int tempNum;
int oneDig;
int twoDig;
int threeDig;
int fourDig;


long cycleTime;
long cycleTimeEnd;
long runTime;


boolean goBack;

boolean invalidOption;



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
Stepper myStepper(stepsPerRevolution, inFourM, inTwoM, inThreeM, inOneM);

// Variáveis do motor de passo 

int stepCount = 0;  // number of steps the motor has taken
int deltaStep = 100;
int motorSpeed = 0;
int halfSpeed = 0;
int halfPotentiometer = 511; //ponto médio do potenciometrio
int sensorReading = 0;
//int speedI = 64;

// Tempo do ciclo de lavagem
/*
 * Contém o valor do tempo do ciclo de lavagem (timeMax);
 * Conta o tempo que falta para acabar a lavagem (timeCounter);
 * Verifica se o tempo já terminou.
 * Para fins demonstrativos utilizamos a seguinte escala temporal:
 * 30 min -----> 1 min
 */


/*-----------------------------------
 * --------Definições do LCD---------
 * ----------------------------------
 */


//https://github.com/matmunk/LiquidCrystal_74HC595
//https://playground.arduino.cc/Main/LiquidCrystal/


LiquidCrystal_74HC595 lcd(dataPin, clockPin,latchPin, registerSelect, enableLCD, dataPinFour,dataPinFive, dataPinSix, dataPinSeven);









/*----------------------------------------
 * --------Definições do Infrared---------
 * ---------------------------------------
 */
IRrecv irrecv(IRPin);
decode_results results;

//Boolean que indica se um código IR foi recebido (true) ou não (false)
//https://www.pjrc.com/teensy/td_libs_IRremote.html
boolean receiveIR;


int comandOption = results.value;








/*--------------------------------------------
 * --------Inicialização do programma---------
 * -------------------------------------------
 */
void setup() {
  // put your setup code here, to run once:


  //definição do pin digital para cada led
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  //definição do pin digital do sensor de IV
  pinMode(IRPin, INPUT);

 //definição do pin digital do buzzer passivo
  pinMode(buzzPin, OUTPUT);

  // Inicialização do LCD
  lcd.begin(16, 2);

  //Inicialização do sensor de infra-vermelhos
  irrecv.enableIRIn();

  lcd.createChar(0,lock);

  /*--------------------------------------------
 * ----------------Apresentação-----------------
 * ---------------------------------------------
 */

   // Mensagem de inicio da máquina
  messageLCD("Bem Vindo!",3,0);
  delay(100);
  lcd.clear();


  
  // Velocidade inicial do motor (MAX 100)
  // https://eletronicaparatodos.com/entendendo-e-controlando-motores-de-passo-com-driver-uln2003-e-arduino-roduino-board/

  
}









/*--------------------------------------------
 * ------------Fluxo do programma-------------
 * -------------------------------------------
 */


void loop() {

  
  lcd.clear();

  product = false;
  while(!product){
    messageLCD(" Coloque o detergente e prima CH", 0, 0);
    moveDisplay(16, 500);
    num = IRrequest();
  }
  
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




/*--------------------------------------------
 * ---------------- Funções ------------------
 * -------------------------------------------
 */

 /* Função para imprimir uma mensagem no LCD
  */

void messageLCD (String message,  int colsLCD, int rowLCD){
  lcd.setCursor(colsLCD, rowLCD);
  lcd.print(message);
}

void moveDisplay(int maxPixel, int waitTime){
  for (int positionCounter = 0; positionCounter < maxPixel; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(waitTime); //delay pequeno para testar rapidamente
  }
  lcd.clear();

}

// Indica se os produtos (detergente e amaciador) foram colocados
boolean productIn (int comandOption, boolean product){
  
  // Associa-se esta função à tecla CH do telecomando
  if(comandOption = 0xFF629D){
    product = true;      
  }
  return product;
}




/*
 * Métodos responsáveis pela execução do ciclo de lavagem,
 * na ordem: lavagem, enxaguamento, centrifugação e descarga.
 * A lavagem e o enxaguamento consistem na rotação do motor
 * no sentido dos ponteiros do relógio, com o motor a rodar mais
 * rapidamente na lavagem do que no enxaguamento. O método
 * correspondente à centrifugação acelera a rotação do motor 
 * clockwise até atingir a velocidade máxima, desacelera-o
 * até parar, inverte o sentido e acelera-o counterclockwise 
 * até atingir a velocidade máxima. O método correspondente 
 * à descarga apenas desacelera o motor até parar, dando-se 
 * depois a descarga.
 * 
 * O número de steps correspondentes a um dado programa 
 * foram obtidos através da velocidade de rotação do motor e 
 * do tempo de execução do programa da seguinte forma:
 * Considerando que a velocidade de rotação do motor são
 * 18 rpm e o tempo introduzido pelo utilizador são 45 min, 
 * o fator de conversão dita que 45 min --> 1,5 min e tem-se
 * 
 * tempo (s)         rotações
 * 60        ----->  18
 * 90        ----->  27
 * 
 * rotações          steps
 * 1         ----->  2048
 * 27        ----->  55296
 * 
 * Logo, a um programa de 45 min, corresponde uma demonstração
 * de 1,5 min com 55296 steps.
 */
 

/*Função que calcula o tempo que falta para acabar uma lavagem e 
* apresentar no ecrã
*/

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


int IRrequest (){
    //número que vai ser introduzido pelo o utilizador
    int number = 0; 

    results.value = 0xFF6897; // garante que entra no while, se a última tecla que o utilizador premiu foi o play
    //se formos utilizar as variáveis receiveIR e comandOption em baixo, temos de atualizá-las à medida que irrecv.decode(&results) e results.value mudam


    while(results.value != 0xFFC23D){
      
      if (irrecv.decode(&results)){   
       
        switch(results.value){ 
          case 0xFF22DD:
            Serial.println(" FAST REWIND    ");  
            number = -3;

            break;
              
          case 0xFFC23D:  
            Serial.println(" PLAY/PAUSE     "); 

            break;

          case 0xFF629D:  
            Serial.println(" CH             ");
            number = -1;
            
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

void progMaquina(int motorSpeed, int temperature, int infLim, int supLim)     
{
  lcd.clear();
  cont1 = 0;
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
    
    
              


            
