/* 
 * Resources used:
 * -SP1 module and three pins (SDO, SCK, CS)
 * 
 * Summary:
 * -MCP41010 is a 10K ohm digital potentiometer 
 * -8 bit wiper; 256 taps; ~39 ohms per step
 * -Wiper is 52 ohms nominal
 * -Wiper defaults to 0x80 at power up
 * -Max current of 1ma when wiper value set to 0x00
 * -Supports SCK up to 10MHz
 * -16 bit shift register
 * -Software shutdown
 * 
 *        Command Byte
 *    ----------------------
 *    |X|X|C1|C0|X|X|P1*|P0|
 *    ----------------------
 * 
 *   C1|C0|Command        P1*|P0|Pot Selection
 *   -----------------------------------------
 *   0  0  Nop            0   0  Nop
 *   0  1  Write data     0   1  Command executed on pot 0
 *   1  0  Shutdown       1   0  Command executed on pot 1
 *   1  1  Nop            1   1  Command executed on both pots
 * 
 *   *P1 is a don't care bit on MCP41xxx's  
 * 
 * Calculate resistance:
 * Rwa = Rab(256-Dn)/256+Rw
 * Rwb = Rab(Dn)/256+Rw
 * 
 * Rab = Total pot resistance
 * Dn = Wiper position
 * Rw = Wiper resistance 
 * 
 * ****************************************************************************
 * 05/23   Adam Hout        -Original source
 * ****************************************************************************
 */

#include "xc.h"
#include "mcp41010.h"

volatile uint8_t wiperPosition = 0;                                             //Maintain current wiper position

//Initialize SP1 and chip select
void initDigPot(void){
   
   SPI1STATbits.SPIEN = 0;                                                      //Disable SPI module
   TRISB = TRISB & 0xFE3F;                                                      //Set RB6-8 as outputs
   CS_SetHigh();                                                                //Chip select active low
   
   //Configure PPS for our SPI pins
   __builtin_write_OSCCONL(OSCCON & 0xBF);                                      //Unlock PPS
   RPOR3bits.RP7R = 0x0008;                                                     //RB7->SPI1:SCK1OUT
   RPOR4bits.RP8R = 0x0007;                                                     //RB8->SPI1:SDO1
   __builtin_write_OSCCONL(OSCCON | 0x40);                                      //Lock PPS
    
   //MSTEN Master; PPRE 4:1; SPRE 4:1; MODE16 enabled; SMP Middle; CKP 0; CKE 1; 
   SPI1CON1 = 0x0532;                                                           //250KHz clock                                                     
   SPI1CON2 = 0x0000;                                                           //Legacy mode SPI
   SPI1STAT = 0x8000;                                                           //Enable SPI1
}

//Writes the command and data word to the MCP410010
void writeDigPot(uint16_t command, uint8_t value){
   
   uint16_t cmdWord;
   uint16_t junk;
   
   cmdWord = command | value;                                                   //Create the full command/data word                                                  
   CS_SetLow();                                                                 
   while(SPI1STATbits.SPITBF);                                                  //Ensure transmit buffer is clear                                                
   SPI1BUF = cmdWord;                                                           //Send the command/data word
   while(!SPI1STATbits.SPIRBF);                                                 //Junk shifted in as data shifted out
   CS_SetHigh();                                                                //Transmission complete
   junk = SPI1BUF;                                                              //Clear the buffer
   wiperPosition = value;                                                       //Save off current position
}

//Decrementing the wiper position increments the resistance
void incrementDigPot(uint8_t value){
   
   if((wiperPosition - value) < 0)
      return;
   
   wiperPosition -= value;
   writeDigPot(CMD_WRITE, wiperPosition);
}

//Incrementing the wiper position decrements the resistance
void decrementDigPot(uint8_t value){
   
   if((wiperPosition + value) > MAX_ADDR)
      return;
   
   wiperPosition += value;
   writeDigPot(CMD_WRITE, wiperPosition);
}

//Calculate the resistance at the current wiper position
uint16_t calculateDigPot(){
   
   uint32_t resistance;
   
   if(wiperPosition == 0)                                                       //No divide by zero
      return POT_OHMS + WIPER_OHMS;
   
   resistance = (uint32_t)POT_OHMS * (uint32_t)wiperPosition;                   
   resistance  = resistance >> 8;                                               //Divide by 256                                               
   resistance += WIPER_OHMS;
   return (uint16_t)resistance;
}


//void __attribute__ ((interrupt, no_auto_psv)) _SPI1Interrupt(void){
//   
//}