#define F_CPU 16000000UL

#include <stdio.h>
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

// === Crane init ===
void init_Crane(void) {
    DDRF = 0b00111100;
    DDRD = 0b10000011;
    DDRC = 0b00000000;
    DDRB = 0b00000000;
    DDRA = 0b00001111;

// === Interne pull-ups ===

    portNoodKnop |= (1 << pinNoodKnop);
    portStartKnop |= (1 << pinStartKnop);
    portSwitchTweedeCoord |= (1 << pinSwitchTweedeCoord);
    PORT_pos_XY |= (1 << pos_X1);
    PORT_pos_XY |= (1 << pos_X2);
    PORT_pos_XY |= (1 << pos_X3);
    PORT_pos_XY |= (1 << pos_X4);
    PORT_pos_XY |= (1 << pos_X5);
    PORT_pos_XY |= (1 << pos_Y1);
    PORT_pos_XY |= (1 << pos_Y2);
    PORT_pos_XY |= (1 << pos_Y3);
    PORT_pos_Y |= (1 << pos_Y4);
    PORT_pos_Y |= (1 << pos_Y5);
    PORT_pos_Z |= (1 << pos_Z);
    PORT_pos_Z |= (1 << pos_Z2);

    printf("init_crane_out\n");
}

// === Timer1 init ===
void init_timer1(void) {
    TCCR1A = 0;
    TCCR1B = 0b00000101; // prescaler 1024
    TIMSK1 = 0b00000001; // overflow interrupt enable
    sei();
    TCNT1 = 63973;       // startwaarde voor ~10 ms

    printf("init_timer_out\n");
}

// === Keypad init ===
void keypad_init(void) {
    ROW_DDR  |= 0x0F;   // PD0–PD3 as outputs
    ROW_PORT |= 0x0F;   // default HIGH (inactive)

    COL_DDR  &= ~0x0F;  // PC0–PC3 as inputs
    COL_PORT |= 0x0F;   // enable pull-ups

    printf("init_keypad_out\n");
}

// === Timer ISR ===
ISR(TIMER1_OVF_vect) {
    if ((PIN_NoodKnop & (1 << pinNoodKnop)) == 0){
        printf("Nood_in\n");
    }else{
       // printf("ISR_OUT\n");
        xNuFinder();
        yNuFinder();
    }
    TCNT1 = 63973; // reset timer
}

// ---------- UART0 setup ----------
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

// === Main loop ===
int main(void) {

    usart0_init(115200);
    stdout = &usart0_stdout; // enable printf to UART;

    printf("Boot OK\n");

    init_Crane();
    init_timer1();
    keypad_init();

    while (1) {
        if (((PINF & (1 << pinStartKnop)) == 0) || (startKnop == 1)) {
            printf("Startknop\n");
            startKnop = 1;

            if (homeSenderDone == 0) homeSender();

            if((infoEindPosOpgehaald == 0) || (infoEindPosOpgehaald2 == 0)) pickUp_and_DropOff_pos();

            if(!(PIN_SwitchTweedeCoord & (1 << pinSwitchTweedeCoord))){
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

//to do: logic motoren omdraaien


