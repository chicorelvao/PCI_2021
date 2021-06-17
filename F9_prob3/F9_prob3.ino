#include "SevSeg.h"
SevSeg sevseg; 

int cont1 = 0;

int num;

int tempNum;

int oneDig;

int twoDig;

int threeDig;

int fourDig;

#include <IRremote.h>
int pin = 9;
IRrecv irrecv(pin);
decode_results results;

void setup()
{
  byte numDigits = 4;
  byte digitPins[] = {10, 11, 12, 13};
  byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8};
  byte hardwareConfig = COMMON_ANODE;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, true); 
  
  Serial.begin(9600);
  pinMode(pin, INPUT);  
  irrecv.enableIRIn();
}

void loop()
{ 
  if (irrecv.decode(&results))
  {
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

      sevseg.refreshDisplay();
    }
    
    Serial.println(num);
    sevseg.setNumber(num);
    irrecv.resume();
  }     
}

// atualiza o número a ser mostrado no display,
// após cada clique nos botões de 0-9 do comando
int updateNum(int digit)
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
