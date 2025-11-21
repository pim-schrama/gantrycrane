#ifndef GRIDFINDING_DEF_H_INCLUDED
#define GRIDFINDING_DEF_H_INCLUDED

#include <avr/io.h>

#define F_CPU 16000000UL

#include <stdio.h>

// === Globale variabelen ===
extern volatile int xNow;
extern volatile int yNow;
extern volatile int xEnd;
extern volatile int yEnd;

extern volatile int xEndDropOff;
extern volatile int yEndDropOff;

extern volatile int xEnd2, yEnd2;
extern volatile int xEndDropOff2, yEndDropOff2;

extern volatile int secondObjectDown;

extern volatile int inputEndPosRetrieved;
extern volatile int inputEndPosRetrieved2;
extern volatile int homeSenderDone;

extern volatile int startButton;
extern volatile int startBlock;

extern volatile int backAndForth;

// === Manual ints ===
extern volatile int centerY;
extern volatile int centerX;

extern volatile int adcY;
extern volatile int valueY;

extern volatile int adcX;
extern volatile int valueX;

extern volatile int ampereSensor;
extern volatile float currentZAxis;
extern volatile float voltageZAxis;

// === Pinnen manual ===
#define pin_Manual PF3
#define PORT_Manual PORTF
#define PIN_Manual PINF

// === Pinnen start ===
#define pin_StartButton PF1
#define port_StartButton PORTF
#define PIN_StartButton PINF

// === Pinnen magneet ===
#define pin_Magnet PF6
#define port_Magnet PORTF

// === Pinnen noodknop ===
#define pin_EmergencyButton PF0
#define port_EmergencyButton PORTF
#define PIN_EmergencyButton  PINF

// === Pinnen switch tweede coordinaten ===
#define pin_SwitchSecondCoord PF5
#define port_SwitchSecondCoord PORTF
#define PIN_SwitchSecondCoord  PINF

// switches X-pos
#define pos_X1 PA1
#define pos_X2 PA3
#define pos_X3 PA5
#define pos_X4 PA0
#define pos_X5 PA2

// switches Y-pos
#define pos_Y1 PA7
#define pos_Y2 PC6
#define pos_Y3 PC4
#define pos_Y4 PA6
#define pos_Y5 PC7

#define port_pos_XY PORTA
#define port_pos_Y  PORTC

// switches Z-pos
// #define pos_Z PF2
// #define PORT_pos_Z  PORTF

// H-brug pinnen x/y
#define pin_HBridgeRightX PC3
#define pin_HBridgeRightY PC1
#define pin_HBridgeLeftX  PC2
#define pin_HBridgeLeftY  PC0

#define port_HBridgeX PORTC
#define port_HBridgeY PORTC

// H-brug pinnen z
#define pin_HBridgeRightZ PD7
#define pin_HBridgeLeftZ PG2

#define port_HBridgeZRight PORTD
#define port_HBridgeZLeft PORTG

// pin check
#define PIN_pos_XY  PINA
#define PIN_pos_Y  PINC
#define PIN_pos_Z  PINF


// Keypad
#define ROWS 4
#define COLS 4

#define ROW_PORT PORTD
#define ROW_PIN  PIND
#define ROW_DDR  DDRD

#define COL_PORT PORTL
#define COL_PIN  PINL
#define COL_DDR  DDRL

// Joystick
#define pin_Switch_Joystick PB7
#define PORT_Switch_Joystick PORTB
#define PIN_Switch_Joystick PINB

// ADC
#define adcSamples 25
#define zAxisResistance 0
#define zAxisNoResistance 0

// === Enums ===

enum MagnetState {
    Get = 1,
    Drop = 2
};

// === Functieprototypes ===

// Position Detection
void x_pos_finder(void);
void y_pos_finder(void);

// Keypad Handling
int keypad_getkey(void);
void processKey(int key);
void pickup_dropoff_pos(void);
void init_keypad(void);

// Motor Control
int motor_x_axis(int richting);
int motor_y_axis(int richting);
int motor_z_axis(int opNeer);

void motors_off(void);

// Position / Direction Comparison
int xNow_to_xEnd_comp(int nu, int eind);
int yNow_to_yEnd_comp(int nu, int eind);

// Homing & Coordination
void homeing_program(void);
void coord_switch(void);

// Magnet Handling
int pickup_program(enum MagnetState);

// Reset / End of Cycle
void end_program(void);

// Timer
void init_timer3(void);

// Joystick
int adc_read(int ch);

int adc_average(uint8_t ch, int samples);

void pwmX_stop(void);
void pwmY_stop(void);
void pwmX_start(void);
void pwmY_start(void);


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
