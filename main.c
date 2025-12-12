#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include "librarys/LiquidCrystal_I2C.h"
#include "gridfinding_def.h"

// === Definitie globale variabelen ===
volatile int startButton = 0;
volatile int startBlock = 0;

volatile int homeSenderDone = 0;
volatile int backAndForth = 1;

volatile int xNow = 0, yNow = 0;
volatile int xEnd = 0, yEnd = 0;

// Statusflags
volatile int inputEndPosRetrieved = 0;

volatile float currentZAxis = 0;

volatile int blockCountRecieved = 0;
volatile int blocks = 0;
volatile int coordCount = 0, coordCountPickDrop = 0;

volatile int xEndArr[MAX_BLOCKS + 1];
volatile int yEndArr[MAX_BLOCKS + 1];
volatile int xEndDropOffArr[MAX_BLOCKS + 1];
volatile int yEndDropOffArr[MAX_BLOCKS + 1];

// === Crane init ===
void init_Crane(void) {
// ==== PORT F ====
// Inputs: Manual, StartButton, SwitchSecondCoord
DDRF &= ~((1 << pin_Manual) | (1 << pin_StartButton) | (1 << pin_SwitchSecondCoord));
// Output: Magnet
DDRF |=  (1 << pin_Magnet);
// ==== PORT K ====
// Input: EmergencyButton
DDRK &= ~(1 << pin_EmergencyButton);
// ==== PORT A ====
// Inputs: X-position switches + Y2 + Y5
DDRA &= ~((1 << pos_X1) | (1 << pos_X2) | (1 << pos_X3) | (1 << pos_X4) | (1 << pos_X5) | (1 << pos_Y2) | (1 << pos_Y5));
// ==== PORT C ====
// Inputs: remaining Y switches + keypad rows? (User did not mark rows as outputs)
DDRC &= ~((1 << pos_Y1) | (1 << pos_Y3) | (1 << pos_Y4));
// ==== PORT G ====
// Outputs: H-bridge X
DDRG |= (1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX);
// ==== PORT L ====
// Outputs: H-bridge Y + H-bridge Z
DDRL |= (1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY)  | (1 << pin_HBridgeRightZ) | (1 << pin_HBridgeLeftZ);
// ==== PORT B ====
// Input: Joystick switch
DDRB &= ~(1 << pin_Switch_Joystick);

                                                             printf("init_crane_out\n");
}

/*
// === Timer1 init ===
void init_timer3(void) {
    TCCR3A = 0;
    //TCCR3B = 0b00000010;    // prescaler 8
    TCCR3B = 0b00000101;    // prescaler 256
    TIMSK3 = 0b00000001;    // overflow interrupt aan
    TCNT3 = 64911;          // startwaarde voor ~10 ms met 256
    //TCNT3 = 45536;          // startwaarde voor ~10 ms met 8

    sei();                  // interupts aan
                                                             printf("init_timer_out\n");
}
*/

// === Keypad init ===
void init_keypad(void) {
    ROW_DDR  |= 0x0F;       // PD0–PD3 als outputs
    ROW_PORT |= 0x0F;       // standaard HIGH (inactief)

    COL_DDR  &= ~0x0F;      // PC0–PC3 als inputs
    COL_PORT |= 0x0F;       // enable pull-ups

                                                             printf("init_keypad_out\n");
}

void init_PCINT_interupt(void){
    PCICR |= (1 << PCIE2);          // Enable PCINT[23:16] (PORTK)
    PCMSK2 |= (1 << PCINT16);       // Enable PCINT16 on PK0
    DDRK &= ~(1 << DDK0);           // PK0 input

    PCICR  |= (1 << PCIE1);       // Enable PCINT[15:8] (PORTJ)
    PCMSK1 |= (1 << PCINT10);     // Enable PCINT10 on PJ1
    PCMSK1 |= (1 << PCINT9);     // Enable PCINT9 on PJ0
    DDRJ &= ~(1 << DDJ1);         // PJ1 input
    DDRJ &= ~(1 << DDJ0);         // PJ0 input


    sei();
                                                             printf("init_PCINT_out\n");
}
/*
// === Timer ISR ===
ISR(TIMER3_OVF_vect) {

    TCNT3 = 64911;      // reset timer
    //TCNT3 = 45536;
}
*/
ISR(PCINT1_vect)
{
    //detected change in PCINT[15:8]
    if (PINJ & (1 << PINJ1)) {
        x_pos_finder();     // X/Y positie check
    }
    if (PINJ & (1 << PINJ0)) {
        y_pos_finder();
    }
}

ISR(PCINT2_vect)
{
    if (PINK & (1 << PINK0)) {
        // PK0 is HIGH doe niks
    } else {
        cli();
        if (!(PIN_EmergencyButton & (1 << pin_EmergencyButton))){
                                                                                printf("Nood_in\n");
            while(!(PIN_EmergencyButton & (1 << pin_EmergencyButton))){
                motors_off();
            }
            sei();
                                                                                printf("nood_uit\n");
            while(!(PIN_StartButton & (1 << pin_StartButton)));
        }
    }

/*  if (PINK & (1 << PINK1)){
        // PK1 is HIGH doe niks
    }else{
        motors_off();
        manual_main();
    }*/
}


// === Main loop ===
int main(void) {

    usart0_init(115200);
    stdout = &usart0_stdout; // enable printf to UART;

                                                                                     printf("Boot OK\n");
    init_Crane();
    init_PCINT_interupt();
    //init_timer3();
    init_keypad();
    init_joystick();
    init_adc();

    pwmX_stop();
    pwmY_stop();

    while (1) {
        if (((PIN_StartButton & (1 << pin_StartButton)) || (startButton == 1)) && (!(PIN_Manual & (1 << pin_Manual)))) {
                                                                                                                                printf("startButton\n");
            pwmX_stop();
            pwmY_stop();

            startButton = 1;

            if (homeSenderDone == 0) homeing_program();

            if(!blockCountRecieved) {
                block_count();
            }

            if(inputEndPosRetrieved == 0) pickup_dropoff_pos();

            if ((inputEndPosRetrieved == 1) && (startBlock == 1)) {

                while((xNow != xEnd) || (yNow != yEnd)){
                    motor_x_axis(xNow_to_xEnd_comp(xNow, xEnd));
                    motor_y_axis(yNow_to_yEnd_comp(yNow, yEnd));
                }

                port_HBridgeX &= ~((1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX));
                port_HBridgeY &= ~((1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY));
            }

            if ((xNow == xEnd) && (yNow == yEnd) && (startBlock == 1)) {
                                                                                                        printf("motorenZ\n");
                motor_z_axis(backAndForth);

            }

        }else if((PIN_Manual & (1 << pin_Manual)) && (startButton != 1)){
            manual_main();
        }
    }
    return 0;
}

//to do: noodknop laten wachten. en eventueel ledje laten knipperen
//       4 extra knoppen voor als motor (voor welke reden ook) het einde berijkt.

//       ampere sensor ipv 2 knoppen op z-as

//       (moet in alle motor loops met een break; of een lijn die de code uit zet.
//       extra: een max tijd dat de magneet aan mag bijven tegen oververhitting.
//       extra: gelijdelijke stop voor motor (tegen het schudden.)
