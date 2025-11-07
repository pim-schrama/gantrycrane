#ifndef GRIDFINDING_DEF_H_INCLUDED
#define GRIDFINDING_DEF_H_INCLUDED

#include <avr/io.h>

#define F_CPU 16000000UL

#include <stdio.h>

// === Globale variabelen ===
extern volatile int xNu;
extern volatile int yNu;
extern volatile int xEind;
extern volatile int yEind;

extern volatile int xEindDropOf;
extern volatile int yEindDropOf;

extern volatile int xEind2, yEind2;
extern volatile int xEindDropOf2, yEindDropOf2;

extern volatile int tweedeBlokjeNeer;

extern volatile int infoEindPosOpgehaald;
extern volatile int infoEindPosOpgehaald2;
extern volatile int homeSenderDone;

extern volatile int startKnop;
extern volatile int startSlot;

extern volatile int heenTerug;


// === Pinnen start ===
#define pinStartKnop PF6
#define portStartKnop PORTF

// === Pinnen magneet ===
#define pinMagneet PF6
#define portMagneet PORTF

// === Pinnen noodknop ===
#define pinNoodKnop PC4
#define portNoodKnop PORTC
#define PIN_NoodKnop  PINC

// === Pinnen switch tweede coordinaten ===
#define pinSwitchTweedeCoord PC5
#define portSwitchTweedeCoord PORTC
#define PIN_SwitchTweedeCoord  PINC

// switches X-pos
#define pos_X1 PB0
#define pos_X2 PB1
#define pos_X3 PB2
#define pos_X4 PB3
#define pos_X5 PB4

// switches Y-pos
#define pos_Y1 PB5
#define pos_Y2 PB6
#define pos_Y3 PB7
#define pos_Y4 PF0
#define pos_Y5 PF1

#define PORT_pos_XY PORTB
#define PORT_pos_Y  PORTF

// switches Z-pos
#define pos_Z PA0
#define pos_Z2 PA1

#define PORT_pos_Z  PORTA

// H-brug pinnen x/y
#define pinHBrug_RechtsOm_X PF2
#define pinHBrug_RechtsOm_Y PF3
#define pinHBrug_LinksOm_X  PF4
#define pinHBrug_LinksOm_Y  PF5

#define portHBrug_X PORTF
#define portHBrug_Y PORTF

// H-brug pinnen z
#define pinHBrug_RechtsOm_Z PA2
#define pinHBrug_LinksOm_Z PA3

#define portHBrug_Z PORTA

// pin check
#define PIN_pos_XY  PINB
#define PIN_pos_Y  PINF
#define PIN_pos_Z  PINA


// Keypad
#define ROWS 4
#define COLS 4

#define ROW_PORT PORTD
#define ROW_PIN  PIND
#define ROW_DDR  DDRD

#define COL_PORT PORTL
#define COL_PIN  PINL
#define COL_DDR  DDRL

// === Enums ===

enum MagnetState {
    Get = 1,
    Drop = 2
};

// === Functieprototypes ===

// Position Detection
void xNuFinder(void);
void yNuFinder(void);

// Keypad Handling
int keypad_getkey(void);
void processKey(int key);
void pickUp_and_DropOff_pos(void);
void keypad_init(void);

// Motor Control
int motorX(int richting);
int motorY(int richting);
int motorZ(int opNeer);

void motorenUit(void);

// Position / Direction Comparison
int xNu_TOV_xEind(int nu, int eind);
int yNu_TOV_yEind(int nu, int eind);

// Homing & Coordination
void homeSender(void);
void coordSwitch(void);

// Magnet Handling
int oppakProgramma(enum MagnetState);

// Reset / End of Cycle
void eindProgramma(void);

// Timer
void init_timer1(void);



// ---------- UART0 setup (logging) ----------
static void usart0_init(uint32_t baud)
{
    UCSR0A = _BV(U2X0); // normal speed
    uint16_t ubrr = (F_CPU / (8UL * baud)) - 1;


    //uint16_t ubrr = (F_CPU / (16UL * baud)) - 1;
    //UCSR0A = 0; // normal speed
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr);
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);          // enable TX,RX
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);        // 8 data, 1 stop, no parity
}

static void usart0_write_char(char c)
{
    while (!(UCSR0A & _BV(UDRE0))) { }         // wait until ready
    UDR0 = c;
}

// Glue so printf works
static int usart0_putchar(char c, FILE *stream)
{
    if (c == '\n') {
        usart0_write_char('\r');
    }
    usart0_write_char(c);
    return 0;
}

// Declare a FILE object
static FILE usart0_stdout = FDEV_SETUP_STREAM(usart0_putchar, NULL, _FDEV_SETUP_WRITE);


#endif
