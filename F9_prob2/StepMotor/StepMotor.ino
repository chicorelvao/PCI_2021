
/*
 Stepper Motor Control - speed control

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.
 A potentiometer is connected to analog input 0.

 The motor will rotate in a clockwise direction. The higher the potentiometer value,
 the faster the motor speed. Because setSpeed() sets the delay between steps,
 you may notice the motor is less responsive to changes in the sensor value at
 low speeds.

 Created 30 Nov. 2009
 Modified 28 Oct 2010
 by Tom Igoe

 */

#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 11:

//IN1 ----> 11
//IN2 ----> 10
//IN3 ----> 9
//IN4 ----> 8
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

int stepCount = 0;  // number of steps the motor has taken
int motorSpeed = 0;
int halfPotentiometer = 511; //ponto médio do potenciometrio
int sensorReading = 0;

//Velocidades minimas e máximas obtidas empiricamente
//O motor nao roda para outras velocidade em 64 passos por rotação completa
int minSpeed = 0;
int maxSpeed = 15;

void setup() {
  // nothing to do inside the setup
  Serial.begin(9600);
}

void lavagem (int timeMax, int speedMov){
  myStepper.setSpeed(speedMov);
  long a = 2048L;
  long steps = a * speedMov * timeMax / 60;
  for(int i = 0; i < steps; i += 100){
    myStepper.step(100);
  }
}

void enxaguamento (int timeMax, int speedMov){
  myStepper.setSpeed(speedMov / 2);
  long a = 2048L;
  long steps = a * speedMov * timeMax / 60;
  for(int i = 0; i < steps; i += 100){
    myStepper.step(100);
  }
}

void centrifugacao (int timeMax, int speedMov){
  long revSteps = 2048L;
  
  long steps;
  // o motor acelera clockwise até atingir a velocidade máxima
  for (int i = 0; i <= speedMov; i = i + 6)
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
  for (int i = speedMov; i >= 0; i = i - 6)
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
  for (int i = 0; i <= speedMov; i = i + 6)
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
  for (int i = speedMov; i >= 0; i = i--)
  {
    myStepper.setSpeed(i);
    steps = revSteps * i * timeMax / (19 * 60);
    for(int i = 0; i < steps; i += 100){
      myStepper.step(-100);
    }
  }
}

void loop() {
  //lavagem(15, 18);
  //enxaguamento(15, 18);
  centrifugacao(30, 18);
  //descarga(15,18);
  /*
  // read the sensor value:
  sensorReading = analogRead(A0);
  // map it to a range from 0 to 100:
  Serial.print("Potenciomentro: ");
  Serial.println(sensorReading);
  
  
  if(sensorReading < halfPotentiometer){
    motorSpeed = map(sensorReading, 0, halfPotentiometer, maxSpeed, minSpeed);
    Serial.print("Velocidade: ");
    Serial.println(motorSpeed);
    myStepper.setSpeed(motorSpeed);
    if(motorSpeed != 0) {
      myStepper.step(-64); 
    }
    
  } else if (sensorReading > halfPotentiometer){
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
    /*
    if(motorSpeed != 0) {
      myStepper.step(64); 
    }
    

    
  } 
  */

  
}
