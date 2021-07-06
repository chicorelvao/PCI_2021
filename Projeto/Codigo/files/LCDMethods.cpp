#include "Interface.h"

byte lockSymbol(){
    byte lock[8] = {0b00100 ,
                    0b01010 ,
                    0b01010 ,
                    0b01010 ,
                    0b11111 ,
                    0b11111 ,
                    0b11111 ,
                    0b00000};
    return lock[8];
}
