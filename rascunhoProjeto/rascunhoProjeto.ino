#include <LiquidCrystal_74HC595.h>

#define DS 4
#define SHCP 5
#define STCP 3
#define RS 1
#define E 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7

LiquidCrystal_74HC595 lcd(DS, SHCP, STCP, RS, E, D4, D5, D6, D7);

#include <IRremote.h>
int pin = 6;
IRrecv irrecv(pin);
decode_results results;

#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 11:

//IN1 ----> 11
//IN2 ----> 10
//IN3 ----> 9
//IN4 ----> 8
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

int cont1;

long num;

int tempNum;

int oneDig;

int twoDig;

int threeDig;

int fourDig;

boolean buttonOnState = true;

void messageLCD (String message,  int colsLCD, int rowLCD){
  lcd.setCursor(colsLCD, rowLCD);
  lcd.print(message);
}

// atualiza o número a ser mostrado no display,
// após cada clique nos botões de 0-9 do comando
long updateNum(int digit)
{ 
  switch (cont1)
  {
    case 1:
    oneDig = digit;
    tempNum = oneDig;
    break;
    
    case 2:
    twoDig = 10 * oneDig + digit;
    tempNum = twoDig;
    break;
    
    case 3:
    threeDig = 10 * twoDig + digit;
    tempNum = threeDig;
    break;

    case 4:
    fourDig = 10 * threeDig + digit;
    tempNum = fourDig;
    break;
  }

  return tempNum;
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(pin, INPUT);  
  irrecv.enableIRIn();
}

void loop() {
  /*
  lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print("Tempo: ");
  messageLCD("Tempo: ", 0, 0);
  cont1 = 0;
  
  while(results.value != 0xFFC23D)
  {
    if (irrecv.decode(&results))
    {      
      switch(results.value)
      {
        case 0xFFA25D:  
          Serial.println(" CH-            "); 
          break;
          
        case 0xFF629D:  
          Serial.println(" CH             "); 
          break;
          
        case 0xFFE21D:  
          Serial.println(" CH+            "); 
          break;
          
        case 0xFF22DD:  
          Serial.println(" FAST FORWARD   "); 
          break;
          
        case 0xFF02FD:  
          Serial.println(" FAST REWIND    "); 
          break;
          
        case 0xFFC23D:  
          Serial.println(" PLAY/PAUSE     "); 
          break;
          
        case 0xFFE01F:  
          Serial.println(" VOL-           "); 
          break;
        case 0xFFA857:  
          Serial.println(" VOL+           "); 
          break;
          
        case 0xFF906F:  
          Serial.println(" EQ             "); 
          break;
          
        case 0xFF6897:
          cont1++;
          Serial.println(" 0              ");
          num = updateNum(0);
          Serial.println(num);
          break;
      
        case 0xFF9867:  
          Serial.println(" 100+           "); 
          break;
          
        case 0xFFB04F:  
          Serial.println(" 200+           "); 
          break;
          
        case 0xFF30CF:  
          cont1++;
          Serial.println(" 1              ");
          num = updateNum(1);
          Serial.println(num);
          break;
      
        case 0xFF18E7: 
          cont1++;  
          Serial.println(" 2              ");
          num = updateNum(2);
          Serial.println(num);
          break;
      
        case 0xFF7A85: 
          cont1++; 
          Serial.println(" 3              ");
          num = updateNum(3);
          Serial.println(num);
          break;
          
        case 0xFF10EF:
          cont1++;   
          Serial.println(" 4              ");
          num = updateNum(4);
          Serial.println(num);
          break;
      
        case 0xFF38C7:  
          cont1++; 
          Serial.println(" 5              ");
          num = updateNum(5);
          Serial.println(num);
          break;
      
        case 0xFF5AA5: 
          cont1++; 
          Serial.println(" 6              ");
          num = updateNum(6);
          Serial.println(num);
          break;
      
        case 0xFF42BD:
          cont1++;    
          Serial.println(" 7              ");
          num = updateNum(7);
          Serial.println(num);
          break;
      
        case 0xFF4AB5: 
          cont1++;   
          Serial.println(" 8              ");
          num = updateNum(8);
          Serial.println(num);
          break;
      
        case 0xFF52AD: 
          cont1++;   
          Serial.println(" 9              ");
          num = updateNum(9);
          Serial.println(num);
          break;
      
        default:
          Serial.print(" unknown button   ");
          Serial.println(results.value, HEX);
      }

      if (cont1 == 4)
      {
        cont1 = 0;
      }
      
      irrecv.resume();
    }
  }
  
  //lcd.setCursor(0, 1);
  //lcd.print(num);
  messageLCD(String(num), 0, 1);
  Serial.println("user input");
  Serial.println(num);
  delay(2000);
  
  int motorSpeed = 18L;
  myStepper.setSpeed(motorSpeed);
  long a = 2048L;
  long b = 60L;
  long steps = a * motorSpeed * num / b;
  
  Serial.println("steps");
  Serial.println(steps);
  for(int i = 0; i < steps; i += 100){
    myStepper.step(100);
  }
  delay(10000);
*/

  while (buttonOnState == true){
    
    // Mensagem de inicio da máquina
    messageLCD("Bem Vindo!",3,0);
    delay(2000);
    lcd.clear();
  
    // Apresentação do menu principal e respetivas opções
    messageLCD("Menu",5,0); 
    delay(2000);
    lcd.clear();
    
    messageLCD("  Programas   Programas  ", 0, 0);
    messageLCD(" 1-Rapidos    2-Delicados", 0, 1);
    // delay no início da apresentação
    delay(750);
    for (int positionCounter = 0; positionCounter < 9; positionCounter++) {
       // scroll one position left:
      lcd.scrollDisplayLeft();
      // wait a bit: (delay rápido para testar rapidamente)
      delay(100);
    }

    lcd.clear();
    // delay na transição entre a apresentação dos programas
    delay(500);
    
    messageLCD("  Programas   Programas  ", 0, 0);
    messageLCD(" 3-Algodoes  4-Sinteticos", 0, 1); 
    // delay no início da apresentação
    delay(750);
    for (int positionCounter = 0; positionCounter < 9; positionCounter++) {
      // scroll one position left:
      lcd.scrollDisplayLeft();
      // wait a bit: (delay rápido para testar rapidamente)
      delay(100);
    }
    
    lcd.clear();
    //o valor máximo do positionCounter é dado pela diferença entre 16 e o comprimento da string que queremos imprimir
    messageLCD("Selecione o programa desejado:", 0, 0);
    for (int positionCounter = 0; positionCounter < 14; positionCounter++) {
      // scroll one position left:
      lcd.scrollDisplayLeft();
      // wait a bit: (delay rápido para testar rapidamente)
      delay(100);
    }

    cont1 = 0;

    results.value = 0xFF6897; // garante que entra no while, se a última tecla que o utilizador premiu foi o play
        
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
            cont1++;
            Serial.println(" 0              ");
            num = updateNum(0);
            Serial.println(num);
            break;
        
          case 0xFF30CF:  
            cont1++;
            Serial.println(" 1              ");
            num = updateNum(1);
            Serial.println(num);
            break;
        
          case 0xFF18E7: 
            cont1++;  
            Serial.println(" 2              ");
            num = updateNum(2);
            Serial.println(num);
            break;
        
          case 0xFF7A85: 
            cont1++; 
            Serial.println(" 3              ");
            num = updateNum(3);
            Serial.println(num);
            break;
            
          case 0xFF10EF:
            cont1++;   
            Serial.println(" 4              ");
            num = updateNum(4);
            Serial.println(num);
            break;
        
          case 0xFF38C7:  
            cont1++; 
            Serial.println(" 5              ");
            num = updateNum(5);
            Serial.println(num);
            break;
        
          case 0xFF5AA5: 
            cont1++; 
            Serial.println(" 6              ");
            num = updateNum(6);
            Serial.println(num);
            break;
        
          case 0xFF42BD:
            cont1++;    
            Serial.println(" 7              ");
            num = updateNum(7);
            Serial.println(num);
            break;
        
          case 0xFF4AB5: 
            cont1++;   
            Serial.println(" 8              ");
            num = updateNum(8);
            Serial.println(num);
            break;
        
          case 0xFF52AD: 
            cont1++;   
            Serial.println(" 9              ");
            num = updateNum(9);
            Serial.println(num);
            break;
        
          default:
            Serial.print(" unknown button   ");
            Serial.println(results.value, HEX);
        }
  
        if (cont1 == 4)
        {
          cont1 = 0;
        }
        
        irrecv.resume();
      }
    }

    lcd.clear();
    messageLCD(String(num), 0, 1);
    delay(1000);
    
    lcd.clear();
    delay(1000);
  }  
}
