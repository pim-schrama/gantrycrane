#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "gridfinding_def.h"

// === Definitie globale variabelen ===
volatile int startKnop = 0;
volatile int startSlot = 0;

volatile int homeSenderDone = 0;
volatile int heenTerug = 1;

volatile int xNu = 0, yNu = 0;
volatile int xEind = 0, yEind = 0;
volatile int xEindDropOf = 0, yEindDropOf = 0;

volatile int xEind2 = 0, yEind2 = 0;
volatile int xEindDropOf2 = 0, yEindDropOf2 = 0;

volatile int tweedeBlokjeNeer = 0;

// Statusflags
volatile int infoEindPosOpgehaald = 0;
volatile int infoEindPosOpgehaald2 = 0;

// === Timer1 init ===
void init_timer1(void) {
    TCCR1A = 0;
    TCCR1B = 0b00000101; // prescaler 1024
    TIMSK1 = 0b00000001; // overflow interrupt enable
    sei();
    TCNT1 = 63973;       // startwaarde voor ~10 ms
}

// === Keypad init ===
void keypad_init(void) {
    ROW_DDR = 0x0F;   // rijen als output
    ROW_PORT = 0x00;
    COL_DDR &= ~(0x0F); // kolommen als input
    COL_PORT |= 0x0F;   // pull-ups aan


}

// === Timer ISR ===
ISR(TIMER1_OVF_vect) {
    if(!(PIN_NoodKnop & (1 << PIN_NoodKnop))){
        while(1){
            //eventueel knipperend ledje

            if((PIN_NoodKnop & (1 << PIN_NoodKnop))) break;
        }
    }
    xNuFinder();
    yNuFinder();
    TCNT1 = 63973; // reset timer
}

// === Main loop ===
int main(void) {
    init_timer1();
    keypad_init();

    while (1) {
        if ((PIN_pos_XY & (1 << pos_X1)) || (startKnop == 1)) {
            startKnop = 1;

            if (homeSenderDone == 0) homeSender();

            if((infoEindPosOpgehaald == 0) || (infoEindPosOpgehaald2 == 0)) pickUp_and_DropOff_pos();

            if(PIN_SwitchTweedeCoord & (1 << pinSwitchTweedeCoord)){
                if ((infoEindPosOpgehaald == 1) && (startSlot == 1) && (infoEindPosOpgehaald2 == 1)) {
                    motorX(xNu_TOV_xEind(xNu, xEind));
                    motorY(yNu_TOV_yEind(yNu, yEind));
                }
            }else{
                if ((infoEindPosOpgehaald == 1) && (startSlot == 1)) {
                    motorX(xNu_TOV_xEind(xNu, xEind));
                    motorY(yNu_TOV_yEind(yNu, yEind));
                }
            }

            if ((xNu == xEind) && (yNu == yEind) && (startSlot == 1)) {
                motorZ(heenTerug);

            }
        }
    }
    return 0;
}

//to do: infoEindPosOpgehaald .... fixen
//if voor of de switch coord2 is ingedrukt en daar boundries voor zetten
//code om nog twee keer door de code heen te gaan
