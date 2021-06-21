/*
 Controlling a servo position using a potentiometer (variable resistor)
 by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

 modified on 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Knob
*/
/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo

/*----- PORTAS -------*/
int servoPin = 9; //Pin do servo
int tempPin = A0;  // analog pin used to connect o sensor de temperatura



/*----- Valores medidos -------*/
int valTemp = 0;    // variable to read the value from the analog pin

int val = 0; //valor de controlo para mapear de bits para angulos

/*----- LIMITES DOS INTERVALOS DE MEDIÇÃO E USO -------*/
/*
 Este valores são obtidos da datasheet do sensor de temperatura que está diretamente calibrado em graus.
 O fator de conversão é 10 mV/Cº.
 Exemplo: 
 Cº          V         Bits
 0º -----> 0 mV
 10º ----> 100mV -----> 20
 40º ----> 400mV -----> 81

*/
int maxTemp = 81; // Bits para a tensão máxima de output do sensor de temperatura
int minTemp = 20; // Bita para a tensão minima de output do sensor de temperatura

int maxDegrees = 180; //Angulo máximo desejado para o servo motor
int minDegrees = 0;  //Angulo minímo desejado para o servo motor

/*---- TEMPO DE PAUSA ----*/
int delayTime = 15; //


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  myservo.attach(servoPin);  //Pin de ligação do servo a uma porta PWM. Porta 9
  
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin A0:
  valTemp = analogRead(tempPin);

  // print out the value you read:
  Serial.println(valTemp);
  
  // scale it to use it with the servo (value between 0 and 180)

 //fluxo de controlo para ajustar o servo motor
  if(valTemp > minTemp && valTemp < maxTemp) {
   //intervalo de medição pedido
    val = map(valTemp, minTemp, maxTemp, minDegrees, maxDegrees);
    
  } else if (valTemp < minTemp){
    //para valores de temperatura menores que a temperatura minia, o servo motor mantem o ponteiro em zero graus
    val = 0;
  } else{
    //para valores de temperatura maiores que a temperatura máxima, o servo motor mantem o ponteiro em 180 graus
    val = 180;
  }

  //Serial.println(val);
  
  Serial.println("--------------- \n");
  
  myservo.write(val); // sets the servo position according to the scaled value
  delay(delayTime);   
 


}
