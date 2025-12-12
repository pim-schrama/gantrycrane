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

//extern volatile int xEndDropOff;
//extern volatile int yEndDropOff;

//extern volatile int xEnd2, yEnd2;
//extern volatile int xEndDropOff2, yEndDropOff2;

//extern volatile int secondObjectDown;

extern volatile int inputEndPosRetrieved;
//extern volatile int inputEndPosRetrieved2;
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

extern volatile float currentZAxis;

extern volatile int blockCountRecieved;
extern volatile int blocks;
extern volatile int coordCount;
extern volatile int coordCountPickDrop;

extern volatile int xEndArr[];
extern volatile int yEndArr[];
extern volatile int xEndDropOffArr[];
extern volatile int yEndDropOffArr[];

// === Debounce ===
#define DEBOUNCE40 _delay_ms(40)
#define DEBOUNCE50 _delay_ms(50)
#define DEBOUNCE60 _delay_ms(60)

// === Pinnen manual ===
#define pin_Manual PF2
#define PORT_Manual PORTF
#define PIN_Manual PINF

// === Pinnen start ===
#define pin_StartButton PF0
#define port_StartButton PORTF
#define PIN_StartButton PINF

// === Pinnen magneet ===
#define pin_Magnet PF4
#define port_Magnet PORTF

// === Pinnen noodknop ===
#define pin_EmergencyButton PK0
#define port_EmergencyButton PORTK
#define PIN_EmergencyButton  PINK

// === Pinnen switch tweede coordinaten ===
#define pin_SwitchSecondCoord PF3
#define port_SwitchSecondCoord PORTF
#define PIN_SwitchSecondCoord  PINF

// switches X-pos
#define pos_X1 PA1
#define pos_X2 PA3
#define pos_X3 PA5
#define pos_X4 PA0
#define pos_X5 PA2

// switches Y-pos
#define pos_Y1 PC7
#define pos_Y2 PA6
#define pos_Y3 PC4
#define pos_Y4 PC6
#define pos_Y5 PA7

#define port_pos_XY PORTA
#define port_pos_Y  PORTC

// switches Z-pos
// #define pos_Z PF2
// #define PORT_pos_Z  PORTF

// H-brug pinnen x/y
#define pin_HBridgeRightX PG1
#define pin_HBridgeRightY PL6  //PG1
#define pin_HBridgeLeftX  PG0
#define pin_HBridgeLeftY  PL7   //PG0

#define port_HBridgeX PORTG
#define port_HBridgeY PORTL     //PORTG

// H-brug pinnen z
#define pin_HBridgeRightZ PL5   //PL7
#define pin_HBridgeLeftZ PL4    //PL6

#define port_HBridgeZ PORTL     //PORTL

// pin check
#define PIN_pos_XY  PINA
#define PIN_pos_Y  PINC
#define PIN_pos_Z  PINF


// Keypad
#define ROWS 4
#define COLS 4

#define ROW_PORT PORTC
#define ROW_PIN  PINC
#define ROW_DDR  DDRC

#define COL_PORT PORTL
#define COL_PIN  PINL
#define COL_DDR  DDRL

// Joystick
#define pin_Switch_Joystick PB7
#define PORT_Switch_Joystick PORTB
#define PIN_Switch_Joystick PINB

// ADC
#define adcSamples 50
#define resistanceCurrentZAxis 0 // >1.202 V <1.202
#define zAxisNoResistance 1750 //1.502  //1.202

#define MAX_BLOCKS 99

// === Enums ===

enum MagnetState {
    Get = 1,
    Drop = 2
};

// === Functieprototypes ===

// Position Detection
void x_pos_finder(void);
void y_pos_finder(void);
float ampereSensor(int pin, int samples);

// Keypad Handling
int keypad_getkey(void);
void processKey(int key);
void pickup_dropoff_pos(void);
void init_keypad(void);

// Motor Control
int motor_x_axis(int direction);
int motor_y_axis(int direction);
int motor_z_axis(int upDown);

void motors_off(void);

// Position / Direction Comparison
int xNow_to_xEnd_comp(int now, int end);
int yNow_to_yEnd_comp(int now, int end);

// Homing & Coordination
void homeing_program(void);
void coord_switch(void);

// Magnet Handling
int pickup_program(enum MagnetState);

// Reset / End of Cycle
void end_program(void);

// Timer
//void init_timer3(void);

// Joystick
void manual_main(void);

void init_adc(void);
void init_joystick(void);

void x_axis_manual(void);
void y_axis_manual(void);
void z_axis_manual(void);

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
