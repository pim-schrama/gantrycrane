#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "gridfinding_def.h"

// === Definitie globale variabelen ===
volatile int startButton = 0;
volatile int startBlock = 0;

volatile int homeSenderDone = 0;
volatile int backAndForth = 1;

volatile int xNow = 0, yNow = 0;
volatile int xEnd = 0, yEnd = 0;
volatile int xEndDropOff = 0, yEndDropOff = 0;

volatile int xEnd2 = 0, yEnd2 = 0;
volatile int xEndDropOff2 = 0, yEndDropOff2 = 0;

volatile int secondObjectDown = 0;

// Statusflags
volatile int inputEndPosRetrieved = 0;
volatile int inputEndPosRetrieved2 = 0;

volatile int ampereSensor = 0;
volatile float voltageZAxis = 0;
volatile float currentZAxis = 0;

int pwmStart = 0;

// === Crane init ===
void init_Crane(void) {
    DDRF = (1 << pin_Magnet);                                      // PF6 = output (magneet), rest input
    DDRC = (1 << PC0) | (1 << PC1) | (1 << PC2) | (1 << PC3);      // PC0–PC3 = outputs (H-brug X/Y)
    DDRD = (1 << PD0) | (1 << PD1) | (1 << PD2) | (1 << PD3) | (1 << PD7); // PD0–PD3 = keypad rows, PD7 = Z H-brug
    DDRG = (1 << PG2);                                             // PG2 = output (H-brug Z linksom)
    DDRA = 0x00;                                                   // alle PA0–PA7 = inputs (positie-switches)
    DDRB = 0x00;                                                   // niet gebruikt, alles input

                                                             printf("init_crane_out\n");
}

// === Timer1 init ===
void init_timer3(void) {
    TCCR3A = 0;
    TCCR3B = 0b00000101; // prescaler 1024
    TIMSK3 = 0b00000001; // overflow interrupt enable
    sei();
    TCNT3 = 63973;       // startwaarde voor ~10 ms

                                                             printf("init_timer_out\n");
}

// === Keypad init ===
void init_keypad(void) {
    ROW_DDR  |= 0x0F;   // PD0–PD3 as outputs
    ROW_PORT |= 0x0F;   // default HIGH (inactive)

    COL_DDR  &= ~0x0F;  // PC0–PC3 as inputs
    COL_PORT |= 0x0F;   // enable pull-ups

                                                             printf("init_keypad_out\n");
}

// === Timer ISR ===
ISR(TIMER3_OVF_vect) {
    if (!(PIN_EmergencyButton & (1 << pin_EmergencyButton))){
        printf("Nood_in\n");
        while(!(PIN_EmergencyButton & (1 << pin_EmergencyButton))){
            motors_off();
        }
        printf("nood_uit\n");

    }else{
                                                            // printf("ISR_OUT\n");
        x_pos_finder();
        y_pos_finder();
    }
    TCNT3 = 63973; // reset timer
}

// === Main loop ===
int main(void) {

                                                                                     usart0_init(115200);
                                                                                     stdout = &usart0_stdout; // enable printf to UART;

                                                                                     printf("Boot OK\n");

    init_Crane();
    init_timer3();
    init_keypad();
    init_joystick();

    pwmX_stop();
    pwmY_stop();

    while (1) {
        if (((PIN_StartButton & (1 << pin_StartButton)) || (startButton == 1)) && (!(PIN_Manual & (1 << pin_Manual)))) {
                                                                                                         printf("startButton\n");
            pwmX_stop();
            pwmY_stop();

            startButton = 1;

            if (homeSenderDone == 0) homeing_program();

            if((inputEndPosRetrieved == 0) || (inputEndPosRetrieved2 == 0)) pickup_dropoff_pos();

            if((PIN_SwitchSecondCoord & (1 << pin_SwitchSecondCoord))){
                if ((inputEndPosRetrieved == 1) && (startBlock == 1) && (inputEndPosRetrieved2 == 1)) {
                   while((xNow != xEnd) || (yNow != yEnd)){
                        motor_x_axis(xNow_to_xEnd_comp(xNow, xEnd));
                        motor_y_axis(yNow_to_yEnd_comp(yNow, yEnd));
                   }

                   port_HBridgeX &= ~((1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX));
                   port_HBridgeY &= ~((1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY));

                }
            }else{
                if ((inputEndPosRetrieved == 1) && (startBlock == 1)) {
                    while((xNow != xEnd) || (yNow != yEnd)){
                        motor_x_axis(xNow_to_xEnd_comp(xNow, xEnd));
                        motor_y_axis(yNow_to_yEnd_comp(yNow, yEnd));
                   }

                   port_HBridgeX &= ~((1 << pin_HBridgeRightX) | (1 << pin_HBridgeLeftX));
                   port_HBridgeY &= ~((1 << pin_HBridgeRightY) | (1 << pin_HBridgeLeftY));
                }
            }

            if ((xNow == xEnd) && (yNow == yEnd) && (startBlock == 1)) {
                motor_z_axis(backAndForth);

            }
        }else if((PIN_Manual & (1 << pin_Manual)) && (startButton != 1)){
            printf("manual in\n");

           if (pwmStart != 1){ pwmX_start(); pwmY_start(); pwmStart = 1;}

           adcY = adc_read(9);
           valueY = adcY - centerY;

           adcX = adc_read(8);
           valueX = adcX - centerX;

           x_axis_manual();
           y_axis_manual();
           z_axis_manual();

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

//       pos_Z & pos_Z2 inpluggen en een led ipv magneet


