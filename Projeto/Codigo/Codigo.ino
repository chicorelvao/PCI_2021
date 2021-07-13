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

//Conexão com o sensor de temperatura
int sensorTemp = A0;





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
int blueLed = 12;

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

//Variavel que contem a temperatura maxima e minima de um programa

int tempMaxLimit = 31;
int tempMinLimit = 15;
//Temperatura medida pelo o sensor de temperatura
int tempNow;


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


//Simbolo do gelo para quando a temperatura está abaixo da temperatura miníma

byte ice[8] =  {0b00100 ,
                0b10101 ,
                0b01110 ,
                0b00100 ,
                0b11111 ,
                0b01110 ,
                0b10101 ,
                0b00100 };


/*----------------------------------------
 * --------Definições do Infrared---------
 * ---------------------------------------
 */

//Objeto que permite a interação com o sensor de infra-vermelhos
IRrecv irrecv(IRPin);
//Objeto que contem o dados de uma informação recebida pelo o sensor de Infra-vermelhos
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
  * mais facilde imprimir estes no LCD
  */
  lcd.createChar(0,lock);
  lcd.createChar(1,ice);

  //Inicialização do sensor de infra-vermelhos
  irrecv.enableIRIn();

 

/*--------------------------------------------
 * ----------------Apresentação-----------------
 * ---------------------------------------------
 */

   // Mensagem de inicio da máquina
  messageLCD("Bem Vindo!",3,0);
  delay(1000);
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

  /**
   * A função messegeLCD permite escrever uma String na posição indicada
   * nos argumentos sendo o primeiro a ordenada e o segundo a abcissa.
  */
  messageLCD("Menu",5,0); 
  delay(500);
  lcd.clear();

  /**O while loop tranca o utilizador até este introduzir um
   *  número que corresponda a um  dos programas.
   */
  while(true)
  {
    //Apresentação das lavagens
    lcd.clear();
    delay(500);
    messageLCD("  Programas ", 0, 0);
    messageLCD("  1-Rapidos  ", 0, 1);
    //O delay serve para dar tempo ao utilizador de ler
    delay(500);
    lcd.clear();
    
    messageLCD("  Programas ", 0, 0);
    messageLCD("  2-Delicados ", 0, 1);
    delay(500);
    lcd.clear();

    messageLCD("  Programas ", 0, 0);
    messageLCD("  3-Algodoes  ", 0, 1);
    delay(500);
    lcd.clear();
    messageLCD("  Programas ", 0, 0);
    messageLCD("  4-Sintéticos ", 0, 1);
    delay(500);
    lcd.clear();
    
    messageLCD("Selecione:", 0, 0);
    delay(500);
    //É pedido ao utilizador um número pela função IRreques() que lê o comando
    num = IRrequest();
    //O número introduzido é verificado e se está dentro do intervalo de opcões
    if( (num > 0) && (num < 5)){
      //Caso esteja, o ciclo é quebrado
      break;


    } else if(num > 5) {
      //Caso contrátio, o utilizador recebe uma mensagem de erro
      lcd.clear();
      messageLCD("Opcao invalida.", 0, 0);
      delay(1000);
      //O programa volta a apresentar o menu e pede outro número
    }

  }

  /**Sendo introduzido um número correto, um programa ou um conjunto de
   * programas é selecionado.
   * Existem 4 opcões:
   *  - Rápidos (Conjunto de programas)
   *    - 1 Rápido: Pré-definido, Temperatura 30ºC
   *    - 2 Rápido: Duração 15-35 min, Temperatura 20ºC
   *    - 3 Rápido: Duração 15-35 min, Temperatura 40ºC
   *    - 4 Rápido: Duração 15-35 min, Temperatura 60ºC 
   * 
   *  - Delicados:  Duração 45-75 min, Temperatura 30ºC 
   *
   *  - Algodões (Conjunto de programas)
   *    - 1 Algodões: Duração 165 - 225 min, Temperatura 30ºC
   *    - 2 Algodões: Duração 450 min, Temperatura 40ºC

   *  - Sintéticos (Conjunto de programas)
   *    - 1 Algodões: Duração 105 - 225 min, Temperatura 30ºC
   *    - 2 Algodões: Duração 360 min, Temperatura 40ºC
   * 
  */

  switch (num){ 
    // Opção- Rápidos  

    case 1:  
      goBack = false;
      num = 0;
      /**O utilizador fica preso no neste loop até introduzir um número correto.
       * No entanto, a booleana goBack permite ao utilizador voltar atrás para 
       * o menu e selecionar outro programa. A variavel goBack é alterada pela
       * função IRrequest() carregando na tecla previous.
       * */
      while(!goBack)
      {
        lcd.clear();
        messageLCD("      Rapidos         Rapidos      ", 0, 0);
        messageLCD(" 1-Rapido (Pre.def) 2-Rapido (20 C)", 0, 1);
        delay(200);
      
        //A função moveDisplay deslocação do texto no ecrã, para o utilizador conseguir ver
        moveDisplay(19, 300);
        //Delay na transição entre a apresentação dos programas
        delay(200);
        
        messageLCD("      Rapidos         Rapidos      ", 0, 0);
        messageLCD(" 3-Rapido (40 C)    4-Rapido (60 C)", 0, 1);
        // delay no início da apresentação
        delay(200);
      
        moveDisplay(19, 300);
        
        messageLCD("Selecione o programa desejado:", 0, 0);
        delay(500);
        //Pedido de um número
        num = IRrequest();

        //Verificação da introdução do input do utilizador
        if( (num > 0) && (num < 5)){
          //O programa é selecionado e o loop é quebrado.
          goBack = true;
          lcd.clear();
          messageLCD("Programa: " + String(num), 0, 0);
          delay(1000);
        } else if( num > 5) {
          //A introdução de um número errado prende o utilizador no loop

          lcd.clear();
          messageLCD("Opcao invalida.", 0, 0);
          delay(1000);

        } 

        

      }
      //Selecção de um programa dos rápidos

      /**
       * Nos programas pré-definidos é usado a função cicloDeLavagem que
       * não pede inputs aos utilizadores. 
       * Em alguns programas, o utilizador podes escolher o tempo de
       * funcionamento. Neste caso, o número é verificado pela função
       * progMaquina. De seguida, a função de cicloDeLavagem é chamada. 
       * A temperatura e velocidade máxima (motorSpeed) não podem ser 
       * definidas pelo utilizador.
      */

     /** Argumentos da função progMaquina:
      *  progMaquina(velocida máxima do motor, temperatura máxima, tempo minino a ser introduzido, tempo maximo)
    */
      switch (num){
        // Rápido (30 min) - tecla 1
        case 1:

          /* Da documentação
          rpm (máquina)          rpm (stepper)
          1200           ----->  18
          800            ----->  12
          */

          motorSpeed = 12;
          cycleDuration = 10;
          tempMaxLimit = 30;
          cicloDeLavagem(motorSpeed, cycleDuration, tempMaxLimit);
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
      goBack = false;
      num = 0;
      while(!goBack){
        
        lcd.clear();
        messageLCD(" Algodoes     Algodoes      Algodoes",0,0);
        messageLCD(" 1-Algodao diario 2-Algodao (225min)",0,1);
        moveDisplay(20, 300);
        
        messageLCD("Selecione o programa desejado:", 0, 0);
        delay(500);
        num = IRrequest();


        if( (num > 0) && (num < 3)){
          goBack = true;
          lcd.clear();
          messageLCD("Programa: " + String(num), 0, 0);
          delay(1000);
        } else if( num > 3) {

          lcd.clear();
          messageLCD("Opcao invalida.", 0, 0);
          delay(1000);

        } 
      }

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
          tempMaxLimit = 40;
          cicloDeLavagem(motorSpeed, cycleDuration, tempMaxLimit);
          invalidOption = false;
          break;
      }
        
      
        
      break;
        
    // Opção- Sintéticos
    
    case 4:
      goBack = false;
      num = 0;

      while(!goBack){
        
        lcd.clear();
        messageLCD(" Sinteticos     Sinteticos     Sinteticos",0,0);
        messageLCD(" 1-Sintetico diario 2-Sintetico (200 min)",0,1);
        moveDisplay(25, 300);

        messageLCD("Selecione o programa desejado:", 0, 0);
        delay(500);
        num = IRrequest();

        if( (num > 0) && (num < 3)){
          goBack = true;
          lcd.clear();
          messageLCD("Programa: " + String(num), 0, 0);
          delay(1000);
        } else if( num > 3) {

          lcd.clear();
          messageLCD("Opcao invalida.", 0, 0);
          delay(1000);

        } 
      }

      
      switch (num){
        // Sintético diário - tecla 1
        case 1:

          progMaquina(18, 30, 105, 225);// 105 min = 1 3/4 h e 225 min = 2 1/2 h
        
          break;

        // Sintético (200 min) - tecla 2
        case 2:

          motorSpeed = 18;
          cycleDuration = 360; // 200 min na datasheet correspondem a 360s no stepper
          tempMaxLimit = 40;
          cicloDeLavagem(motorSpeed, cycleDuration, tempMaxLimit);
          break;

      }
        
      
      break;
      
  
  
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

/** O tempo que falta é apresentado ao utilizador durante o ciclo
 * de lavagem. O tempo restante é calculado a partir da duração
 * atribuida para cada fase de lavagem (Ver secção Controlo do
 * ciclo de lavagem). O arduino tem uma função chamada millis()
 * que conta o tempo desde que o arduino está ativo. O retorno é 
 * um long. 
 * O tempo restante é obtido somando o tempo de duração mais 
 * o tempo medido no timer, antes da lavagem começar e subtraíndo
 * o tempo atualmente medido durante a lavagem:
 * tempo atual: millis()
 * tempo restante = tempo medido no inicio da lavagem + duração da fase
*/
void printTimeLeft(){

  long timeLeft = cycleTimeEnd - millis();
  String printTime;
  String minuteStr;
  //O time LEft é um long e necessita de ser convertido para o formato mm:ss
  long hourLeft = timeLeft/60000;
  //Não existe horas ou minutos negativos
  if(hourLeft < 0) {
    hourLeft = 0;
  }

  long minuteLeft = (timeLeft - hourLeft*60000)/1000;
  
  if(minuteLeft < 0 && hourLeft == 0) {
    minuteLeft = 0 ;
  }

  //Se os minutos forem menores que 10, no formato mm:ss tem de aparecer um zero e depois o número
  if(minuteLeft < 10) {
    minuteStr = "0" + String(minuteLeft);
  } else {
    minuteStr = String(minuteLeft);
  }



  printTime = String(hourLeft) + ":" + minuteStr;
  //O tempo é imprimido
  messageLCD(printTime, 9,1);


  /**O utilizador tem a opção de parar a máquina. Se a maquina parar,
   * tambem tem de parar a contagem do tempo. Assim, a variavel stopTimeInit
   * marca a primeira pausa e stopTimedEnd, marca quando a lavagem recomeça.
   * O tempo de finalização de uma fase tem de ser atualizado (runTime).
   * O tempo a ser mostrado ao utilizador também é atualizado.(CycleTimeEnd)
   * 
  */
  int stopTimeInit = millis();
  //IRpause();
  //checkTemp(tempMaxLimit, tempMinLimit);
  int stopTimeEnd = millis();

  runTime += stopTimeEnd-stopTimeInit;
  cycleTimeEnd +=  stopTimeEnd-stopTimeInit;

  
  

 }




/*---------------------------------------------------------------
 * ----------------Controlo do ciclo de lavagem------------------
 * --------------------------------------------------------------
 */

/**O utilizador pode escolher o tempo de lavagem de alguns programas.
 * A função proMaquina verifica se o tempo introduzida está dentro de
 * um determinado intervalo e chama a função cicloDeLavagem para iniciar
 * o programa de lavagem;
 * 
*/

void progMaquina(int motorSpeed, int tempMaxLimit, int infLim, int supLim)     
{
  lcd.clear();
  num = 0;
  goBack = false;

  //Conta o número de vezes que o utilizador tentou introduzir número
  int countTries = 0;
  //O while loop so quebra quando o número selecionado estiver dentro de um intervalo
  num = infLim;

  while(countTries == 0 || (num < infLim || num > supLim))
  {
    countTries++;
    //Na primeira tentativa:
    if (countTries == 1)
    {
      messageLCD(" Insira a duracao desejada: ",0,0);
      moveDisplay(12, 200);
    }
    //Em tentativas posteriores, aperece erro:
    else if(countTries > 1 && num != 0)
    {
      messageLCD(" Duracao introduzida invalida!",0,0);
      moveDisplay(14, 200);
      messageLCD(" Insira a duracao desejada: ",0,0);
      moveDisplay(12, 200);
    } 
      
    //Se o utilizador quiser voltar atrás, o ciclo é quebrado e não avança para o ciclo de lavagem
    if(!goBack){
      num = IRrequest();
    } else{
      break;
    }

  }


  if(!goBack){
    cicloDeLavagem(motorSpeed, num, tempMaxLimit); 
  }           
}

/** A função cicloDeLagavem dividi um progam em quatro fases
 * que têm de respeitar o tempo de duração introduzido pelo o utilizador
 * 
*/
void cicloDeLavagem(int motorSpeed, int  cycleDuration, int tempMaxLimit){ 
  //Informação ao utilizador que a lavagem vai começar
  lcd.clear();
  lcd.setCursor(15,1);
  lcd.write(byte(0));
  messageLCD("A lavar...",0,0);
  digitalWrite(greenLed, HIGH);
  //Impede que o programa trave logo após escolher uma opção no menu
  irrecv.resume();

  //Cálculo da "hora interna" do arduino a que o programa vai terminar
  cycleTimeEnd = millis() + ( (long) cycleDuration)*1000;
  /** as frações do tempo de duração correspondentes a cada fase do ciclo 
   * precisam de ser ajustadas com valores que façam mais sentido
  */
  int washDuration = 0.25 * cycleDuration;
  int rinseDuration = 0.25 * cycleDuration;
  int spinDuration = 0.25 * cycleDuration;
  int drainDuration = 0.25 * cycleDuration;

  //São chamadas as 4 fases de lavagem
  lavagem(washDuration, motorSpeed);
  //Esta fase é a de exaguamento
  lavagem(rinseDuration, (motorSpeed-5));
  centrifugacao(spinDuration, motorSpeed);
  descarga(drainDuration, motorSpeed);

  //Led verde é ativado
  digitalWrite(greenLed, LOW);
  
  //No fim do programa, o buzzer apita para informar o utilizador
  NewTone(buzzPin, 1000); // Send 1KHz sound signal...
  delay(2000);    
  noNewTone(buzzPin);     // Stop sound...    

}
    
// Indica se os produtos (detergente e amaciador) foram colocados
/*
void productIn (){

  
     * Os valores -1  e -3 devolvidos por IRrequest() foram arbitrariamente definidos para 
     * servirem de flag à colocação de detergente e à opção de voltar atrás, respetivamente.
     * Se num = -1 o detergente foi colocado e o programa segue, caso contrário o 
     * detergente não foi colocado e o programa volta a solicitar a sua inserção.
     * Se num = -3 a opção de voltar atrás foi escolhida e o programa volta à 
     * apresentação dos programas, caso contrário o programa segue normalmente
     

  product = false;

  while(product){

    messageLCD(" Coloque o detergente e prima CH", 0, 0);
    moveDisplay(16, 500);
    num = IRrequest();
  
  // Associa-se esta função à tecla CH do telecomando
    if(num = 0xFF629D){
      product = true;      
    }

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
  }


}
*/

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
 

//Fase de lavagem

/**Entre cada delta passos, o motor imprime o tempo restante
     * verifica se o comando foi pressionado e ainda, verifica-se 
     * a temperatura ultrapassou os limites definidos.
    */

// 1ª - Lavagem: O motor gira a velocida constante numa unica direção
// 2ª - Enxaguamento: O mesmo que a lavagem, mas a uma velocidade menor
void  lavagem (int timeMax, int speedMov){
  //A velocidade é definida
  myStepper.setSpeed(speedMov);
  deltaStep = 100;
  //A fase só pode durar uma fração definida do tempo total de lavagem
  runTime = ((long) timeMax)*1000 + millis();
  //Enquanto a lavagem nao ultrapassar a o tempo definido
  while(runTime > millis()){
    //O motor anda um número minimo de passos
    myStepper.step(deltaStep);
    //É imprimido o tempo que falta, o comando é verificado e a temperatura também
    printTimeLeft();
    
    
  }
  
  
}


/**Na centrifugação, o motor aceleração numa direção, deseceleraça
 * nessa direção e faz o inverso para a outra.
 * 
*/
void centrifugacao (int timeMax, int speedMov){
  //Cálculo da hora a que vai terminar esta fase
  runTime = ((long) timeMax)*1000 + millis();

  //Controlo do tempo
  while(runTime > millis()) {
    //O motor roda da seguinte forma:
    /**
     * Desacelera para a direita
     * Acelera para a esquerda
     * Desacelera para a esquerda
     * Acelera para a a direita
     * 
     * Este processo é repetido 4 vezes.
     * No fim, é verificado se ainda há tempo para fazer
     * mais rotações.
     * 
     * A função rotationDirection trata deste processo
    */


    for(int rotationType = 0; rotationType < 4; rotationType++){

      switch (rotationType){

        case 0:
          
          rotationDirection(speedMov, false, true);
          break;

        case 1:
        
          rotationDirection(speedMov, true, false);
          break;

        case 2:
          
          rotationDirection(speedMov, false, false);
          break;

        case 3:
       
          rotationDirection(speedMov, true, true);
          break;
        
        default:
          break;
      }
      //A hora de termina acontecer durante as 4 repetições, o loop é quebrado
      if(millis() > runTime) {
        break;
      }

    }

    

  }

}

/** A seguinte função define a direção a acelaração e direção 
 * de rotação do motor através dos argumentos.
 * Se speedUp true, então acelera e vice-versa.
 * Se toRight true, então roda para a direta.
 * 
*/
void rotationDirection(int speedMov, boolean speedUp, boolean toRight){
  //Definição da direção
  if(toRight) {
    deltaStep = 100;
  } else {
    deltaStep = -100;
  }

  //Definição de acelaração ou desacelaração
  if(speedUp){
    //A velocidade vai crescendo a cada 4 vezes deltaSteps
    for (int i = 3; i <= speedMov; i = i + (speedMov/3))
    {
      
    
      myStepper.setSpeed(i);
      for(int j = 0; j<= 4; j++){
        myStepper.step(deltaStep);
        printTimeLeft();
      }
      
    }
    
  } else {
    //A velocidade vai diminuindo a cada 4 vezes deltaSteps
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

//Na descarga, o maquina vai desacelarando lentamente, até o programa terminar

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

//Retorno o número introduzido pelo o utilizador
int IRrequest (){
    //número que vai ser introduzido pelo o utilizador
    int number = 0; 
    /**Os números são construídos ao somar multiplos de 10:
     * Tecla 1 -> 1
     * Tecla 2 -> 2*10 + 1 = 21
     * Etc
     * 
    */


    /**O hexadecimal corresponde à tecla Play. Esta tecla serve
     * para confirmar o número introduzido pelo o utilizador.
     */
    results.value = 0xFFC23D; 

    /** 
    * O programa fica preso no while loop até a tecla play ser
     * pressionada. 
     * Dentro do while loop, o utilizador pode construir números
     * de 0 até 999. O limite advem do facto de um inteiro nao
     * puder ser maior que 32767. Assim, evitam-se erros de 
     * tamanhos nos registadores.
     * Além disso, nenhum ser humano usa números maior que 500 
     * para tarefas domésticas.
     * 
    */

    //Esta boleana impede que o sensor leia demasiadas vezes o valor de uma tecla pressionada
    //Atua como uma espécie de buffer
    boolean catchIR;
    while(results.value != 0xFFC23D || irrecv.decode(&results)){

      //Se o comando tiver sido pressionado, entra no if statement
      if(irrecv.decode(&results)){
        catchIR = true;
      } else {
        catchIR = false;
      }
  

      if (catchIR){   
       //O comando funciona por hexadecimais que são traduzidos para números ou booleans

        switch(results.value){ 
          case 0xFF22DD:
          //Tecla Rewind - Voltar atrás no menu
            goBack = true;
            //Quando se carrega rewind, o número que estava a ser construído é eliminado
            number = 0;
            messageLCD("Voltar atras?", 0, 1);

            break;

          case 0xFF629D:  
          //Tecla CH
            messageLCD("Detergente colocado?", 0, 1);
            break;
            
            //Por ordem: Teclas 0-9
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
        }
        
        //Se o número introduzido pelo utilizador é maior que 900, então é eliminado
        if(number > 900){
          //O utilizador pode construir outro número do zero
          number = 0;
          messageLCD("0     ", 0, 1);
        } 
        
        /** Se o utilizador carregar primeiro na tecla rewind, então
         * a boolean goBack fica verdadeira. Se o utilizador carregar
         * de seguida num número, então quer dizer que quer entrar 
         * num programa. Vai existir conflito, portanto, após qualquer
         * número ser construído, booleana goBack é negativa.
         * 
        */
        if(number >= 0){
          goBack = false;
        }

        //Reset do objeto de IR para ler novos códigos.
        irrecv.resume();
        

      }
      
    } 
    irrecv.resume();

    return number;
     
    
    
}

//IRpause permite parar uma lavagem. Faz uma pausa.       
void IRpause(){

    results.value = 0xFFC23D; 

    while(results.value != 0xFFC23D || irrecv.decode(&results)){
      //Se o comando tiver sido pressionado, entra no if statement
      
      messageLCD("Paragem.", 0, 1);
      //Se o comando for novamente pressionado, a lavagem é retomada
      if (irrecv.decode(&results)){   
       
        irrecv.resume();

      }
      
    } 
    messageLCD("        ", 0, 1);
    irrecv.resume();
}
    
    
/*---------------------------------------------------------------
 * --------------------Sensor de temperatura---------------------
 * --------------------------------------------------------------
 */

//Verificação da temperatura no sensor
void checkTemp(int maxTemp, int minTemp){
//Medição da temperatura
  tempNow = analogRead(sensorTemp) * 5/(0.01*1023);

  //Se a temperatura estiver muito alta, o led vermelho é aceso e a lavagem para
  if(tempNow > maxTemp) {
    pinMode(redLed, LOW);
    //a lavagem para devido a este loop
    while(true) {
      messageLCD("Too hot.", 0, 1);
      tempNow = analogRead(sensorTemp)* 5/(0.01*1023);
      //Se a temperatura voltar ao normal, a lavagem continua
      if(tempNow < maxTemp) {
        break;
      }

    }
    //Para uma temperatura muito baixo, é imprimido um floco de gelo
  } else if(tempNow < minTemp) {
    pinMode(redLed, HIGH);
    lcd.setCursor(15,0);
    lcd.write(byte(1));

  } else {
    //Dentro do intervalo de temperatura, a lavagem funciona normalmente
    pinMode(redLed, HIGH);
    lcd.setCursor(15,0);
    lcd.write(" ");

  }


}

            
