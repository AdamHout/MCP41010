/* 
 * File:   mcp41010.h
 * Author: adam
 *
 */

#ifndef MCP41010_H
#define	MCP41010_H

#ifdef	__cplusplus
extern "C" {
#endif

//MCP41xxx commands
#define CMD_WRITE 0x1100                                                        //Write to pot
#define CMD_SHTDN 0x2100                                                        //Shutdown pot
    
//Pot constants
#define MAX_ADDR    0xFF                                                        //Max pot address/tap
#define WIPER_OHMS  0x34                                                        //Wiper resistance - 52 ohms
#define POT_OHMS    0x2710                                                      //Pot resistance - 10K ohms
    
//Chip select macros
#define CS_SetLow()    (LATBbits.LATB6 = 0)
#define CS_SetHigh()   (LATBbits.LATB6 = 1)
    

//Function declarations
void initDigPot(void);
void writeDigPot(uint16_t, uint8_t);
void incrementDigPot(uint8_t);
void decrementDigPot(uint8_t);
uint16_t calculateDigPot(void); 

#ifdef	__cplusplus
}
#endif

#endif	/* MCP41010_H */

