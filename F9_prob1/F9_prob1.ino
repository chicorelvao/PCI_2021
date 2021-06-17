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
int tempPin = A0;  // analog pin used to connect the potentiometer

/*----- Valores medidos -------*/
int valTemp = 0;    // variable to read the value from the analog pin

/*----- LIMITES DOS INTERVALOS DE MEDIÇÃO E USO -------*/

int maxTemp = 81; // Endereço para tensão máxima de output do sensor de temperatura
int minTemp = 20; // Endereço para tensão minima de output do sensor de temperatura

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
  int val = map(valTemp, minTemp, maxTemp, minDegrees, maxDegrees); 
  myservo.write(val); // sets the servo position according to the scaled value
  delay(delayTime);   

}
