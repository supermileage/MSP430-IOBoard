// ****************************** PIN-OUT **************************************
//
// MSP430G2553 - Function - MCP2515
// ---------------------------------  
//    P1.4     -   !INT   -  Pin 12
//    P1.5     -   SCK    -  Pin 13
//    P1.6     -   MOSI   -  Pin 14
//    P1.7     -   MISO   -  Pin 15
//    P2.5     -   !CS    -  Pin 16
//
// *****************************************************************************

#include "mcp2515.h"


can_t can_tx;                                                                    // CAN-Send variable
can_t can_rx;                                                                    // CAN-Receive variable
  
int main(void)
{  
  // --- 1. MSP430-Stop Watchdog -----------------------------------------------
  
  WDTCTL = WDTPW + WDTHOLD;                                                      // Stop Watchdog-Timer 

  // --- 2. Set MSP430-Clock -------------------------------------------------
  
  if (CALBC1_1MHZ==0xFF) while(1);  					         // If calibration constant erased. Do not load, trap CPU!!	  
  DCOCTL = 0;                                                                    // Take the lowest frequency, DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                                                         // Range
  DCOCTL = CALBC1_1MHZ;                                                          // DCO-Step + Modulation 

  // --- 3. MSP430-Pin-IR and LED Adjusting -----------------------------------
  
  P1DIR |= BIT0;                                                                 // P1.0 as an exit (LED at the EXP430G2)                                                            
  P1OUT |=  BIT4;                                                                // P1.4 set
  P1REN |= BIT4;                                                                 // P1.4 Pullup
  P1IE |= BIT4;                                                                  // P1.4 Interrupt activate (only for this pin)
  P1IES |= BIT4;                                                                 // P1.4 Hi/lo Edge
  P1IFG &= ~BIT4;                                                                // P1.4 IFG Cleared 
  
  // --- 4. Init SPI, MMCP2515 and CAN-Variables -------------------------------
  
  MCP2515_SPI_init();                                                            // initializes SPI
  MCP2515_init();                                                                // initializes MCP2515
  MCP2515_CanVariable_init (&can_tx);                                            // initializes Send variable
  MCP2515_CanVariable_init (&can_rx);                                            // initializes Receive variable
  
  _EINT();                                                                       // Enable interrupts otherwise the MSP430 pin IR is useless
  
  while(1)                                                                       // endless loop
  { 
    P1OUT ^= BIT0;                                                               // P1.0 Toggle
    __delay_cycles(DELAY_1s);                                                    // Wait 1 Second
  } // while
}
    
// Port 1: Interrupt-Service-Routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  MCP2515_can_rx0(&can_rx);                                                      // Read information on receive channel RX0
  __delay_cycles(DELAY_10ms);                                                    // Wait 10ms
  MCP2515_can_tx0(&can_rx);                                                      // Send the received back (echo)
  P1IFG &= ~BIT4;                                                                // P1.4 IFG delete again
}
