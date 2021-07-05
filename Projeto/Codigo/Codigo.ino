/*
 * Projeto de PCI 2021 - PL2 - Grupo 2
 * Implementação de uma máquina lavar.
 *                          26/06/2021
 */

//bibliotecas usadas
#include <Stepper.h>
#include <LiquidCrystal_74HC595.h>
#include <IRremote.h>





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

//Pin do buzzer
int buzzPin;

//Pins de conexão com os LEDs
int greenLed = 4;
//o power led é vermelho
int powerLed = 5;
int redLed = 6;
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

long num; // numero final comando
int temperature;
int rotations;
int tempoFuncionamento;

//Boolean que indica se o produto(s) para a lavagem já foram introduzidos
boolean product = false;










/*----------------------------------------------
 * ------------Variáveis adicionais-------------
 * ---------------------------------------------
 */

// Tempo do ciclo de lavagem
int timeCounter;
int timeMax;
boolean timeCheck = false;

 //Conta o número de dígitos inseridos pelo utilizador no LCD
int cont1;
 //Variáveis envolvidas na atualização do número final no comando
int tempNum;
int oneDig;
int twoDig;
int threeDig;
int fourDig;






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
int speedI = 64;







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
  pinMode(powerLed, OUTPUT);
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


  /*--------------------------------------------
 * ----------------Apresentação-----------------
 * ---------------------------------------------
 */

   // Mensagem de inicio da máquina
  messageLCD("Bem Vindo!",3,0);
  delay(2000);
  lcd.clear();

  // Apresentação do menu principal e respetivas opções
  messageLCD("Menu",5,0); 
  delay(2000);
  lcd.clear();


  
  // Velocidade inicial do motor (MAX 100)
  // https://eletronicaparatodos.com/entendendo-e-controlando-motores-de-passo-com-driver-uln2003-e-arduino-roduino-board/
  myStepper.setSpeed(speedI);


  
}









/*--------------------------------------------
 * ------------Fluxo do programma-------------
 * -------------------------------------------
 */


void loop() {

  
  lcd.clear();
  messageLCD("  Programas   Programas  ", 0, 0);
  messageLCD(" 1-Rapidos    2-Delicados", 0, 1);
  // REMOV - Delay no início da apresentação - 
  delay(100);

  //Deslocação do texto no ecrã, para o utilizador conseguir ver
  moveDisplay(9, 100);
  // delay na transição entre a apresentação dos programas
  delay(500);
  
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
      lcd.clear();
      messageLCD("Rápidos       Rápidos     Rápidos     Rápidos  Rápidos ",0,0);
      messageLCD("1-Rápido (Pre.def) 2-Rápido ()",0,0);
      num = IRrequest();

      switch (num){
        // Rápido (30 min) - tecla 1
        case 1:
          progMachine (30, 682);
          // FALTA a opção de pausa com o comando
        
        break;
  
        // Rápido ( Temperatuta e velocidade ajustável ) - tecla 2
        case 2: 
          // FALTA: Fazer as opções do comando para colocar numeros (temperatura e velocidade)
          // FALTA: a funcao PAUSE
          // FALTA: condição pra dar erro se a temperatura e velocidades colocadas estiverem fora do intervalo (a ser definido de acordo com a datasheet:
          lcd.clear();
          messageLCD("Temperatura (entre 20º-60º): ",0,0); 
          
          temperature = IRrequest();
  
          lcd.clear();
          // FALTA: ver intervalos de velocidade e como relacionar isso para cada programa
          messageLCD("Velocidade (entre - ): ",0,0); 
          
          rotations = IRrequest();
  
          lcd.clear();
          // Admitindo 10-200
          messageLCD("Tempo (entre 10-200 min): ",0,0); 
        
          tempoFuncionamento = IRrequest();
  
          progMachine (tempoFuncionamento, 682);

          break;
      }

          
      
      break;
    // Opção- Delicados 
    case 2: 
        
      break;
    // Opção- Algodões
    case 3: 
        
        break;
    // Opção- Sintéticos
    case 4:
        
        break;
    
    default:
      Serial.print(" unknown button   ");
      Serial.println(results.value, HEX);
  }
    
  lcd.setCursor(0, 0);
  lcd.print("Menu       Menu     Menu     Menu  Menu ");
  lcd.print("1-Lavagens  2-Cenfriguar 3-Torcer   4-EN");
  delay(500);
  lcd.scrollDisplayLeft();
  delay(100);
  
}




/*--------------------------------------------
 * ---------------- Funções ------------------
 * -------------------------------------------
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


// Centrifugação - Torcer
void motorStepMovs (int v, int t){
 int steps = v*t;
 //Gira o eixo do motor no sentido horario - 120 graus
 for (int i = 0; i<=3; i++)
 {
 myStepper.step(-steps); 
 }
 
 //Gira o eixo do motor no sentido anti-horario - 120 graus
 for (int i = 0; i<=2; i++)
 {
 myStepper.step(steps); 
 }
 //Gira o eixo do motor no sentido horario, aumentando a
 //velocidade gradativamente
 for (int i = 10; i<=60; i=i+10)
 {
   myStepper.setSpeed(i);
   myStepper.step(40*i);
 }
 delay(50);
}


// Funçao que descreve um programa
// timeMax: 1min <=> 1seg (para fins demonstrativos)
void progMachine (int timeMax, int speedMov){
  
  product = productIn (comandOption, product);
  // Enquanto o detergente não for introduzido
  while (product == false)
  {
    digitalWrite (redLed, HIGH); // Sinaliza que o ciclo de lavagem não pode ser iniciado
    lcd.clear();
    messageLCD("Wainting...", 0,0);
    product = productIn (comandOption, product);               
  }
                
  digitalWrite (redLed, LOW); 
  digitalWrite (greenLed, HIGH); // Sinaliza que se pode iniciar o ciclo de lavagem
  digitalWrite (blueLed, HIGH); // Indica que o produto para a lavagem foi introduzido
                
  lcd.clear();
  messageLCD("Lets Start!", 0,0);
                
  while (timeCheck == false)
  {
    motorStepMovs (speedMov, timeMax);
    timeCounter++;
    if(timeCounter = timeMax){
      timeCheck == true;
    }
  }
}


// Ciclo de Lavagem: Lavagem -> Enxaguamento -> Centrifugação -> Descarga
// Lavagem 
void lavagem (int timeMax, int speedMov){
  
}


// Selecionar um número no comando para a temperatura e velocidade
// NOTA: Colocar a referencia a este código - baseado no programa do problema 3 da Ficha 9!!


int IRrequest (){

    int number; 
    results.value = 0xFF6897; // garante que entra no while, se a última tecla que o utilizador premiu foi o play
    //se formos utilizar as variáveis receiveIR e comandOption em baixo, temos de atualizá-las à medida que irrecv.decode(&results) e results.value mudam


    while(results.value != 0xFFC23D)
    {
      if (irrecv.decode(&results))
      {      
        switch(results.value)
        {
          case 0xFFC23D:  
            Serial.println(" PLAY/PAUSE     "); 

            break;
            
          case 0xFF6897:
            number = 0;
            messageLCD(String(number), 0, 1);
            delay(1000);
            
            break;
        
          case 0xFF30CF:  
            number = 1;
            messageLCD(String(number), 0, 1);
            delay(1000);
            break;
        
          case 0xFF18E7: 
            number = 2;
            messageLCD(String(number), 0, 1);
            delay(1000);
            break;
        
          case 0xFF7A85: 
            number = 3;
            messageLCD(String(number), 0, 1);
            break;
            
          case 0xFF10EF:
            number = 4;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF38C7:  
            number = 5;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF5AA5: 
            number = 6;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF42BD:
            number = 7;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF4AB5: 
            number = 8;
            messageLCD(String(number), 0, 1);
            break;
        
          case 0xFF52AD: 
            number = 9;
            messageLCD(String(number), 0, 1);
            break;
        
          default:
            Serial.print(" unknown button   ");
            Serial.println(results.value, HEX);
        }
        
        irrecv.resume();
      }
    }
    
    return number;
}
