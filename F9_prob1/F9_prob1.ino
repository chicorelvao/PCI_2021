/* Francisco Relvão, João Fernandes e Sílvia Santos -> Grupo C (TP2)
 * Exercício: Implementação de um termómetro analógico de ponteiro 
 * com alcance entre os 10ºC-40ºC
 * 
 * Programa: Leitura do valor proveniente do sensor de temperatura, 
 * através da porta analógica A0. Cálculo dos bits para a tensão 
 * mínima (correspondente a 10ºC) e máxima de output (correspondente a 40ªC) 
 * do sensor de temperatura, através do fator de conversão deste. Utilização 
 * da função map para converter um valor dentro de um intervalo de valores 
 * (de bits) para outro intervalo (ângulos do motor servo), de modo a 
 * conseguirmos implementar um termómetro analógico de pontenteiro.
 */
 
/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > 
  Serial Plotter menu). Attach the center pin of a potentiometer to pin A0,
  and the outside pins to +5V and ground.This example code is in the public 
  domain.
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

#include <Servo.h>

Servo myservo;  /* criação de um objeto servo para controlar o motor servo */ 


/*----- PORTAS -------*/
int servoPin = 9; /*Pin do servo*/
int tempPin = A0; /*Pin analógico utilizado para conectar o sensor de 
                    temperatura*/


/*----- Valores medidos -------*/
int valTemp = 0;    // variável para ler o valor proveniente do pin analógico
int val = 0; // valor de controlo para mapear de bits para angulos

/*----- LIMITES DOS INTERVALOS DE MEDIÇÃO E USO -------*/
/*
 Este valores são obtidos da datasheet do sensor de temperatura que está 
 diretamente calibrado em graus.
 O fator de conversão é 10 mV/Cº.
 Temperatura máxima = 40ºC
 Temperatura mínima = 10ºC 
 Exemplo: 
 Cº          V         Bits
 0º -----> 0 mV
 10º ----> 100mV -----> 20
 40º ----> 400mV -----> 81
*/

int maxTemp = 81; /* Bits para a tensão máxima de output do 
                     sensor de temperatura*/
int minTemp = 20; /* Bita para a tensão minima de output do
                     sensor de temperatura*/


int maxDegrees = 180; /*Angulo máximo desejado para o servo motor*/
int minDegrees = 0;  /*Angulo minímo desejado para o servo motor*/

/*---- TEMPO DE PAUSA ----*/
int delayTime = 15; 


/* A rotina de setup corre uma única vez, quando é realizado reset*/
void setup() {
  Serial.begin(9600); /*inicalizar a comunicação a 9600 bits por segundo*/
  myservo.attach(servoPin); /*Pin de ligação do servo a uma porta PWM. Porta 9*/
  
}

/*a rotina de loop corre infinitivamente*/
void loop() {
  /*lê o valor do input do pin analógico A0*/
  valTemp = analogRead(tempPin);

  /*imprime o valor que é lido pelo sensor*/
  Serial.println(valTemp);
  
  /*a escala a usar com o servo : valor entre 0 e 180 
  
  fluxo de controlo para ajustar o servo motor*/
  
  if(valTemp > minTemp && valTemp < maxTemp) {
   /*intervalo de medição pedido*/
    val = map(valTemp, minTemp, maxTemp, minDegrees, maxDegrees);
    
  } else if (valTemp < minTemp){
    /* para valores de temperatura menores que a temperatura minia, 
     * o servo motor mantem o ponteiro em zero graus
     */
    val = 0;
  } else{
    /* para valores de temperatura maiores que a temperatura máxima, 
     * o servo motor mantem o ponteiro em 180 graus
     */
    val = 180;
  }
  
  Serial.println("--------------- \n");
  
  myservo.write(val); /*posiciona o servo de acordo com o valor da escala*/
  delay(delayTime);   
 

}
