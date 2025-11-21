#include "gridfinding_def.h"

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>

volatile int centerY = 0;
volatile int centerX = 0;

volatile int adcY = 0;
volatile int valueY = 0;

volatile int adcX = 0;
volatile int valueX = 0;


int z = 0;

void init_adc(void) {
    ADMUX = (1 << REFS0);                  // Vref = AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // prescaler = 128
}

void init_joystick(void) {
    DDRB |= (1 << PB5) | (1 << PB6) | (1 << PB4); // OC1A, OC1B, OC2A
    DDRH |= (1 << PH6); // OC2B

    //PORT_Switch_Joystick |= (1 << pin_Switch_Joystick);

    init_adc();

    centerY = adc_read(9);
    centerX = adc_read(8);
}

int adc_read(int ch) {
    ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

void pwmY_start(void) {
    // Fast PWM 10-bit, non-inverting on both outputs, prescaler = 8
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) |
             (1 << WGM11)  | (1 << WGM10);
    TCCR1B = (1 << WGM12)  | (1 << CS11);
}

void pwmX_start(void) {
    // Fast PWM 8-bit, non-inverting on both outputs, prescaler = 8
    TCCR2A = (1 << COM2A1) | (1 << COM2B1) |
             (1 << WGM21)  | (1 << WGM20);
    TCCR2B = (1 << CS21);
}

void pwmX_stop(void) {
    // Disable PWM outputs
    TCCR2A = 0;
    TCCR2B = 0;
    // Drive pins LOW manually
    PORTB &= ~(1 << PB4);
    PORTH &= ~(1 << PH6);
}

void pwmY_stop(void) {
    // Disable PWM outputs
    TCCR1A = 0;
    TCCR1B = 0;
    // Drive pins LOW manually
    PORTB &= ~((1 << PB5) | (1 << PB6));
}

void x_axis_manual(void){
    if ((adcX >= (centerX - 10)) && (adcX <= (centerX + 10))) {
        pwmX_stop();
    } else {
        pwmX_start();
        if(valueX > 0){
            DDRH &= ~(1<<PH6);
            DDRB |= (1<<PB4);
            OCR2A = (valueX / 4);
        }else{
            DDRB &= ~(1<<PB4);
            DDRH |= (1<<PH6);
            OCR2B = (abs(valueX) / 4);
        }
    }
}

void y_axis_manual(void){
    if ((adcY >= (centerY - 10)) && (adcY <= (centerY + 10))) {
        pwmY_stop();
    } else {
        pwmY_start();
        if(valueY > 0){
            DDRB &= ~(1<<PB5);
            DDRB |= (1<<PB6);
            OCR1A = (valueY);
        }else{
            DDRB &= ~(1<<PB5);
            DDRB |= (1<<PB6);
            OCR1B = (abs(valueY));
        }
    }
}

void z_axis_manual(void){
    if(!(PIN_Switch_Joystick & (1 << pin_Switch_Joystick))){
        _delay_ms(50);
                                                    printf("z_manual\n");
        switch(z){
        case 2:
            port_HBridgeZRight &= ~(1 << pin_HBridgeRightZ);
            port_HBridgeZLeft &= ~(1 << pin_HBridgeLeftZ);
            z = 0;
                                                    printf("z_manual_stil\n");
        break;

        case 0:
            port_HBridgeZRight &= ~(1 << pin_HBridgeRightZ);
            port_HBridgeZLeft |= (1 << pin_HBridgeLeftZ);
            z = 1;
                                                    printf("z_manual_links\n");
        break;

        case 1:
            port_HBridgeZLeft &= ~(1 << pin_HBridgeLeftZ);
            port_HBridgeZRight |= (1 << pin_HBridgeRightZ);
            z = 2;
                                                    printf("z_manual_rechts\n");
        break;
        }
        while((PIN_Switch_Joystick & (1 << pin_Switch_Joystick)));
        _delay_ms(50);
    }
}
