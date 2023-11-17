/*
 * mcp41010 - Interface a PIC24F to a MCP41010 digipot via SPI
 *
 * *****************************************************************************
 * 05/23   Adam Hout        -Original source
 * ****************************************************************************
 */

    // CONFIG2
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = PRI            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = ON             // IOLOCK Protection (Once IOLOCK is set, cannot be changed)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC Oscillator)
#pragma config SOSCSEL = SOSC           // Sec Oscillator Select (Default Secondary Oscillator (SOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = ON                // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = ON              // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC2/PGD2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)


//Includes
#include "xc.h"
#include "sys.h"
#include "mcp41010.h"

int main(void) {
   
   uint16_t resistance;
   
   //MCP41010 demo logic
   initDigPot();                                                                //Initialize SPI and chip select pins
   writeDigPot(CMD_WRITE, 0xC0);                                                //Tap 192
   resistance = calculateDigPot();                                              //Compute the resistance
   
   incrementDigPot(10);                                                         //Tap 182
   resistance = calculateDigPot();
   
   decrementDigPot(20);                                                         //Tap 202
   resistance = calculateDigPot();
   
   writeDigPot(CMD_WRITE, 0x00);                                                //Tap 0
   resistance = calculateDigPot();                                              //Max resistance
   
   writeDigPot(CMD_WRITE, MAX_ADDR);                                            //Just the wiper resistance
   resistance = calculateDigPot();
   
   writeDigPot(CMD_SHTDN, 0x00);                                                //Software shutdown - data byte ignored
   
   while(1);                                                                    //Application logic would go here                                                              

   return 0;
}
