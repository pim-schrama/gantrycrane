#ifndef GRIDFINDING_DEF_H_INCLUDED
#define GRIDFINDING_DEF_H_INCLUDED

#include <avr/io.h>

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
#define pinMagneet PD7
#define portMagneet PORTD

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

// H-brug pinnen x/y
#define pinHBrug_RechtsOm_X PF2
#define pinHBrug_RechtsOm_Y PF3
#define pinHBrug_LinksOm_X  PF4
#define pinHBrug_LinksOm_Y  PF5

#define portHBrug_X PORTF
#define portHBrug_Y PORTF

// H-brug pinnen z
#define pinHBrug_RechtsOm_Z PA1
#define pinHBrug_LinksOm_Z PA2

#define portHBrug_Z PORTD

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

#define COL_PORT PORTC
#define COL_PIN  PINC
#define COL_DDR  DDRC

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


#endif
