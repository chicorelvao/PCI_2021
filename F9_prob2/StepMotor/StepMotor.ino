/*
 Stepper Motor Control - speed control
 This program drives a unipolar or bipolar stepper motor.
 /* Francisco Relvão, João Fernandes e Sílvia Santos -> Grupo C (TP2)
 * Exercício: Controlo da velocidade e sentido de rotação de um
 * motor de passo com um potenciómetro
 * 
 * Programa: Definição das velocidades mínima e máxima do motor de 
 * passo e do número de bits associado à colocação do eixo do potenciómetro
 * na posição central. Utilização da função map() para associar
 * a cada valor do número de bits (entre 0 e 1023), um valor da velocidade 
 * no stepper (entre 0 e 15).
 */

#include <Stepper.h>

// Passos realizados pelo motor numa revolução completa
const int stepsPerRevolution = 2048;

// Inicialização da biblioteca do stepper nos pinos 8-11
//IN1 ----> 11
//IN2 ----> 10
//IN3 ----> 9
//IN4 ----> 8
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

// Número de bits associado à seleção de uma posição do eixo do potenciómetro
int sensorReading = 0;
int potenciometer = A0;

// Velocidades minima e máxima obtidas empiricamente
// O motor nao roda para outras velocidades, em 2048 passos por rotação completa
int minSpeed = 0;
int maxSpeed = 15;

// Número de bits associado à colocação do eixo do potenciómetro na posição central
int halfPotentiometer = 511; 

// Velocidade do motor
int motorSpeed = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  //lê o valor no sensor e imprime-o
  sensorReading = analogRead(potenciometer);
  Serial.print("Potenciomentro: ");
  Serial.println(sensorReading);
  
  /*
   * O valor da velocidade do motor é mapeada de duas formas diferentes para valores
   * lidos no potenciómetro diferentes do valor médio.
   */
   
  /*
   * Para um valor lido menor do que o valor médio do potenciómetro, a velocidade 
   * é mapeada dos 0 bits ao valor médio;
    */
  if(sensorReading < halfPotentiometer){
    motorSpeed = map(sensorReading, 0, halfPotentiometer, maxSpeed, minSpeed);
    Serial.print("Velocidade: ");
    Serial.println(motorSpeed);
    myStepper.setSpeed(motorSpeed);
    if(motorSpeed != 0) {
      myStepper.step(-50); 
    }  
  } 

    /*
   * Para um valor lido maior do que o valor médio do potenciómetro, a velocidade 
   * é mapeada do valor médio aos 1023 bits;
    */  
  else if (sensorReading > halfPotentiometer){
    motorSpeed = map(sensorReading, halfPotentiometer, 1023, minSpeed, maxSpeed);
    Serial.print("Velocidade: ");
    Serial.println(motorSpeed);
    myStepper.setSpeed(motorSpeed);
    /* Para velocidades muito baixa, o motor perder muito tempo no metodo .step()
     * Este método só para de funcionar, após realizar todos os passos que lhe foi pedido.
     * 
     * Exemplo:
     * 
     * myStepper.setSpeed(20) ----> Velocidade de 20% 
     * myStepper.step(200) ----> Fazer 200 passos (Código para aqui até fazer os 200 passos)
     * Quanto maior a velocidade, mais depressa faz os passos.
    */
    if(motorSpeed != 0) {
      myStepper.step(50); 
    }
  }  
}
