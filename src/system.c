#include "system.h"

//////////////////////////////////////////////////////////////////////
// PIC16F1459 CONFIG bits
//////////////////////////////////////////////////////////////////////

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection Bits (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover Mode (Internal/External Switchover Mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config CPUDIV = NOCLKDIV// CPU System Clock Selection Bit (NO CPU system divide)
#pragma config USBLSCLK = 48MHz // USB Low SPeed Clock Selection bit (System clock expects 48 MHz, FS/LS USB CLKENs divide-by is set to 8.)
#pragma config PLLMULT = 3x     // PLL Multipler Selection Bit (3x Output Frequency Selected)
#pragma config PLLEN = ENABLED  // PLL Enable Bit (3x or 4x PLL Enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

/**
 * Initialize framework to given state.
 *
 * @state state to initialize the system into
 */
void
sys_init(SYSTEM_STATE state) {
    switch (state) {
    case SYSTEM_STATE_USB_START:
        // Enable active clock tuning for USB full speed with INTOSC
        OSCCON = 0xFC; //HFINTOSC @ 16MHz, 3X PLL, PLL enabled
        ACTCON = 0x90; //Active clock tuning enabled for USB

        // Port direction (0:output, 1:input)
        TRISA = 0b11001111; // RA[45] for output, others for USR/MCLR/NC
	TRISB = 0b10101111; // RB[46] for output, others for UART/NC
	TRISC = 0b00000000; // all pins for output

        // configure baudrate (= Fosc / (64 * (SPBRG[HL] + 1)))
        SPBRGH = 0;
        SPBRGL = 25;
        BAUDCONbits.BRG16 = 0;
        TXSTAbits.BRGH = 0;

        // enable UART
        TXSTAbits.TXEN = 1; // TX at RB7
        TXSTAbits.SYNC = 0;
        RCSTAbits.CREN = 1; // RX at RB5
        RCSTAbits.SPEN = 1;

        // timer setup
        T1CONbits.TMR1ON = 1;
        TMR1H = 0x00;
        TMR1L = 0x00;

        // global interrupt setup
        PIE1bits.TMR1IE = 1; // TIM1 overflow INT
        INTCONbits.PEIE = 1; // Peripheral INT
        INTCONbits.GIE  = 1; // GLobal INT
        break;
            
    case SYSTEM_STATE_USB_SUSPEND: 
        break;
            
    case SYSTEM_STATE_USB_RESUME:
        break;
    }
}
