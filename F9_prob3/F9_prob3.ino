/* Francisco Relvão, João Fernandes e Sílvia Santos -> Grupo C (TP2)
 * Exercício: Implementação de um circuito que permita a escrita de
 * números com um máximo de quatro dígitos num display de 7 segmentos
 * 
 * Programa: Em cada iteração do loop(), o sensor de infravermelhos
 * aguarda por um sinal proveniente do comando e associa o código
 * hexadecimal do sinal ao dígito do comando, guardando numa variável
 * o valor do número formado pelo conjunto de dígitos selecionados até
 * ao momento. Enquanto a tecla PLAY não for selecionada, o número 
 * guardado não é mostrado no display e o programa acrescenta o dígito
 * selecionado à direita do número guardado.
*/
#include <SevSeg.h>
#include <IRremote.h>

// criação de um objeto sevseg para controlar o display de sete segmentos
SevSeg sevseg; 

// variável responsável pela contagem do número de dígitos introduzidos
int cont1 = 0;

/*
 * As 4 variáveis seguintes guardam o valor calculado do número atualizado,
 * após a introdução do 1º, 2º, 3º e 4º dígitos, respetivamente, ou seja, 
 * oneDig guarda o número formado pelo 1º dígito, após a introdução do 1º dígito;
 * twoDig guarda o número formado pelo 1º e 2º dígitos, após a introdução do 2º dígito;
 * ...
 */
 
int oneDig;

int twoDig;

int threeDig;

int fourDig;

// variável devolvida pelo método updateNum(), que é responsável pela atualização do número a ser apresentado no display
int tempNum;

/* 
 * variável que armazena o número atualizado após cada dígito selecionado,
 * e o seu valor é ultimamente apresentado no display ao premir o botão PLAY
  */
int num;

// pin do sensor de infravermelhos
int pin = 9;

IRrecv irrecv(pin);

decode_results results;

void setup()
{
  // número de dígitos do display
  byte numDigits = 4;
  
  // array com os pins a que os dígitos do display estão ligados
  byte digitPins[] = {10, 11, 12, 13};
  
  // array com os pins a que os segmentos do display estão ligados
  byte segmentPins[] = {2, 3, 4, 5, 6, 7, 8};
  
  /*
   * A variável seguinte exige o conhecimento do tipo de display em causa: cátodo-comum ou ânodo-comum.
   * Os displays de cátodo-comum têm todos os cátodos dos segmentos ligados à terra, enquanto que
   * os displays de ânodo-comum têm todos os ânodos dos segmentos ligados à fonte de alimentação.
   * Segundo a datasheet, o display utilizado neste exercício é do tipo ânodo comum.
   */
  byte hardwareConfig = COMMON_ANODE;
  
  //guarda true se os segmentos estão ligados às portas através de resistências, e guarda false caso contrário.
  bool resistorsOnSegments = true;
  
  // inicializa o display
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments); 
  
  // inicializa o sensor 
  irrecv.enableIRIn();

  Serial.begin(9600);
}

void loop()
{ 
  // A cada nova iteração do loop() corresponde um novo número criado pelo utilizador
  cont1 = 0;

  // Igualando results.value a um valor arbitrário, a entrada no ciclo while é
  // garantida, mesmo que a última tecla premida pelo utilizador tenha sido o PLAY
  results.value = 0xFF629D;

  /*
   * A correspondência utilizada entre o número hexadecimal associado ao sinal detetado pelo
   * sensor e o número selecionado no comando encontra-se no seguinte link:
   * https://create.arduino.cc/editor/LogMaker360/d9ec220a-8a3c-4403-897c-e8bf0ff188df/preview
   * O ciclo while que se segue realiza esta correspondência e atualiza o número a ser mostrado no display.
   */
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

      // Quando o comprimento do número chega a 4, faz-se reset do número
      if (cont1 == 4)
      {
        cont1 = 0;
      }
      
      // torna possível a receção do próximo sinal enviado pelo comando
      irrecv.resume();
    }

    // permite mostrar o número final de forma contínua
    sevseg.refreshDisplay();
  }

  Serial.println(num);

  // envia o número final para ser mostrado no display
  sevseg.setNumber(num);    
}

/*
 * atualiza o número a ser mostrado no display,
 * após cada clique nos botões de 0-9 do comando
 */
int updateNum(int digit)
{ 
  /*
   * O número atualizado é dado pela soma entre 
   * 10 vezes o número guardado e o novo dígito.
   */
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
