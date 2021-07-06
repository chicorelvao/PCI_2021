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
int cycleDuration;

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
Stepper myStepper(stepsPerRevolution, inFourM, inTwoM, inThreeM, inOneM);

// Variáveis do motor de passo 

int stepCount = 0;  // number of steps the motor has taken
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
  // myStepper.setSpeed(speedI);

  Serial.begin(9600);
  
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
  Serial.println("num");
  Serial.println(num);
  
  switch (num){ 
    // Opção- Rápidos  
    
    case 1:  
      lcd.clear();
      messageLCD("Rápidos       Rápidos        Rápidos        Rápidos        Rápidos",0,0);
      messageLCD("1-Rápido (Pre.def) 2-Rápido (20 C) 3-Rápido (40 C) 4-Rápido (60 C)",0,0);
      moveDisplay(9, 100);
      num = IRrequest();

      switch (num){
        // Rápido (30 min) - tecla 1
        case 1:
          //progMachine (30, 682);
          // FALTA a opção de pausa com o comando
          /*
          rpm (máquina)          rpm (stepper)
          1200           ----->  18
          800            ----->  12
          */
          motorSpeed = 12;
          cycleDuration = 60;
          temperature = 30;
          //progMachine (30, 682);
          cicloDeLavagem(motorSpeed, cycleDuration, temperature);
               
        break;
  
        // Rápido (duração e temperatura ajustável) - tecla 2
        case 2: 
          // FALTA: Fazer as opções do comando para colocar numeros (temperatura e velocidade)
          // FALTA: a funcao PAUSE
          // FALTA: condição pra dar erro se a temperatura e velocidades colocadas estiverem fora do intervalo (a ser definido de acordo com a datasheet:
          /*
          rpm (máquina)          rpm (stepper)
          1200           ----->  18
          800            ----->  12
          */
          messageLCD("Insira a duração desejada (15-35 min): ",0,0);
          moveDisplay(9, 100);
          num = IRrequest();
          motorSpeed = 12;
          cycleDuration = num;
          temperature = 20;
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
 * Logo, a um programa de 60 min, corresponde uma demonstração
 * de 1,5 min com 55296 steps.
 */
 
// Lavagem 
void lavagem (int timeMax, int speedMov){
  Serial.println(speedMov);
  myStepper.setSpeed(speedMov);
  long a = 2048L;
  long steps = a * speedMov * timeMax / 60;
  Serial.println("steps");
  Serial.println(steps);
  for(int i = 0; i < steps; i += 100){
    myStepper.step(100);
  }
}

// Enxaguamento
void enxaguamento (int timeMax, int speedMov){
  Serial.println(speedMov);
  myStepper.setSpeed(speedMov);
  long a = 2048L;
  long steps = a * speedMov * timeMax / 60;
  Serial.println("steps");
  Serial.println(steps);
  for(int i = 0; i < steps; i += 100){
    myStepper.step(100);
  }
}

void centrifugacao (int timeMax, int speedMov){
  long revSteps = 2048L;
  
  long steps;
  // o motor acelera clockwise até atingir a velocidade máxima
  for (int i = 0; i <= speedMov; i = i + (speedMov/3))
  {
    myStepper.setSpeed(i);
    steps = revSteps * i * timeMax / (3 * 3 * 60);
    Serial.println("steps1");
    Serial.println(steps);
    for(int i = 0; i < steps; i += 100){
      myStepper.step(100);
    }
  }
  // o motor desacelera clockwise até parar
  for (int i = speedMov; i >= 0; i = i - (speedMov/3))
  {
    myStepper.setSpeed(i);
    steps = revSteps * i * timeMax / (3 * 3 * 60);
    Serial.println("steps2");
    Serial.println(steps);
    for(int i = 0; i < steps; i += 100){
      myStepper.step(100);
    }
  }
  // o motor acelera counterclockwise até atingir velocidade máxima
  for (int i = 0; i <= speedMov; i = i + (speedMov/3))
  {
    myStepper.setSpeed(i);
    steps = revSteps * i * timeMax / (3 * 3 * 60);
    Serial.println("steps3");
    Serial.println(steps);
    for(int i = 0; i < steps; i += 100){
      myStepper.step(-100);
    }
  }
}


void descarga (int timeMax, int speedMov){
  long revSteps = 2048L;
  
  long steps;
  // o motor desacelera até parar e dá-se a descarga
  for (int i = speedMov; i >= 0; i = i - (speedMov/3))
  {
    myStepper.setSpeed(i);
    steps = revSteps * i * timeMax / (3 * 60);
    for(int i = 0; i < steps; i += 100){
      myStepper.step(-100);
    }
  }
}

/*
// Centrifugação - Torcer
void motorStepMovs (int v, int t){
 int steps = v*t;
 //Gira o eixo do motor no sentido horario - 120 graus
 for (int i = 0; i<=3; i++)
 {
 myStepper.step(-2048); 
 }
 
 //Gira o eixo do motor no sentido anti-horario - 120 graus
 for (int i = 0; i<=2; i++)
 {
 myStepper.step(2048); 
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
*/




int IRrequest (){
    //número que vai ser introduzido pelo o utilizador
    int number = 0; 
    //Variável que conta as casas decimais para o utilizador fazer número grandes rapidamente
    int countDecimal = 0;
    results.value = 0xFF6897; // garante que entra no while, se a última tecla que o utilizador premiu foi o play
    //se formos utilizar as variáveis receiveIR e comandOption em baixo, temos de atualizá-las à medida que irrecv.decode(&results) e results.value mudam


    while(results.value != 0xFFC23D){

      if (irrecv.decode(&results)){   

        switch(results.value){
          case 0xFFC23D:  
            Serial.println(" PLAY/PAUSE     "); 

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
    messageLCD(String(num), 0, 1);
    delay(1000);
    
}

void cicloDeLavagem(int motorSpeed, int  cycleDuration, int temperature)
{ 
  // as frações do tempo de duração correspondentes a cada fase do ciclo precisam de ser ajustadas com valores que façam mais sentido
  int washDuration = 0.25 * cycleDuration;
  int rinseDuration = 0.25 * cycleDuration;
  int spinDuration = 0.25 * cycleDuration;
  int drainDuration = 0.25 * cycleDuration;
  lavagem(washDuration, motorSpeed);
  enxaguamento(rinseDuration, motorSpeed);
  centrifugacao(spinDuration, motorSpeed);
  descarga(drainDuration, motorSpeed);
}
    
    
              


            
